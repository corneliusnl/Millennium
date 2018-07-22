/*
 * buffer cache
 *
 * $Id: BUF.C 1.5 1996/12/15 08:34:10 bart Exp $
 *
 */

#include <sys/buf.h>
#include <assert.h>

#define _DEBUG
#include <trace.h>
#undef _DEBUG

#define MAXBUF       	(400)
#define NHASH  		(32)
#define MAXIDS		(16)

static uint nbuf = 0;
static uint rootsz = 0;
static uint bufsz = 0;


static struct buffer_head  root_bh;
static struct buffer_head  buf_pool[MAXBUF];
static struct buffer_head* free_list;
static struct buffer_head* free_lists[MAXIDS];
static struct buffer_head* hash_table[NHASH];

struct buffer_funcs {
   rdblkfun read;
   wrblkfun write;
};

static struct buffer_funcs bufop[MAXIDS];

/*
 * buffer status flags
 */
#define  BS_LOCKED   (0x02)
#define  BS_VALID    (0x04)
#define  BS_BUSY     (0x08)
#define  BS_DIRTY    (0x10)

//#define BUFSTAT
#ifdef BUFSTAT
static uint read_req;
static uint write_req;
static uint dev_read;
static uint dev_write;
#endif

/*
 * protos
 */
void bwrite(struct buffer_head* bh);
void buf_dump();

/*
hash_func(uint dev, uint blk)
{
   return(((unsigned)dev^blk)%NHASH);
}
*/

#define hash_func(dev, blk)	(((unsigned)(dev)^(blk))%NHASH)

void
init_bufs(uint n, uint id, uint sz)
{
   int c;
   uint from = nbuf;
   uint to = from + n;
   struct buffer_head* bh = &buf_pool[from];
   uchar* ptr;

   nbuf += n;
   if(nbuf >= MAXBUF) {
   	nbuf -= n;
   	printf("init_bufs(): fatal!, nbuf >= MAXBUF\n");
   	return;
   }
 
   /*
    * alloc the whole pool of mem for buffers
    * since halloc() is page aligned, we would waste a lot of mem
    * in case of buffers of size less than a page (4K)
    */
   //printf("init_bufs(): %d bytes for buffers of size %d\n", sz*n, sz);
   ptr = (uchar*)halloc(sz * n);

   /*
    * now init all the buffers
    * maintaining free list links
    */
   for(c = from; c < to; c++) {
      bzero(bh, sizeof(struct buffer_head));
      bh->id = id;
      bh->sz = sz;
      //bh->pbuf = (uchar*)halloc(sz);
      bh->pbuf = ptr;
      ptr += sz;
      bh->prev_free = bh - 1;
      bh->next_free = bh + 1;
      bh++;
   }
   bh--;

   free_lists[id] = &buf_pool[from];
   free_lists[id]->prev_free = bh;
   bh->next_free = free_lists[id];

   for(c = 0; c > NHASH; c++)
      hash_table[c] = NULL;   
}

static void
insert_hash(struct buffer_head* bh, int ihash)
{
   bh->next = hash_table[ihash];
   hash_table[ihash] = bh;
   bh->next->prev = bh;
}

static void
remove_hash(struct buffer_head* bh)
{
   int ihash;

   if(bh->next)
      bh->next->prev = bh->prev;
   if(bh->prev)
      bh->prev->next = bh->next;

   ihash = hash_func(bh->dev, bh->blk);
   if(hash_table[ihash] == bh)
      hash_table[ihash] = bh->next;

   bh->next = bh->prev = NULL;
}

/*
 * enqueue at the end of free list
 */
static void
insert_free(struct buffer_head* bh)
{
   uint id = bh->id;
   ASSERT(free_lists[id]);

   bh->next_free = free_lists[id];
   bh->prev_free = free_lists[id]->prev_free;
   free_lists[id]->prev_free->next_free = bh;
   free_lists[id]->prev_free = bh;
}

