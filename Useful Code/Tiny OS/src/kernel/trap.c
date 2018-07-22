/*
 * higher level CPU trap handling
 * 
 * $Id: TRAP.C 1.2 1996/12/15 08:37:49 bart Exp $
 *
 */

#include <types.h>
#include <sys/thread.h>
#include <sys/pgfault.h>
#include <i386/inlines.h>

extern struct tcb* cur_tcb;
extern uint copy_msg_pf;

/*
 * protos
 */
void dump_regs(struct eframe*);
static void user_pf(struct eframe*);
static void kern_pf(struct eframe*);

/*
 * page fault lock
 */
ulong pf_lock = 0;
static ulong cr2 = 0;

void 
trap_pagefault(ulong stack)
{
	struct eframe* f = (struct eframe*)&stack;
	ushort* pvid = (ushort*)0xb8000;

	cli();
	cr2 = get_cr2();

	if(USERMODE(f))
		user_pf(f);
 	else
		kern_pf(f);
}

static void 
user_pf(struct eframe* f)
{
	printf("\nPage Fault in user mode.\n");
	printf("CR2: %x, error code: %x\n", cr2, f->ecode); 
	dump_regs(f);
	kill_proc(cur_tcb->pid, -1);
	schedule(f);
}

static void 
pflock_msgclnt()
{
	printf("PAGE FAULT occurred while copying message...\n");
	PF_UNLOCK;
	kill_proc(cur_tcb->pid, -1);
	printf("Client process killed\n");
	schedule(&cur_tcb->f);
}

static void 
pflock_msgsrv()
{
	printf("PAGE FAULT occurred while copying msgs...\n");
	PF_UNLOCK;
	kill_proc(cur_tcb->pid, -1);
	printf("Server process killed\n");
	schedule(&cur_tcb->f);
}

static void 
kern_pf(struct eframe* f)
{
	printf("\nFatal! page fault while in kernel mode.\n");
	printf("CR2: %x, error code: %x\n", cr2, f->ecode);
	if(pf_lock) {
		switch(pf_lock) {
			case PFL_MSGCLNT:
				pflock_msgclnt();
				break;

			case PFL_MSGSRV:
				pflock_msgsrv();
				break;

			default:
				printf("whoa! wierd pf_lock\n");
				break;
		}
	}
	/*
	 * doh! this is not cool ...
	 */ 
	dump_regs(f);
	printf("report this bug to: bart@sgh.waw.pl.\n");
	printf("thank you and good night.\nabout to reboot...");
	if(kyesno())	
		reboot();
	while(1);
}

void
dump_regs(struct eframe* f)
{
   printf("eax: %x ebx: %x ecx: %x edx: %x\n", f->eax, f->ebx, f->ecx, f->edx);
   printf("eip: %x esp: %x cr3: %x\n", f->eip, f->esp, get_cr3());
}
