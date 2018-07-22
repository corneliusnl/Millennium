/*
 * intel task structure handling
 * 
 * $Id: TSS.C 1.1 1996/11/26 15:43:41 bart Exp $
 *
 */

#include <types.h>
#include <sys/tss.h>
#include <sys/desc.h>
#include <sys/mem.h>

extern ulong pd[];
extern struct sys_desc gdt[];

struct tss ktss;

/*
static ulong ktss_stk[512];
ulong kstack = (ulong)(void*)&ktss_stk[510];
*/

ulong kstack = (ulong)(void*)(0x40000 - 0x4000);

static void init_tss_desc(uint, ulong);
static void ktss_setup(struct tss*, ulong);

void init_ktss()
{
	init_tss_desc(KTSS_SEL, (ulong)(void*)&ktss);
	ktss_setup(&ktss, kstack);

	/*
	printf("init_ktss(): kstack = %d\n", kstack);
	*/
}	
	
void 
panic(char* msg)
{
	cli();
	printf("%s\n", msg);
	while(1);
}
	
static void 
init_tss_desc(uint sel, ulong tss_addr)
{
	uint i = sel/8;

	gdt[i].base_0_15 = tss_addr;
	gdt[i].base_16_23 = (tss_addr >> 16);
	gdt[i].base_24_31 = (tss_addr >> 24);
	gdt[i].limit = sizeof(struct tss) - 1;
	gdt[i].gav_lim = 0x00;
	gdt[i].dpl_type = 0xE9;
}

static void 
ktss_setup(struct tss* ptss, ulong stk)
{
	ptss->eip = (ulong)(void*)&panic;
	ptss->esp = stk;
	ptss->cs = 0x08;
	ptss->ds = 0x10;
	ptss->es = 0x10;
	ptss->fs = 0x10;
	ptss->gs = 0x10;
	ptss->ss = 0x10;
	ptss->ss0 = 0x10;
	ptss->esp0 = stk;
	ptss->trap = 0x00;
	ptss->iomap = 0x00FF;
	ptss->eflags = 0x00000202;
	ptss->cr3 = (ulong)(void*)pd;
}

