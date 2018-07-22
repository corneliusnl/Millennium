/*
 * Virtual memory manager
 *
 * $Id: VMM.C 1.3 1996/12/15 08:38:04 bart Exp $
 *
 */

#include <types.h>
#include <sys/mem.h>
#include <sys/proc.h>
#include <sys/vmm.h>
#include <i386/inlines.h>

#define MUL_PAGESZ(x)   ((x) << 12)
#define DIV_PAGESZ(x)   ((x) >> 12)

#define MUL_NPTE(x)  ((x) << 10)
#define DIV_NPTE(x)  ((x) >> 10)

#define MUL_4M(x) ((x) << 22)
#define DIV_4M(x) ((x) >> 22)

#define NPTESHIFT (10)

/*
 * pages to rounded page tables
 */
#define ptorpt(x) (((ulong)(x) + (NPTE-1)) >> NPTESHIFT)

/*#define _DEBUG*/
#include <trace.h>
#undef _DEBUG

/*
 * finds first free region of continuous 'npg' pages in page table 'pt'
 * returns starting index in 'pt'
 * I know it sucks but it works
 */
static int
first_fit(ulong* pt, int npg, int from)
{
   long start = -1;
   long count = 0;
   long big_count = 0;
   long big_start = 0;

   int c;

   TRACE("first_fit...\n");

   for(c = from; c < NPTE; c++) {
      if(pt[c] == FREE_PTE) {
         if(start < 0) {   
            start = c;
         }
         count++;
      } else {
         if(count > big_count) {
            big_count = count;   
            big_start = start;
            if(count >= npg) {
               return start;
            }
         }
         start = -1;
         count = 0;
      }
   }
   if(count >= npg)
      return start;

   return (-1);
}

/*
 * allocate new page table at page dir index 'i'
 */
static ulong
vm_new_pt(struct pcb* p, ulong i)
{
   uint c;
   ulong page;
   ulong* pd = (ulong*)VA_MAPD;
   ulong* vas = (ulong*)VA_VAS;
   ulong* vas_pt = vas + NPTE;
   ulong* pt;

   TRACE("vm_new_pt()\n");

   /*
    * first, alloc a page, put its phys addr in page dir
    * and map it in vas
    */
   page = mem_alloc();

   pd[i] = page | PA_USER;
   vas_pt[i] = page | PA_KERN;

   TRACE("vm_new_pt(): new page allocated\n");

   /*
    * refresh TLB
    */
   ldcr3(p->pa_pd);

   /*
    * calc virtual addr of newly alloc'd pt
    */
   pt = vas + MUL_NPTE(i);

   /*
    * initialize it
    */
   for(c = 0; c < NPTE; c++)
      pt[c] = FREE_PTE;

   return(page);
}

/*
 * free page table at index i
 */
static void
vm_free_pt(struct pcb* p, ulong i)
{
   ulong* pd = (ulong*)VA_MAPD;
   ulong* vas = (ulong*)VA_VAS;
   ulong* vas_pt = vas + NPTE;
   ulong* pvas = vas + MUL_NPTE(i);

   ulong c = NPTE;
   while(c--) {
      if(PADDR(*pvas) != FREE_PTE) {
         mem_free(PADDR(*pvas));
         *pvas = FREE_PTE;
      }
      pvas++;
   }
   mem_free(PADDR(pd[i]));

   /*
    * this is important to mark it as free
    * since next allocations will look for it
    */
   vas_pt[i] = FREE_PTE;
}

/*
 * maps 'npg' pages at page table 'pt' starting from 'from'
 * either allocates physical addresses or just marks the region
 * dirty gotos are for better performance
 */
static void
map_region(ulong* pt, uint from, uint npg, uint map_type)
{
   int c;
   uint mark;

   switch(map_type) {
      case VM_ALLOC:
      case VM_COMMIT:
         goto __alloc_region;
         break;
      case VM_FREE:
         mark = FREE_PTE;
         goto __free_region;
         break;
      case VM_RESERVE:
         mark = RESERVED_PTE;
         goto __mark_region;
         break;
      case VM_SETFREE:
         mark = FREE_PTE;
         goto __mark_region;
         break;
      case VM_DECOMMIT:
         mark = RESERVED_PTE;
         goto __free_region;
         break;
      default:
         return;
   };

__alloc_region:
   for(c = from; c < from + npg; c++)
      pt[c] = mem_alloc() | PA_USER;
   return;

__mark_region:
   for(c = from; c < from + npg; c++)
      pt[c] = mark;
   return;

__free_region:
   for(c = from; c < from + npg; c++) {
      mem_free(PADDR(pt[c]));
      pt[c] = mark;
   }
}