static void
remove_free(struct buffer_head* bh)
{
   uint id = bh->id;
   ASSERT(free_lists[id]);

   bh->prev_free->next_free = bh->next_free;
   bh->next_free->prev_free = bh->prev_free;
   if(free_lists[id] == bh)
      free_lists[id] = bh->next_free;
   bh->next_free = bh->prev_free = NULL;
}

static struct buffer_head*
get_free(uint id)
{
   struct buffer_head* bh;

   bh = free_lists[id];
   if(bh == NULL)
   	return (NULL);

   /*
   if(bh->cnt) {
   printf("no free buffers of type[%d]\n", bh->id);
   return (NULL);
   }
   */

   remove_free(bh);
 
   /*
   if(free_lists[id]->next_free == free_lists[id]) {
      printf("get_free(): that was the last buf...\n");
      free_lists[id] = NULL;
   }
   */

   return (bh);
}

static struct buffer_head*
get_hashq(int dev, int blk, int ihash, uint id)
{
   struct buffer_head* bh = hash_table[ihash];

   for(bh = hash_table[ihash]; bh != NULL; bh = bh->next)
      if(bh->dev == dev && bh->blk == blk && bh->id == id)
         return (bh);

   return (NULL);
}

struct buffer_head*
getblk(uint dev, uint blk, uint id)
{
   int ihash = hash_func(dev, blk);
   struct buffer_head* bh;

   //cli();
   //printf("getblk(): ihash = %d\n", ihash);

   /*
    * first, try hash queue
    */
tryit:
   bh = get_hashq(dev, blk, ihash, id);
   if(bh) {
      //printf("getblk(): found a buffer on hashq: %d\n", (ulong)bh);
      if(bh->lock) {
         printf("getblk(): buffer is locked...\n");
         /*
          * sleep here until buffer is freed
          */
	 schedule(NULL);
	 printf("getblk(): ok, woke up, trying again..\n");
	 goto tryit;

         return (NULL);
      }
      /*
       * check if buffer was used
       * perform any pending action if needed (e.g. delay write)
       */
      if(bh->status & BS_DIRTY) {
         //printf("getblk(): buffer is dirty...\n");
      }

      /*
       * ok, we got the buffer
       */
      //printf("getblk(): ok, got the buffer, let's lock it\n");
      bh->lock = 1;

      remove_free(bh);
      //sti();
      return (bh);
   }

   /*
    * we didn't find buffer on hash queue
    * get new one
    */
   bh = get_free(id);
   if(!bh) {
      printf("getblk(): no free buffers\n");
      /*
       * sleep for any free buffer
       */
      return (NULL);
   }
   //printf("getblk(): got free buffer: %d\n", (ulong)bh);

   if(bh->status & BS_DIRTY) {
      //printf("getblk(): this buf is dirty...\n");
   }

   if(bh->dev == dev && bh->blk == blk && bh->id == id) {
      //printf("getblk(): ok, buffer got that block...\n");
   } else {

      //printf("getblk(): reassigning the buffer...\n");
      if(bh->status & BS_DIRTY) {
         //printf("getblk(): this buf is dirty and we gotta flush it now\n");
         //if(kyesno())
         bwrite(bh);
      }

      remove_hash(bh);

      bh->id = id;
      bh->dev = dev;
      bh->blk = blk;
      bh->status = 0;
      insert_hash(bh, ihash);
   }

   bh->lock = 1;
   bh->cnt++;

   //sti();
   return (bh);   
}

void
brelse(struct buffer_head* bh)
{
   if(!bh) {
      printf("brelse(): fatal, bh = NULL\n");
      return;
   }

   //printf("brelse(): bh->id=%d\n", bh->id);

   //cli();
   insert_free(bh);

   bh->lock = 0;
   bh->cnt--;

   //sti();
}

