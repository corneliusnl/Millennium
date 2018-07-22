/*
 * thread handling data definitions
 */
#ifndef _THREAD_H_
#define _THREAD_H_

#include <types.h>
#include <i386/frame.h>
#include <i386/setjmp.h>

#define USER_DSEG    (0x23)
#define USER_CSEG    (0x1B)
#define USER_EFLAGS  (0x00000202)

#define USERMODE(f)  ((f)->ecs & 0x3)

/*
 * thread states
 */
#define TS_READY     (0x01)
#define TS_WAITING   (0x02)
#define TS_DEAD      (0x03)
#define TS_IOPENDING (0xF0)

/*
 * thread control commands - thrctl syscall
 */
#define TC_KILL		(0x02)
#define TC_SLEEP	(0x04)
#define TC_WAKEUP	(0x08)
#define TC_SETPRIO	(0x10)

/*
 * some params
 */
#define MAX_THREADS  (64)  /* max # of threads in the system   */
#define NSTKPGS      (4)   /* # pages for thread stack         */
#define TIMESLICE    (3)   /* timeslice                        */
#define DEFAULT_PRIO (10)  /* default thread priority          */
#define MAX_PRIO     (50)  /* max priority		       */	 	

/*
 * thread control block
 */
struct tcb {
   struct frame   f;

   ulong    ticks;      /* ticks collected so far           */
   int      prio;       /* priority                         */
   int      cnt;        /* current count                    */

   ulong    tid;        /* thread id                        */
   ulong    pid;        /* id of process thread belongs to  */
   ulong    pa_pd;      /* phys addr of page dir            */

   ulong    stk;        /* stack pointer                    */
   ulong    kstack;     /* kernel stack pointer             */
   jmp_buf  ctx;        /* context                          */

   struct tcb* plink;   /* process link                     */
   struct tcb* next;    /* scheduler link                   */

   ulong    state;      /* state: ready, waiting and stuff  */
};


#endif /* _THREAD_H_ */
