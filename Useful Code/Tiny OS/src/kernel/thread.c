/*
 * thread support
 *
 * $Id: THREAD.C 1.4 1996/12/15 08:37:42 bart Exp $
 *
 */

#include <assert.h>
#include <types.h>
#include <bcopy.h>
#include <sys/tss.h>
#include <sys/thread.h>
#include <sys/proc.h>
#include <sys/mem.h>
#include <i386/inlines.h>

#include <trace.h>

#define FRAMESZ      (14)  

extern struct tss ktss;
extern ulong kstack;

/*
 * functions from switch.s
 */
extern void savestate(struct frame*, struct frame*);
extern void resume_current();
extern void resume_context(struct frame*);

static struct tcb tcb_pool[MAX_THREADS];

struct tcb* cur_tcb;
struct frame* cur_frame;

static struct tcb* last_tcb = tcb_pool;

static uint ntcb = 0;

struct kern_stat {
   ulong    uticks;  /* user ticks     */
   ulong    kticks;  /* kernel ticks   */
};

static struct kern_stat kstat = {0};

/*
 * allocates thread control block 
 * returns pointer to it. NULL - no free TCB
 */
struct tcb*
alloc_tcb()
{
   ulong c;
   struct tcb* t = tcb_pool;

   for(c = 0; c < MAX_THREADS; c++) {
      if(t->tid == 0) {
         t->tid = c+1;
         return (t);
      }
      t++;
   }
   return (NULL);
}

/*
 * creates new thread, sets up links, initializes thread registers
 * sets the newly created thread as current
 */
/*
struct tcb*
create_thread(struct pcb* p, ulong eip)
{
   struct tcb* t = alloc_tcb();

   TRACE1("t->tid=%d\n", t->tid);

   t->state = TS_READY;
   t->prio = DEFAULT_PRIO;
   t->cnt = t->prio;
   t->ticks = 0;
   t->pid = p->pid;
   t->pa_pd = p->pa_pd;

   p->last_t = t;
   t->plink = NULL;

   t->stk = alloc_tstack(p);  
   t->next = NULL;

   bzero(&t->f, sizeof(struct frame));
   t->f.eip = eip;
   t->f.esds = (USER_DSEG << 16) | USER_DSEG;
   t->f.ess = USER_DSEG;
   t->f.ecs = USER_CSEG;
   t->f.esp = t->f.ebp = (VA_STACK + ((t->stk+1)*PAGESZ) - 4);
   t->f.eflags = 0x0007202;

   t->next = last_tcb->next;
   last_tcb->next = t;
   last_tcb = t;

   ntcb++;

   TRACE4("tcb->tid: %d; tcb->f.eip: %x; tcb->stk: %x, tcb->f.esp: %x\n", 
           t->tid, t->f.eip, t->stk, t->f.esp);
   TRACE2("tcb->f.esds: %x; tcb->ecs: %x\n", t->f.esds, t->f.ecs);
   TRACE2("tcb->next->tid: %d; last_tcb->tid: %d\n", 
           t->next->tid, last_tcb->tid);
   dump_pd(p);

   cur_tcb = t;

   return(t);
}
*/

/*
 * creates new thread, sets up links, initializes thread registers
 * sets the newly created thread as current
 */
struct tcb*
_create_thread(struct pcb* p, ulong eip)
{
   struct tcb* t = alloc_tcb();

   TRACE1("t->tid=%d\n", t->tid);
   if(t == NULL) {
      return (NULL);
   }

   /*
    * thread properties
    */
   t->state = TS_READY;
   t->prio = DEFAULT_PRIO;
   t->cnt = t->prio;
   t->ticks = 0;
   t->pid = p->pid;
   t->pa_pd = p->pa_pd;

   /*
    * process thread links
    */
   p->last_t = t;
   t->plink = NULL;
   t->next = NULL;

   /*
    * stack
    */
   t->stk = _alloc_tstack(p);
   t->kstack = alloc_kstack(t->tid);

   /*
    * thread's machine state ( registers and stuff )
    */
   bzero(&t->f, sizeof(struct frame));
   t->f.eip = eip;
   t->f.esds = (USER_DSEG << 16) | USER_DSEG;
   t->f.ess = USER_DSEG;
   t->f.ecs = USER_CSEG;
   /*t->f.esp = t->f.ebp = (VA_STACK + ((t->stk+1)*PAGESZ) - 4);*/
   t->f.esp = t->f.ebp = t->stk;
   t->f.eflags = 0x0007202;

   /*
    * threads queue
    */
   t->next = last_tcb->next;
   last_tcb->next = t;
   last_tcb = t;

   ntcb++;

   cur_tcb = t;

   return(t);
}

