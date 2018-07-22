/*
 * malloc() and family
 *
 * Ideas from VSTa and Linux
 * Copyright (c) 1996 Andy Valencia
 * Copyright (c) 1996 Linus Torvalds
 * Copyright (c) 1996 Bart Sekura
 *
 * For big allocations we use memory blocks. Smaller use buckets - memory 
 * areas divided into fixed, allocatable blocks. Both memory blocks 
 * and buckets use first page as a description. They use mmap, munap to
 * actually rip some real memory.
 *
 */

#include <stdlib.h>
#include <assert.h>
#include <types.h>
#include <sys/mem.h>
#include <sys/vmm.h>
#include <sys/calls.h>

//#define _DEBUG
#include <trace.h>
#undef _DEBUG

/*
 * memory block type magics
 * describes if we deal with block or bucket
 */
#define  MID_BLOCK   (0xAAAABBBB)
#define  MID_BUCKET  (0xCCCCDDDD)

/*
 * memory block descriptor
 * memblk is at least one page big, and always takes first extra page
 * for description
 */
struct memblk {
   ulong    magic;      /* indicates we're dealing with block */
   void*    va;         /* virtual address of a block */
   uint     npg;        /* number of pages in it */
   struct memblk* next; /* next block in a linked list */
};

/*
 * pool of memory blocks
 */
static struct memblk* pool = 0;
static uint nblocks = 0;

/*
 * bucket descriptor - for smaller allocations
 * also takes first page for description
 */
struct bucket_desc {
   ulong                      magic;   /* identity */
   void*                      va;      /* virtual addr */
   struct bucket_desc*        next;    /* list of desc */
   void*                      freeptr; /* free slot    */
   uint                       npg;     /* number of pg */
   uint                       refcnt;  /* reference    */
   uint                       sz;      /* size of slot */
   struct bucket_directory*   bdir;    /* parent bdir
                                          saves search time */
};

/*
 * directory of bucket descriptors grouped by their size
 * npg says how many pages bucket uses; for bigger bucket sizes
 * it's better to have more pages than one to maintain reasonable
 * overhead/actual space ratio
 */
struct bucket_directory {
   uint        		sz;
   uint        		npg;
   struct bucket_desc*  list;
};

/*
 * bucket directory groups bucket size categories
 * note last zeroed entry for easy search
 * XXX currently all categories use one page
 */
static struct bucket_directory bucket_dir[] = 
   { {   16, 1, (struct bucket_desc*) NULL },
     {   32, 1, (struct bucket_desc*) NULL },
     {   64, 1, (struct bucket_desc*) NULL },
     {  128, 1, (struct bucket_desc*) NULL },
     {  256, 1, (struct bucket_desc*) NULL },
     {  512, 1, (struct bucket_desc*) NULL },
     { 1024, 1, (struct bucket_desc*) NULL },
     { 2048, 1, (struct bucket_desc*) NULL },
     { 4096, 1, (struct bucket_desc*) NULL },
     {    0, 0, (struct bucket_desc*) NULL } };

/*
 * bucket pool
 */
static struct bucket_desc* bucket_pool = NULL;

/*
 * finds block that describes given pointer
 */
static struct memblk*
findblk(void* p)
{
   struct memblk* blk = pool;
   uint c = nblocks;

   while(c--) {
      if((blk->va <= p) && 
         (((char*)blk->va + ptob(blk->npg)) > (char*)p))
         return (blk);

      blk = blk->next;
   }
   
   return (NULL);
}

/*
 * alloc block of memory
 */
static void*
core_alloc(uint npg)
{
   struct memblk* m;
   void* p;

   TRACE1("core_alloc(): getting %d page(s)...\n", npg);
   p = mmap(npg+1, VM_ALLOC);
   
   if(p == NULL)
      return (NULL);

   TRACE1("core_alloc(): block desc: %d\n", (ulong)p);
   m = (struct memblk*)p;
   p += PAGESZ;
   m->magic = MID_BLOCK;
   m->va = p;
   m->npg = npg;
   m->next = pool;

   pool = m;

   nblocks++;

   return (p);
}

/*
 * allocate bucket in a given category (bdir)
 * return a pointer to initialized bucket descriptor structure
 */
struct bucket_desc*
bucket_alloc(struct bucket_directory* bdir)
{
   struct bucket_desc* b;
   void* p;
   ulong addr, c;

   TRACE("bucket_alloc(): getting pages...\n");

   /*
    * get real memory + 1 for overhead (bucket desc)
    */
   p = mmap(bdir->npg + 1, VM_ALLOC);
   TRACE1("bucket_alloc(): p = %x\n", (ulong)p);
   
   if(p == NULL)
      return (NULL);

   /*
    * fill in bucket desc structure
    */
   b = (struct bucket_desc*)p;
   p += PAGESZ;
   b->magic = MID_BUCKET;
   b->bdir = bdir;
   b->va = b->freeptr = p;
   b->npg = bdir->npg;
   b->sz = bdir->sz;
   b->refcnt = 0;

   /*
    * maintain list links
    */
   b->next = bdir->list;
   bdir->list = b;

   /*
    * setup freeptr stuff
    * end with null, to indicate this bucket is full
    */
   c = (PAGESZ * bdir->npg) / b->sz;
   TRACE1("bucket_alloc(): c = %d\n", c);
   addr = (ulong)p;
   while(c--) {
      *((void**)addr) = (c == 0) ? NULL : (void*)(addr + b->sz);
      addr += b->sz;
   }

   return (b);
}

