/*
 * interrupt descriptor table
 *
 * $Id: IDT.C 1.3 1996/12/15 08:35:01 bart Exp $
 *
 */

#include <sys/desc.h>
#include <types.h>
#include <sys/thread.h>
#include <sys/syscall.h>

extern struct gate_desc idt[];

/*
 * assembler isr stubs
 */
extern void kb_isr(void);
extern void spur_isr(void);
extern void clock_isr(void);
extern void _wd_isr(void);
extern void _fd_isr(void);

extern void _trap_pagefault(void);

/*
 * system calls stubs
 */
extern void SYSCALL(kernstat);
extern void SYSCALL(getch);
extern void SYSCALL(exit);
extern void SYSCALL(open);
extern void SYSCALL(read);
extern void SYSCALL(write);
extern void SYSCALL(seek);
extern void SYSCALL(close);
extern void SYSCALL(chdir);
extern void SYSCALL(getcwd);
extern void SYSCALL(exec);
extern void SYSCALL(pctl);
extern void SYSCALL(thrctl);

extern void SYSCALL(allocmsg);
extern void SYSCALL(connectmsg);
extern void SYSCALL(sendmsg);
extern void SYSCALL(pgalloc);
extern void SYSCALL(waitpid);
extern void SYSCALL(reply);
extern void SYSCALL(wait);

extern void SYSCALL(mmap);
extern void SYSCALL(munmap);

extern void SYSCALL(sleep);
extern void SYSCALL(wakeup);

extern void SYSCALL(exec2);

/*
 * trap types
 */
#define     TRAP_GATE      (0x8F00)
#define     IRQ_GATE       (0x8E00)

#define  USER_TRAP_GATE    (0xEF00)

#define TRAP_STUB(name, msg) \
   void _x##name() { print(##msg); while(1); } 
   
TRAP_STUB(divide, "exception: divide by zero\n")
TRAP_STUB(debug, "exception: debug\n")
TRAP_STUB(nmi, "exception: nmi\n")
TRAP_STUB(breakpoint, "exception: breakpoint\n")
TRAP_STUB(overflow, "exception: overflow\n")
TRAP_STUB(bounds, "exception: bounds\n")
TRAP_STUB(opcode, "exception: opcode\n")
TRAP_STUB(mathgone, "exception: mathgone\n")
TRAP_STUB(double, "exception: double\n")
TRAP_STUB(mathover, "exception: mathover\n")
TRAP_STUB(tss, "exception: tss\n")
TRAP_STUB(segment, "exception: segment\n")
TRAP_STUB(stack, "exception: stack\n")
TRAP_STUB(general, "exception: general\n")
TRAP_STUB(page, "exception: page\n")
TRAP_STUB(matherr, "exception: matherr\n")

#define TRAP_ENTRY(name) (ulong)(void*)&_x##name

extern void trap_pagefault(void);

#define IRQ(x) ((x)+32)

static void 
setup_gate(int i, ulong p_hand, ushort type)
{
   idt[i].offset_0 = p_hand;
   idt[i].selector = 0x08;
   idt[i].type = type;
   idt[i].offset_16 = (p_hand >> 16);
}