struct tcb*
kill_thread(struct pcb* p, ulong tid)
{
   ulong c;
   struct tcb* t = &tcb_pool[tid-1];

   t->tid = 0;
   t->state = TS_DEAD;
   /*
    * TEMPORARILY not needed ...
    * will be used when we alloc multithreaded processes
    * free_tstack(p, t->stk);
    */

   free_kstack(tid);

   /* 
    * find thread scheduler link 
    */
   for(c = 0; c < MAX_THREADS; c++) {
      if(tcb_pool[c].next == t)
         break;
   }
   tcb_pool[c].next = t->next;
   last_tcb = t->next;
   /*t->next = 0;*/

   /* 
    * find thread process link 
    */
   for(c = 0; c < MAX_THREADS; c++) {
      if(tcb_pool[c].plink == t)
         break;
   }
   tcb_pool[c].plink = t->plink;

   ntcb--;

   return(t->plink);
}

void
t_stat()
{
   ulong c;
   for(c = 0; c < 5; c++) {
      printf("t->tid: %d, t->next->tid: %d, t->pid: %d, t->plink->tid: %d\n",
         tcb_pool[c].tid, 
         tcb_pool[c].next ? tcb_pool[c].next->tid : 0, 
         tcb_pool[c].pid,
         tcb_pool[c].plink ? tcb_pool[c].plink->tid : 0);
   }
}

/*
void
__tswitch(struct frame* f, struct tcb* t)
{
   savestate(f, &cur_tcb->f);
   cur_tcb = t;
   ldcr3(cur_tcb->pa_pd);
   resume();
}
*/

/*
 * first we setup tss stack pointer (tss.esp0) for thread kernel stack
 * next time thread enters kernel mode, it'll use this stack
 * then we resume current (cur_tcb)
 */
void
resume()
{
   ktss.esp0 = cur_tcb->kstack;
   resume_current();
}

void
tswitch(struct frame* f, struct tcb* t)
{
   fast_copy(f, &cur_tcb->f, 14);
   cur_tcb = t;
   ldcr3(cur_tcb->pa_pd);
   resume();
}

/*
 * switch to given thread
 */
void
swtch(struct tcb* t)
{
   if(cur_tcb == t)
   	return;

   cur_tcb = t;
   //if(get_cr3() != cur_tcb->pa_pd)
   ldcr3(cur_tcb->pa_pd);

   ktss.esp0 = cur_tcb->kstack;
   longjmp(cur_tcb->ctx, 1);      
}

/*
 * TINOS scheduler
 * based on Linux's
 */
void
schedule(struct frame* f)
{
   struct tcb* next; 
   struct tcb* t;
   struct tcb* first = tcb_pool;
   int c;

   c = -1;
   next = t = first;

   //cli();
   while(1) {
      if((t = t->next) == first)
         break;

      if((t->state == TS_READY) && (t->cnt > c)) {
         c = t->cnt;
         next = t;
      }
   }

   if(!c) {
      t = first;
      while((t = t->next) != first) {
         t->cnt = (t->cnt >> 1) + t->prio;
      }
   }

   if(next != first) {
      /*
       * this one is cool: since we're gonna run this guy
       * let's get him down a little
       */
      next->cnt--;
      if(next->cnt < 0) {
         printf("hmm, this guy's cnt is less than zero...\n");
      }
   }

   if(next == cur_tcb) {
      /*
       * sanity
       * This only happens, according to my experience, when init process
       * (which is also and idle process when it has done its job)
       * failes for some reason
       */
      if(cur_tcb->state != TS_READY) {
         printf("FATAL! cur_tcb->state != TS_READY\n");
         while(1);
      }

      return;
   }

   /*
    * save context of current thread
    * we return when we're resumed
    */
   if(setjmp(cur_tcb->ctx))
      return;

   /*
    * switch to next
    */
   cur_tcb = next;
   if(get_cr3() != cur_tcb->pa_pd)
      ldcr3(cur_tcb->pa_pd);
   ktss.esp0 = cur_tcb->kstack;
   longjmp(cur_tcb->ctx, 1);         
}

static uint cur_ticks = 0;

/*
 * clock interrupt
 * first some statistics, then schedule...
 */
void
clock(ulong stack)
{
   struct frame* f = (struct frame*)&stack;

   if(USERMODE(f)) {
      cur_tcb->ticks++;
      kstat.uticks++;

      schedule(f);

      /*
      if(++cur_ticks >= TIMESLICE) {
	      cur_ticks = 0;	
	      schedule(f);
      }
      */
   } else {
      kstat.kticks++;
   }
}

/*
 * this differs from swtch() coz it switches to user space
 */
void
exec_thread(struct tcb* t)
{  
   cur_tcb = t;
   if(get_cr3() != cur_tcb->pa_pd)
      ldcr3(cur_tcb->pa_pd);

   /*
    * setup esp pointer for kernel stack
    * and switch to user space
    */
   ktss.esp0 = cur_tcb->kstack;
   resume_current();
}

void
dump_kstat()
{
   printf("user   ticks: %x\n", kstat.uticks);
   printf("kernel ticks: %x\n", kstat.kticks);
}

int
thread_prio(ulong tid, int newprio)
{
   if(tid >= MAX_THREADS && newprio > MAX_PRIO)
	return (0);

   tcb_pool[tid-1].prio = newprio;
   return (1);
}

void
killme(struct frame* f)
{
	if(USERMODE(f)) {
		kill_proc(cur_tcb->pid, 0);
		schedule(f);
	}
}
