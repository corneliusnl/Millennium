/*
 * call gate interface
 *
 * $Id: GATE.C 1.1 1996/11/26 15:41:22 bart Exp $
 *
 */
#include <types.h>
#include <sys/desc.h>
#include <sys/thread.h>

#define DIV8(x)	((x)>>3)

#define TEST_CALL_GATE	(0x30)

extern struct sys_desc gdt[];
extern _system_call();

void call_gate(uint sel, ulong pa, uint nparms)
{
	struct gate_desc* cg = (struct gate_desc*) &gdt[DIV8(sel)];
	
	cg->offset_0 = pa;
	cg->selector = 0x08;
	cg->type = 0xEC00;
	cg->offset_16 = (pa >> 16);
}

void init_call_gates()
{
	call_gate(TEST_CALL_GATE, (ulong)(void*)&_system_call, 0);
}

void system_call(ulong stack)
{
	ulong c;
	struct frame* f = (struct frame*)&stack;

	/*printf("cr3 is now: %d\n", get_cr3());
	printf("t->tid = %d\n", cur_tcb->tid);
	printf("f->ecs = %x, f->esds = %x, f->esp = %d\n",
		f->ecs, f->esds, f->esp);
	printf("f->_esp = %x, f->eip = %x, f->eflags = %x\n",
		f->_esp, f->eip, f->eflags);
	printf("f = %d\n", (ulong)f);*/

	c = 100000000;
	while(c--);

	printf("returning to user...\n");

	/* return value to user */
	f->eax = 10;
}

