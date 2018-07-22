/*
 * kernel heap management
 *
 * $Id: HEAP.C 1.2 1996/12/04 16:08:15 bart Exp $
 *
 */

#include <assert.h>
#include <types.h>
#include <bcopy.h>
#include <sys/mem.h>

//#define _DEBUG
#include <trace.h>

/* 
 * kernel heap, right after 4th 64K till video
 * and after 1M till max. 2M
 */
#define KHEAP_START_LO     (0x040000)
#define KHEAP_END_LO       (0x0a0000)
#define KHEAP_START_HI     (0x100000)
#define KHEAP_END_HI       (0x200000)

#define KHEAP_SZ           ((KHEAP_END_LO-KHEAP_START_LO) \
                           +(KHEAP_END_HI-KHEAP_END_HI))

static uchar* pheap_lo = (void*)KHEAP_START_LO;
static uchar* pheap_hi = (void*)KHEAP_START_HI;

/*
 * kernel heap allocations, what a simple memory management :-)
 * page aligned, zeroed
 */
ulong
halloc(uint nb)
{
   ulong va;

   /*
    * page align
    */
   nb += (PAGESZ - (nb % PAGESZ));

   /*
    * first try low mem
    */
   if((ulong)pheap_lo + nb > KHEAP_END_LO) {
      /*
       * oops, no space in low, try high
       */
      if((ulong)pheap_hi + nb > KHEAP_END_HI) {
         panic("halloc(): outta kernel heap.\n");
      }
      va = (ulong)pheap_hi;
      pheap_hi += nb;
   } else {
      va = (ulong)pheap_lo;
      pheap_lo += nb;
   }

   bzero((void*)va, nb);

   TRACE2("halloc(): va = %x, nb = %d\n", (ulong)va, nb);

   return(va);
}

ulong
get_heap_ptr()
{
   return((ulong)pheap_lo);
}

ulong
get_heap_end()
{
   return((ulong)pheap_hi);
}