/*
 * checks for free entries in given page table
 */
static int
is_free(ulong* pt, uint from, uint to)
{
   uint c;

   for(c = from; c < to; c++)
      if(pt[c] != FREE_PTE)
         return (0);
   return (1);
}

/*
 * convert page dir index and page table index 
 * into virtual address
 */
static ulong
ii2va(int ipd, int ipt)
{
   return(MUL_4M(ipd) + MUL_PAGESZ(ipt));
}

/*
 * alloc some virtual memory
 */
ulong
vmm_alloc(struct pcb* p, uint npg, uint alloc_type)
{
   ulong* pd = (ulong*)VA_MAPD;
   ulong* vas = (ulong*)VA_VAS;
   int ipd = PD_CODE;
   int free = -1;
   ulong va = 0;

   if(npg >= NPTE) {
      /*
       * large allocations >= 4M
       */
      uint c;
      uint npt;   	/* total page table we need to find'n'alloc */
      uint npt_whole;   /* page tables that we fully allocate with pages */
      uint npt_rest; 	/* pages left */

      /*
       * round up to whole page dirs
       */
      npt = npt_whole = DIV_NPTE(npg);
      npt_rest = npg % NPTE;
      if(npt_rest)
         npt++;

      /*
       * find free region in page dir
       */
      TRACE4("npg >= NPTE: %d pages, %d PTs, %d PTs wholly alloc'd, %d pgs rest\n", 
         npg, npt, npt_whole, npt_rest);
      free = first_fit(pd, npt, ipd);
      if(free == -1) {
         printf("cannot allocate\n");
         return (NULL);
      }
      TRACE1("free = %d\n", free);
      
      /*
       * alloc every page table, and alloc all pages in it
       */
      for(c = free; c < free + npt_whole; c++) {
         vm_new_pt(p, c);
         map_region(vas + MUL_NPTE(c), 0, NPTE, alloc_type);
      }
      if(npt_rest) {
         vm_new_pt(p, c);
         map_region(vas + MUL_NPTE(c), 0, npt_rest, alloc_type);
      }
      
      va = ii2va(free, 0);
      TRACE1("va = %x\n", va);

   } else {
      /*
       * smaller allocations
       */
      TRACE("npg < NPTE\n");
      TRACE1("debug: pd[ipd] = %x\n", pd[ipd]);
      TRACE1("debug: PADDR(pd[ipd]) = %x\n", PADDR(pd[ipd]));

      /*
       * try every page table entry in page dir
       */
      while(1) {
         /*
          * yeah, it's free - just alloc it (tm)
          */
         if(PADDR(pd[ipd]) == FREE_PTE) {
            TRACE1("whole page table is free at %d\n", ipd);
            vm_new_pt(p, ipd);
            free = 0;
            break;
         }

         /*
          * find first fittin' region
          */
         free = first_fit(vas + MUL_NPTE(ipd), npg, 0);
         if(free >= 0)
            break;

         /*
          * setup to try next page table entry
          * if we reached end of page dir - that sucks!
          */
         if(++ipd >= NPTE) {
            printf("fatal: cannot allocate\n");
            return (NULL);
         }
      }

      /*
       * free holds first free page index
       * ipd  - index in page dir
       */

      map_region(vas + MUL_NPTE(ipd), free, npg, alloc_type);
      va = ii2va(ipd, free);

      TRACE1("ipd  = %d\n", ipd);
      TRACE1("free = %d\n", free);
      TRACE1("va = %x\n", va);

      ldcr3(p->pa_pd);
   }

   return (va);
}

/*
 * free memory
 */
