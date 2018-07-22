/*
 * physical memory manager
 *
 * $Id: PMM.C 1.2 1996/12/04 16:08:49 bart Exp $
 *
 */

#include <types.h>
#include <sys/mem.h>

//#define _DEBUG
#include <trace.h>

/*
 * page descriptor
 */
struct page_desc {
   ulong                flags;
   struct page_desc*    next;
};

/*
 * physical pages database
 */
static struct page_desc* page_db = NULL;
static uint pdbsz;
static uint pdbsz_pgs;

static uint phys_mem;
static uint swap_mem;
static uint total_mem;
static uint phys_pgs;
static uint swap_pgs;
static uint total_pgs;
static uint kernel_pgs;
static uint alloc_pgs;

/*
 * allocation physical base address
 * depends on size of page database
 */
static uint base;

static uint heap_end;

/*
 * list of free pages
 */
static struct page_desc* free_pgs = NULL;
static uint nfree = 0;

/*
 * initialize physical memory manager
 */
void
pmm_init(uint physz, uint swapsz)
{
   uint c;

   /*
    * get heap end
    * this is our page database start
    */
   heap_end = get_heap_end();

   phys_mem = physz;
   swap_mem = swapsz;
   total_mem = phys_mem + swap_mem;

   phys_pgs = physz/PAGESZ;
   swap_pgs = swapsz/PAGESZ;
   total_pgs = phys_pgs + swap_pgs;

   printf("phys_mem: %d(%d); swap_mem: %d(%d); total_mem: %d(%d)\n", 
           phys_mem, phys_pgs, swap_mem, swap_pgs, total_mem, total_pgs);
   printf("heap_end: %x\n", heap_end);

   /*
    * calc how many pages kernel has eaten
    */
   kernel_pgs = heap_end/PAGESZ;
   TRACE1("kernel_pgs = %d\n", kernel_pgs);

   /*
    * calc allocatable pages
    */
   alloc_pgs = total_pgs - kernel_pgs;
   TRACE1("alloc_pgs = %d\n", alloc_pgs);

   /*
    * calc page database size
    */
   pdbsz = alloc_pgs*sizeof(struct page_desc);
   pdbsz_pgs = btorp(pdbsz);

   TRACE2("pdbsz: %d, pdbsz_pgs: %d\n", pdbsz, pdbsz_pgs);

   /*
    * adjust allocatable pages no, it decreases by number of pages
    * page database eats
    */
   alloc_pgs -= pdbsz_pgs;
   TRACE1("alloc_pgs: %d\n", alloc_pgs);

   /*
    * now, in turn, adjust page directory size, since
    * the number of allocatable pages changed
    */
   pdbsz = alloc_pgs*sizeof(struct page_desc);
   pdbsz_pgs = btorp(pdbsz);
   TRACE2("pdbsz: %d, pdbsz_pgs: %d\n", pdbsz, pdbsz_pgs);

   /*
    * page database starts just after heap
    */
   page_db = (struct page_desc*)heap_end;

   /*
    * calc the base addr of allocations
    */
   base = heap_end + (pdbsz_pgs*PAGESZ);
   TRACE1("base: %x\n", base);
   TRACE1("base pgs + alloc_pgs = %d\n", btorp(base) + alloc_pgs);


   /*
    * ok, in the beginning all pages are on the free list
    */
   for(c = 0; c < alloc_pgs; c++) {
      page_db[c].flags = 0;
      page_db[c].next = &page_db[c+1];
   }
   page_db[c-1].next = NULL;
   TRACE1("&page_db[c-1] = %x\n", &page_db[c-1]);

   /*
    * all allocatable pages are ready to go
    */
   free_pgs = page_db;
   nfree = alloc_pgs;

   printf("page descriptor table: pdbsz: %d, base: %x, alloc: %d\n",
	  pdbsz, base, alloc_pgs);
}

void
pmm_dump()
{
   uint c = 70;
   while(c--)
      printf("-");
   printf("\n");

   for(c = 0; c < alloc_pgs; c++) {
      printf("&page_db[c] = %x; page_db[c].flags = %d; page_db[c].next = %x\n",
         (ulong)&page_db[c], page_db[c].flags, page_db[c].next);
   }
}

/*
 * get free page from free pages list
 */
static struct page_desc*
pg_alloc()
{
   struct page_desc* p = free_pgs;
   free_pgs = free_pgs->next;
   p->next = NULL;

   return(p);
}

/*
 * put a page to free pages list
 */
static void
pg_free(struct page_desc* p)
{
   p->next = free_pgs;
   free_pgs = p;
}

/*
 * alloc physical page
 */
ulong
pmm_alloc()
{
   struct page_desc* p = pg_alloc();
   ulong addr = (base + ((p - page_db) << 12));
   TRACE1("addr = %x\n", addr);

   nfree--;

   return(addr);
}

/*
 * free physical page
 */
void
pmm_free(ulong addr)
{
   ulong i = ((PADDR(addr) - base) >> 12);
   TRACE1("i = %d\n", i);
   pg_free(&page_db[i]);
   nfree++;
}
   
void
pmm_stat()
{
//   printf("npgs: %d; nfree: %d; pamsz: %d\n", npgs, nfree, pamsz);
}

ulong
mem_alloc()
{
   return pmm_alloc(); 
}

void
mem_free(ulong va)
{
   pmm_free(va);
}

uint
get_nfree()
{
   return(nfree);
}