void 
init_idt()
{
   int c;

   /*
    * on init, fill with spurious isr
    */
   for(c = 0; c < 256; c++) 
      setup_gate(c, (ulong)(void*)&spur_isr, TRAP_GATE);

   /* 
    * CPU exceptions 
    */

   setup_gate(0, TRAP_ENTRY(divide), TRAP_GATE);
   setup_gate(1, TRAP_ENTRY(debug), TRAP_GATE);
   setup_gate(2, TRAP_ENTRY(nmi), TRAP_GATE);
   setup_gate(3, TRAP_ENTRY(breakpoint), TRAP_GATE);
   setup_gate(4, TRAP_ENTRY(overflow), TRAP_GATE);
   setup_gate(5, TRAP_ENTRY(bounds), TRAP_GATE);
   setup_gate(6, TRAP_ENTRY(opcode), TRAP_GATE);
   setup_gate(7, TRAP_ENTRY(mathgone), TRAP_GATE);
   setup_gate(8, TRAP_ENTRY(double), TRAP_GATE);
   setup_gate(9, TRAP_ENTRY(mathover), TRAP_GATE);
   setup_gate(10, TRAP_ENTRY(tss), TRAP_GATE);
   setup_gate(11, TRAP_ENTRY(segment), TRAP_GATE);
   setup_gate(12, TRAP_ENTRY(stack), TRAP_GATE);
   setup_gate(13, TRAP_ENTRY(general), TRAP_GATE);
   setup_gate(14, (ulong)(void*)&_trap_pagefault, TRAP_GATE);
   setup_gate(16, TRAP_ENTRY(matherr), TRAP_GATE);

   /* 
    * hardware IRQs 
    */

   setup_gate(32, (ulong)(void*)&clock_isr, IRQ_GATE);
   setup_gate(33, (ulong)(void*)&kb_isr, IRQ_GATE);
   setup_gate(38, (ulong)(void*)&_fd_isr, IRQ_GATE);
   setup_gate(46, (ulong)(void*)&_wd_isr, IRQ_GATE);
   setup_gate(47, (ulong)(void*)&_wd_isr, IRQ_GATE);
   
   /* 
    * system calls 
    */

   setup_gate(SYSCALL_KERNSTAT, (ulong)&SYSCALL(kernstat), USER_TRAP_GATE);
   setup_gate(SYSCALL_GETCH, (ulong)&SYSCALL(getch), USER_TRAP_GATE);
   setup_gate(SYSCALL_EXIT, (ulong)&SYSCALL(exit), USER_TRAP_GATE);
   setup_gate(SYSCALL_OPEN, (ulong)&SYSCALL(open), USER_TRAP_GATE);
   setup_gate(SYSCALL_READ, (ulong)&SYSCALL(read), USER_TRAP_GATE);
   setup_gate(SYSCALL_WRITE, (ulong)&SYSCALL(write), USER_TRAP_GATE);
   setup_gate(SYSCALL_SEEK, (ulong)&SYSCALL(seek), USER_TRAP_GATE);
   setup_gate(SYSCALL_CLOSE, (ulong)&SYSCALL(close), USER_TRAP_GATE);
   setup_gate(SYSCALL_CHDIR, (ulong)&SYSCALL(chdir), USER_TRAP_GATE);
   setup_gate(SYSCALL_GETCWD, (ulong)&SYSCALL(getcwd), USER_TRAP_GATE);
   setup_gate(SYSCALL_EXEC, (ulong)&SYSCALL(exec), USER_TRAP_GATE);
   setup_gate(SYSCALL_PCTL, (ulong)&SYSCALL(pctl), USER_TRAP_GATE);
   setup_gate(SYSCALL_THRCTL, (ulong)&SYSCALL(thrctl), USER_TRAP_GATE);


   setup_gate(SYSCALL_ALLOCMSG, (ulong)&SYSCALL(allocmsg), USER_TRAP_GATE);
   setup_gate(SYSCALL_CONNECTMSG, (ulong)&SYSCALL(connectmsg), USER_TRAP_GATE);
   setup_gate(SYSCALL_SENDMSG, (ulong)&SYSCALL(sendmsg), USER_TRAP_GATE);
   setup_gate(SYSCALL_WAITPID, (ulong)&SYSCALL(waitpid), USER_TRAP_GATE);
   setup_gate(SYSCALL_REPLY, (ulong)&SYSCALL(reply), USER_TRAP_GATE);
   setup_gate(SYSCALL_WAIT, (ulong)&SYSCALL(wait), USER_TRAP_GATE);

   setup_gate(SYSCALL_MMAP, (ulong)&SYSCALL(mmap), USER_TRAP_GATE);
   setup_gate(SYSCALL_MUNMAP, (ulong)&SYSCALL(munmap), USER_TRAP_GATE);

   setup_gate(SYSCALL_SLEEP, (ulong)&SYSCALL(sleep), USER_TRAP_GATE);
   setup_gate(SYSCALL_WAKEUP, (ulong)&SYSCALL(wakeup), USER_TRAP_GATE);

   setup_gate(SYSCALL_EXEC2, (ulong)&SYSCALL(exec2), USER_TRAP_GATE);
}
   