int
vmm_free(struct pcb* p, ulong va, uint npg, uint free_type)
{
   ulong* pd = (ulong*)VA_MAPD;
   ulong* vas = (ulong*)VA_VAS;

   uint ipd = IPD(va);
   uint ipt = IPT(va);

   TRACE1("free_region(): va = %x\n", va);
   TRACE2("ipd = %d, ipt = %d\n", ipd, ipt);

   if(npg >= NPTE) {
      int c;
      int npt;
      int npt_whole;
      int npt_rest;

      npt = npt_whole = npg / NPTE;
      npt_rest = npg % NPTE;
      if(npt_rest)
         npt++;

      TRACE1("large dealloc, %d page tables\n", npt);

      for(c = ipd; c < ipd + npt_whole; c++) {
         map_region(vas + MUL_NPTE(c), 0, NPTE, free_type);
         vm_free_pt(p, c);
      }
      if(npt_rest) {
         map_region(vas + MUL_NPTE(c), 0, npt_rest, free_type);
         /*
          * do not free the page table as there might be other
          * allocations here
          *
          * if(is_free(vas + MUL_NPTE(c), npt_rest, NPTE))
          * vm_free_pt(p, c);
          */
      }

   } else {
      map_region(vas + MUL_NPTE(ipd), ipt, npg, free_type);
   }

   ldcr3(p->pa_pd);
   
   return (1);
}

/*
 * setup sharing area
 * XXX does this work?
 */
ulong
vm_share_pgs(struct pcb* p, ulong* pas, uint npg)
{
   /*
    * alloc continuous space
    */
   ulong* pd = (ulong*)VA_MAPD;
   ulong* vas = (ulong*)VA_VAS;
   int ipd = PD_CODE;
   int free = -1;
   ulong va = 0;
   ulong* pt;
   uint c;
   
   if(npg >= NPTE) {
      printf("cannot share, too many pages\n");
      return (NULL);
   }

   while(1) {
      /*
       * yeah, it's free - just alloc it (tm)
       */
      if(PADDR(pd[ipd]) == FREE_PTE) {
         TRACE1("whole page table is free at %d\n", ipd);
         vm_new_pt(p, ipd);
         free = 0;
         break;
      }
      /*
       * find first fittin' region
       */
      free = first_fit(vas + MUL_NPTE(ipd), npg, 0);
      if(free >= 0)
         break;
      /*
       * setup to try next page table entry
       * if we reached end of page dir - that sucks!
       */
      if(++ipd >= NPTE) {
         printf("fatal: cannot allocate\n");
         return (NULL);
      }
   }
   /*
    * free holds first free page index
    * ipd  - index in page dir
    */

   /*
    * fill it up with physical addresses
    */

   pt = vas + MUL_NPTE(ipd);
   for(c = free; c < free + npg; c++)
      pt[c] = *pas++;

   va = ii2va(ipd, free);

   /*
    * refresh TLB
    */
   ldcr3(p->pa_pd);  

   return (va);
}

#ifdef THIS
static void
vmm_dump(ulong* p)
{
	int c = 8;
	while(c--) 
		printf("%x,", *p++);
	printf("\n");
}
#endif

#define MAX_ADDR 1024
static ulong pages[MAX_ADDR];

void
vmm_copy(struct pcb* from, struct pcb* to, 
	 void* va_from, void* va_to, uint npg)
{
   uint c, i;
   ulong* va, *p;

   if(npg > MAX_ADDR) {
	printf("vmm_copy(): npg > MAX_ADDR\n");
	return;
   }

   if(get_cr3() != from->pa_pd)
	ldcr3(from->pa_pd);

   va = (ulong*)VA_VAS;
   i = (MUL_NPTE(IPD(va_from))) + IPT(va_from);
   va += i;

#ifdef THIS
   printf("vmm_copy(): i=%d\n", i);
   printf("vmm_copy(): source dump:\n");
   vmm_dump(va);
   kyesno();
   printf("vmm_copy(): getting addresses...\n");
#endif

   c = npg;
   p = pages;
   while(c--) {
      *p++ = *va++;	
   }

   ldcr3(to->pa_pd);
   va = (ulong*)VA_VAS;
   i = (MUL_NPTE(IPD(va_to))) + IPT(va_to);
   va += i;

#ifdef THIS
   printf("vmm_copy(): ok, we're in new address space..\n");
   printf("vmm_copy(): now reserving region for %d pages\n", npg);
#endif

   vmm_alloc(to, npg, VM_RESERVE);

#ifdef THIS
   va = (ulong*)VA_VAS;
   va += i;
   printf("vmm_copy(): ok, reserved.\n");
   printf("vmm_copy(): dest dump:\n");
   vmm_dump(va);
   kyesno();
#endif

   c = npg;
   p = pages;
   while(c--)
	*va++ = *p++;

#ifdef THIS
   printf("vmm_copy(): addresses copied.\n");
   kyesno();
   va = (ulong*)VA_VAS;
   va += i;
   printf("vmm_copy(): dest dump:\n");
   vmm_dump(va);
#endif
}