void
buf_dump()
{
   int c;
   struct buffer_head* b = buf_pool;

   for(c = 0; c < nbuf; c++) {
      printf("%d;[id]=%d;[sz]=%d;[pr]=%d;[nx]=%d;[pr_free]:%d;[nx_free]:%d\n",
               (ulong)b,  
               b->id, 
          b->sz,
               b->prev ? b->prev : 0, 
               b->next ? b->next : 0, 
               b->prev_free ? b->prev_free : 0, 
               b->next_free ? b->next_free : 0);
      b++;
   }
   //printf("free_list = %d\n", (ulong)free_list);

   printf("free_lists:\n");
   for(c = 0; c < nbuf; c++) {
   struct buffer_head* b = free_lists[c];
   if(free_lists[c] == 0)
      continue;

   printf("[id]=%d, list: ", free_lists[c]->id);
   while(1) {
      printf("%d, ", (ulong)b);
      b = b->next_free;
      if(b == free_lists[c] || b == NULL)
         break;
   }
   printf("\n");
   }
}

struct buffer_head*
bread(uint dev, uint blk, uint id)
{
   struct buffer_head* bh;

   //bh = (blk == ROOT_BLOCK) ? &root_bh : getblk(dev, blk, 1);
   //bh = (blk == ROOT_BLOCK) ? getblk(dev, blk, 1) : getblk(dev, blk, 2);
   bh = getblk(dev, blk, id);
   ASSERT(bh->id == id);

   if(bh == NULL) {
      printf("bread(): oops, getblk() gave us NULL...\n");
      return (NULL);
   }

#ifdef BUFSTAT
   read_req++;
#endif

   /*
    * that's what it's all about
    * if the buffer is valid, we don't needa read it
    */
   if(bh->status & BS_VALID) {
      //printf("bread(): buffer is valid...\n");
      return (bh);
   }

   //printf("bread(): reading block, bh=%d, [dev:%d;blk:%d;id:%d]\n", 
   //       (ulong)bh, bh->dev, bh->blk, bh->id);
   //dosfs_readblk(blk, bh->pbuf);
   ASSERT(bufop[id].read);
   bufop[id].read(bh);
   bh->status |= BS_VALID;

#ifdef BUFSTAT
   dev_read++;
#endif

   return (bh);
}

void
bwrite(struct buffer_head* bh)
{
   //dosfs_writeblk(bh->blk, bh->pbuf);

   ASSERT(bufop[bh->id].write);
   bufop[bh->id].write(bh);
   bh->status &= ~BS_DIRTY;

#ifdef BUFSTAT
   dev_write++;
#endif
}

void
buf_dirty(struct buffer_head* bh)
{
   bh->status |= BS_DIRTY;

#ifdef BUFSTAT
   write_req++;
#endif
}

void
buf_valid(struct buffer_head* bh)
{
   bh->status |= BS_VALID;
}

void
setbufop(uint id, rdblkfun read_func, wrblkfun write_func)
{
   bufop[id].read = read_func;
   bufop[id].write = write_func;
}

void
syncbufs()
{
   uint c;
   struct buffer_head* bh = &buf_pool[0]; 

   printf("syncing buffers...");
   if(!kyesno())
	return;

   wd_async(0);
   c = MAXBUF; 
   while(c--) {
      if((bh->id) && (bh->status & BS_DIRTY)) {
         //printf("dirty buf: %d, id: %d, blk: %d, sz: %d\n",
           //      (ulong)bh, bh->id, bh->blk, bh->sz);
         //if(kyesno())
         bwrite(bh);
      }
      bh++;
   }
}

void
bufzero()
{
#ifdef BUFSTAT
	read_req = write_req = 0;
	dev_read = dev_write = 0;
#endif
}

void
bufstat()
{
#ifdef BUFSTAT
	printf("read: %d / %d   write: %d / %d\n",
		read_req, dev_read, write_req, dev_write);
#endif
}
