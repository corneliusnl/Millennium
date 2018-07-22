/*
 * paging support
 *
 * $Id: PAGING.C 1.1 1996/11/26 15:42:16 bart Exp $
 *
 */

#include <types.h>
#include <i386/inlines.h>
#include <sys/mem.h>
#include <sys/kernel.h>

#include <trace.h>

extern ulong pd[];
extern ulong pt[];

extern void enable_paging();

static void 
init_kpt(ulong start_addr)
{
   ulong addr;
   int  c;

   c = 0;
   addr = start_addr;

   while(c < 1024) {
      pt[c++] = addr | PA_KERN;
      addr += PAGESZ;
   }
}

static void 
init_kpd()
{
   init_kpt(0x00000000);
   pd[0] = (ulong)(void*)&pt[0] | PA_KERN;
}

void 
init_kmem()
{
   init_kpd();

   TRACE1("pd addr = %d\n", (ulong)(void*)pd);

   ldcr3((ulong)pd);
   enable_paging();
}

void
kspace()
{
   ldcr3((ulong)&pd);
}