static void
core_free(void* p)
{
   struct memblk* blk = pool;
   struct memblk** pprev = &pool;
   uint c = nblocks;

   if(c == 0) {
      TRACE("core_free(): no blocks allocated.\n");
      return;
   }

   p -= PAGESZ;

   while(c--) {
      if(blk == p) {
         *pprev = blk->next;
         TRACE("core_free(): found a block...\n");
         TRACE2("core_free(): addr: %d, pages: %d\n", (ulong)p, blk->npg+1);

         munmap(p, blk->npg + 1, VM_FREE);

         nblocks--;
         return;
      }

      pprev = &blk->next;
      blk = blk->next;
   }

   TRACE("core_free(): no block found...\n");   
}

/*
 * free memory from one of our buckets
 */
static void
bucket_free(void* p)
{
   struct bucket_desc* b;
   ulong addr;
   void* ptr;

#ifdef _DEBUG
   uint off, slot;
#endif

   /*
    * eliminate page offset
    */
   addr = (ulong)p;
   PALIGN(addr);

   /*
    * get pointer to bucket descriptor
    */
   ptr = (void*)addr;
   b = (struct bucket_desc*)(ptr - PAGESZ);
   ASSERT(b);

   TRACE2("bucket_free(): b->va = %d; b->sz = %d\n", (uint)b->va, b->sz);

#ifdef _DEBUG
   off = (uint)p - (uint)b->va;
   slot = ((uint)p - (uint)b->va) / b->sz;
   TRACE2("bucket_free(): off = %d; slot = %d\n", off, slot);
#endif

   /*
    * rearrange freeptr
    */ 
   *((void**)p) = b->freeptr;
   b->freeptr = p;

   /*
    * check refcnt i- if zero free the whole bucket
    */
   if(--b->refcnt == 0) {
      struct bucket_desc* bptr = b->bdir->list;
      struct bucket_desc** pprev = &b->bdir->list;

      TRACE("bucket_free(): refcnt = 0\n");
      /*
       * go thru bdesc list to find our desc, then free it
       */
      while(bptr) {
         if(bptr == b) {
            TRACE("bucket_free(): ok, found desc...\n");
            *pprev = bptr->next;
            munmap((void*)b, b->npg + 1, VM_FREE);

            return;
         }
         pprev = &bptr->next;
         bptr = bptr->next;
      }
      TRACE("bucket_free(): oops, didn't find bucket desc in dir.\n");
   }
}

void
malloc_stat()
{
   struct memblk* blk = pool;
   uint c = nblocks;

   while(c--) {
      if(blk->va)
         printf("va: %d;", (ulong)blk->va);
      if(blk->next)
         printf("next->va: %d;", (ulong)blk->next->va);
      else
         printf("next: NULL;");

      blk = blk->next;
   }
   printf("\n");
}

void
dump_pool()
{
   struct memblk* blk = pool;
   struct bucket_directory* bdir = bucket_dir;

   while(blk) {
      printf("blk->va: %d\n", blk->va);
      blk = blk->next;
   }
   while(bdir->sz) {
      if(bdir->list) {
         struct bucket_desc* b = bdir->list;
         printf("list for %d buckets...\n", bdir->sz);
         while(b) {
            ulong* p = (ulong*)b->va;
            ulong addr = (ulong)p;

            printf("\tb->va: %d; b->sz: %d; b->refcnt: %d; b->freeptr: %d\n", 
               b->va, b->sz, b->refcnt, (ulong)b->freeptr);
            /*
            printf("\tlist for b:\n");
            while(*p) {
               printf("%d,", *p);
               addr += b->sz;
               p = (ulong*)addr;
            }
            printf("\n");
            */

            b = b->next;
         }
      } else
         printf("empty list for %d\n", bdir->sz);

      bdir++;
   }
}

void*
malloc(size_t size)
{
   struct bucket_directory* bdir;
   struct bucket_desc* bdesc;
   void* ret;

   if(size == 0)
      return (NULL);

   if(size >= PAGESZ) {
      void* p;

      TRACE1("malloc(): allocating from core...(%d bytes)\n", size);
      p = core_alloc(btorp(size));
      return(p);
   }

   TRACE("malloc(): size less than a page, trying buckets...\n");

   for(bdir = bucket_dir; bdir->sz < size; bdir++) {
      if(!bdir->sz) {  /* we reached the end of dir */
         TRACE1("malloc(): size too big for a bucket (%d)\n", size);
         return (NULL);
      }
   }

   TRACE1("malloc(): found bucket_desc: sz = %d\n", bdir->sz);

   for(bdesc = bdir->list; bdesc != NULL; bdesc = bdesc->next) {
      if(bdesc->freeptr) {
         TRACE("malloc(): found freeptr in bdesc\n");
         goto found;
      }
   }
   
   TRACE("malloc(): no freeptr bucket desc, trying to allocate one\n");
   bdesc = bucket_alloc(bdir);

found:
   ret = bdesc->freeptr;
   bdesc->freeptr = *((void**)ret);
   bdesc->refcnt++;

   return (ret);
}

void
free(void* p)
{  
   ulong addr = (ulong)p;
   ulong* ptr;
   
   /*
    * first, eliminate page offset
    */
   //addr &= ~0x3FF;
   PALIGN(addr);
   TRACE1("free(): addr: %d\n", addr);

   /*
    * now, let's find our magic numbers
    */
   ptr = (ulong*)(addr - PAGESZ);
   TRACE1("free(): magic: %x\n", *ptr);

   if(*ptr == MID_BLOCK) {
      struct memblk* blk = findblk(p);
      if(blk) {
         TRACE("free(): blk found:\n");
         core_free(p);
      } else
         TRACE("free(): no block found...\n");

      return;
   }
   if(*ptr == MID_BUCKET) {
      TRACE("free(): freeing from bucket...\n");

      bucket_free(p);
      return;
   }

   TRACE1("free(): trying to deallocate weird object...(%d)\n", addr);
}
