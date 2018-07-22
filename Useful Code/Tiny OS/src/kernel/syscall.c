/*
 * system calls
 *
 * $Id: SYSCALL.C 1.5 1996/12/15 08:36:08 bart Exp $
 *
 */

#include <types.h>
#include <assert.h>
#include <bcopy.h>
#include <sys/desc.h>
#include <sys/thread.h>
#include <sys/arg.h>
#include <sys/argframe.h>
#include <sys/proc.h>
#include <sys/mem.h>
#include <sys/vmm.h>
#include <sys/msg.h>
#include <sys/tss.h>
#include <sys/kernel.h>
#include <i386/inlines.h>
#include <i386/setjmp.h>

//#define _DEBUG
#include <trace.h>
#undef _DEBUG

static struct tcb* sender = NULL;

extern struct tss ktss;

extern struct tcb* cur_tcb;
extern uint idle;

extern struct pcb* get_pcb(struct tcb*);
extern struct pcb* get_proc(ulong);

static char* args_buf = 0;
static char* name_buf = 0;

void
init_syscall()
{
   name_buf = (char*)halloc(MAX_NAME);
   args_buf = (char*)halloc(MAX_ARGSZ);
}

void
syscall_getch(ulong stack)
{
   struct frame* f = (struct frame*)&stack;

   /* 
    * if key is already in, return it 
    */
   f->eax = kb_getch();
   if(f->eax)
      return;
   
   /* 
    * block that thread 
    */
   cur_tcb->state = TS_WAITING;
   kb_waiter(cur_tcb);

   /*
    * sleep and run another one
    */
   schedule(f);

   /*
    * ok, there's a key, get it!
    */
   f->eax = kb_getch();
}

void
syscall_exit(ulong stack)
{
   struct frame* f = (struct frame*)&stack;
   ulong* pstk = (ulong*)f->esp;
   ulong ecode = (ulong)*++pstk;

   TRACE1("killing process, pid %d\n", cur_tcb->pid);

   kill_proc(cur_tcb->pid, ecode);

   TRACE("now switching to next...\n");

   schedule(f);
}

void
syscall_open(ulong stack)
{
   struct frame* f = (struct frame*)&stack;
   ulong* pstk = (ulong*)f->esp;
   char* file = (char*)*++pstk;
   uint flags = (uint)*++pstk; 
   uint mode = (uint)*++pstk; 
   uint c;
   int h;

   h = fsm_open(get_pcb(cur_tcb), file, flags, mode);

#ifdef _DEBUG
   if(h == -1) {
      printf("open failed\n");
   } else {
      printf("open ok\n");
   }
#endif

   f->eax = h;
}

void
syscall_read(ulong stack)
{
   struct frame* f = (struct frame*)&stack;
   ulong* pstk = (ulong*)f->esp;
   int h = (int)*++pstk;
   void* buf = (void*)*++pstk;
   uint count = (uint)*++pstk;

   if(h < 4)
   f->eax = -1;
   else
      f->eax = fsm_read(h, buf, count);
}

void
syscall_write(ulong stack)
{
   struct frame* f = (struct frame*)&stack;
   ulong* pstk = (ulong*)f->esp;
   int h = (int)*++pstk;
   void* buf = (void*)*++pstk;
   uint count = (uint)*++pstk;

   
   if(h < 4) 
   f->eax = -1;
   else
      f->eax = fsm_write(h, buf, count); 
}

void
syscall_seek(ulong stack)
{
   struct frame* f = (struct frame*)&stack;
   ulong* pstk = (ulong*)f->esp;
   int h = (int)*++pstk;
   uint pos = (uint)*++pstk;

   f->eax = fsm_seek(h, pos);
}

void
syscall_close(ulong stack)
{
   struct frame* f = (struct frame*)&stack;
   ulong* pstk = (ulong*)f->esp;
   int h = (int)*++pstk;

   f->eax = fsm_close(h);
}

void
syscall_chdir(ulong stack)
{
   struct frame* f = (struct frame*)&stack;
   ulong* pstk = (ulong*)f->esp;
   char* dir_name = (char*)*++pstk;
   
   f->eax = fsm_setpwd(get_pcb(cur_tcb), dir_name);
}

void
syscall_getcwd(ulong stack)
{
   struct frame* f = (struct frame*)&stack;
   ulong* pstk = (ulong*)f->esp;
   char* buf = (char*)*++pstk;
   
   //getpwd(get_pcb(cur_tcb), buf);
}

void
syscall_exec(ulong stack)
{
   struct frame* f = (struct frame*)&stack;
   ulong* pstk = (ulong*)f->esp;
   char* name;
   ulong pid, c, len;
   ulong argc;
   char* pargs, *args, *p;
   struct tcb* callee = cur_tcb;
   struct pcb* proc = get_pcb(callee);
   struct proc_env* penv = (struct proc_env*)VA_PENV;

   /*
    * get name of a process, store it in our space
    */
   name = (char*)*++pstk;
   if(name == NULL) {
      f->eax = -1;
      return;
   }
   strcpy(name_buf, name);

   /*
    * get args
    */
   pargs = args = (char*)*++pstk;
   if(pargs == NULL) {
      f->eax = -1;
      return;
   }

   argc = *(ulong*)args;
   args += sizeof(ulong);

   TRACE1("argc = %d\n", argc);
   TRACE1("args[0] = %s\n", args);

   p = args;
   len = sizeof(ulong);
   for(c = 0; c < argc; c++) {
      uint l = strlen(p) + 1;
      len += l;
      p += l;
   }

   bcopy(pargs, args_buf, len); 

   /*
    * set kernel working dir to process's
    */
   setkwd(proc);
   f->eax = _create_proc(proc, name_buf);

   TRACE1("create_proc returned: %d\n", f->eax); 

   if(f->eax == 0) {
      cur_tcb = callee;
      ldcr3(cur_tcb->pa_pd);
      return;
   }

   /*
    * save state of callee thread and launch a new process
    */
   if(setjmp(callee->ctx))
      return;

   __exec_proc(f->eax, len, args_buf);   
}

void
syscall_exec2(ulong stack)
{
   struct frame* f = (struct frame*)&stack;
   struct pcb* proc;
   struct tcb* callee;
   ulong pid, len;
   ulong* pstk = (ulong*)f->esp;
   char* name = (char*)*++pstk;
   void* va = (void*)*++pstk;
   uint npg = (uint)*++pstk;
   ulong entry = (ulong)*++pstk;

   callee = cur_tcb;

   //printf("exec2(): va: %x, npg: %d, entry: %x\n", (ulong)va, npg, entry);

   cli();
 
   proc = get_pcb(cur_tcb);
   f->eax = _create_proc2(proc, name, va, npg, entry);
   //printf("exec2(): ok, _create_proc2() returned %d\n", f->eax);   

   if(!f->eax) {
       ldcr3(proc->pa_pd);
       return;
   }

   //printf("exec2(): saving context...\n");   
   if(setjmp(callee->ctx))
      return;

   exec_proc2(f->eax);   
}

void
syscall_allocmsg(ulong stack)
{
   struct frame* f = (struct frame*)&stack;
   ulong* pstk = (ulong*)f->esp;
   ulong msg = (ulong)*++pstk;

   TRACE2("thread %d waiting for msg %d\n", cur_tcb->tid, msg);

   f->eax = alloc_msg(cur_tcb, msg);   
}

void
syscall_connectmsg(ulong stack)
{
   struct frame* f = (struct frame*)&stack;
   ulong* pstk = (ulong*)f->esp;
   ulong msg = (ulong)*++pstk;

   f->eax = connect_msg(msg);
}

void
syscall_wait(ulong stack)
{
   struct frame* f = (struct frame*)&stack;
   ulong* pstk = (ulong*)f->esp;
   int port = (ulong)*++pstk;
   void* va = (void*)*++pstk;
   ulong max_count = (ulong)*++pstk;

   if(!wait_msg(port, va, max_count)) {
      f->eax = 0;
      return;
   }

   if(sender)
      sender->state = TS_READY;
   
   schedule(f);
}

void
syscall_sendmsg(ulong stack)
{
   struct frame* f = (struct frame*)&stack;
   ulong* pstk = (ulong*)f->esp;
   int port = (int)*++pstk;
   ulong id = (ulong)*++pstk;
   void* va = (void*)*++pstk;
   ulong count = (ulong)*++pstk;

   TRACE2("thread %d sending port %d\n", cur_tcb->tid, port);
   TRACE2("va %d, count %d\n", (ulong)va, count);

   if(!send_msg(port, id, va, count)) {
      f->eax = 0;
      return;
   } 

   cur_tcb->state = TS_WAITING;
   sender = cur_tcb;
   sender->cnt++;

   schedule(f);
}

void
syscall_waitpid(ulong stack)
{
   struct frame* f = (struct frame*)&stack;
   ulong* pstk = (ulong*)f->esp;
   ulong pid = (ulong)*++pstk;

   /*
    * it's important to check if process exists
    * coz it may happen that the process we want to wait for
    * already terminated
    */
   if(is_alive(pid)) {
      set_wait(cur_tcb, pid);
      schedule(f);
   }
   f->eax = -10;
}

void
syscall_reply(ulong stack)
{
   struct frame* f = (struct frame*)&stack;
   
   sender->state = TS_READY;
}

void
syscall_mmap(ulong stack)
{
   struct frame* f = (struct frame*)&stack;
   ulong* pstk = (ulong*)f->esp;
   ulong npg = (ulong)*++pstk;
   ulong flags = (ulong)*++pstk;

   //printf("mmap(): npg = %d\n", npg);

   f->eax = vmm_alloc(get_pcb(cur_tcb), npg, flags);
}

void
syscall_munmap(ulong stack)
{
   struct frame* f = (struct frame*)&stack;
   ulong* pstk = (ulong*)f->esp;
   ulong va = (ulong)*++pstk;
   ulong npg = (ulong)*++pstk;
   ulong flags = (ulong)*++pstk;

   f->eax = vmm_free(get_pcb(cur_tcb), va, npg, flags);
}

static void
showvas(struct pcb* p)
{
   uint c;
   ulong* pd;
   ulong* pt;

   pd = (ulong*)VA_MAPD;
   pt = (ulong*)VA_VAS;

   for(c = 0; c < 10; c++)
      printf("%x,",pd[c]);
   printf("\n");
   for(c = 0; c < 10; c++)
      printf("%x,",pt[c]);
   printf("\n");

   pt = (ulong*)(VA_VAS+(PAGESZ*PD_CODE));
   for(c = 0; c < 10; c++)
      printf("%x,",pt[c]);
   printf("\n");

   printf("now real values...\n");
   kspace();

   pd = (ulong*)p->pa_pd;
   pt = (ulong*)PTE(pd[PD_CODE]);
   dump_pt(pd);
   dump_pt(pt);
}

static jmp_buf context;
static struct frame* cur_frame;
static struct tcb* sleeper;

static void
sleep(struct frame* f)
{
   //printf("sleep(): let setjmp()...\n");

   /*
    * note: setjmp returns 0 if we've just saved the context
    * if we're resumed, it returns 1 ...
    */
   /*
   if(setjmp(context)) {
      printf("sleep(): yeah, we're resumed...\n");
      return;
   }

   printf("sleep(): ok, kernel context saved, let's schedule()...\n");
   schedule(cur_frame);
   */

   printf("sleep(): ok, let's sleep for awhile...\n");
   schedule(cur_frame);
   printf("sleep(): ok, we're awaken...\n");
}

void
syscall_sleep(ulong stack)
{
   struct frame* f = (struct frame*)&stack;
   ulong* pstk = (ulong*)f->esp;
   struct tcb* this = cur_tcb;
   ulong c;

   this->state = TS_WAITING;
   cur_frame = f;
   sleeper = cur_tcb;
   
   printf("syscall: do some processing...\n");
   for(c = 0; c < 10000000; c++);

   printf("syscall: ok, we need to sleep...\n");
   sleep(f);
   printf("syscall: wow, we're up again...\n");
   printf("syscall: some processing again...\n");
   for(c = 0; c < 10000000; c++);

   printf("syscall: now, let's sleep again...\n");
   sleep(f);
   printf("syscall: YES! up one more time...\n");

   printf("syscall: now, let's go back to suspended thread...\n");   

   return;
   //this->state = TS_READY;
   //swtch(this);

/*****************/

   if(setjmp(context)) {
      printf("YEAH! ...resumed...\n");

      cur_tcb->state = TS_READY;
      ktss.esp0 = cur_tcb->kstack;
      resume_current();
   }

   /*fast_copy(f, &cur_tcb->f, 14);*/
   cur_tcb->state = TS_WAITING;

   printf("ok, here we go after saving context...\n");
   printf("eip:%x, edi:%x, esi:%x, edx:%x, ecx:%x\n",
      context->eip, context->edi, context->esi, context->edx,
      context->ecx);

   schedule(f);
}

void
syscall_wakeup(ulong stack)
{
   struct frame* f = (struct frame*)&stack;
   ulong* pstk = (ulong*)f->esp;
   ulong c;

   printf("syscall_wakeup()....\n");
   printf("interrupt: ok, let's try to swtch()...\n");

   /*
    * save the thread we've interrupted
    */
   if(setjmp(cur_tcb->ctx))
      return;

   swtch(sleeper);
   printf("after swtch() : FATAL!\n");
   return;

   /*
    * we've interrupted some thread
    * so, save its context
    * note, we don't do copying stuff, schedule will do it
    * coz we're setting cur_frame current context on stack
    */
   /*
    * fast_copy(f, &cur_tcb->f, 14);
    */
   cur_frame = f;

   /*
   cur_tcb = p->t;
   ldcr3(cur_tcb->pa_pd);
   printf("ok, ldcr3 ok\n");
   */

   printf("interrupt: let's longjmp() ...\n");
   longjmp(context, 1);
}

static void
test_vmcopy(ulong pid, uint npg)
{
   vmm_copy(get_pcb(cur_tcb), get_proc(pid), VA_CODE, VA_CODE, npg);
   printf("test_vmcopy(): done.\n");
}

void
syscall_kernstat(ulong stack)
{
   struct frame* f = (struct frame*)&stack;
   ulong* pstk = (ulong*)f->esp;
   uint stat = (uint)*++pstk;
	
   switch(stat) {
	case KS_PROC:
		p_stat();   
		break;
	case KS_REBOOT:
		reboot();
		break;
        case KS_ASYNC:
		wd_async();
		break;
	default:
		printf("syscall_kernstat(): warning, bad value specified.\n");
   }
}

void
syscall_pctl(ulong stack)
{
   struct frame* f = (struct frame*)&stack;
   ulong* pstk = (ulong*)f->esp;
   uint cmd = (uint)*++pstk;
   uint pid = (uint)*++pstk;
   ulong val = (ulong)*++pstk;
   struct pcb* p = get_proc(pid);

   if(!is_alive(pid)) {
	printf("syscall_pctl(): wierd pid\n");
	f->eax = -1;
	return;
   }
 
   printf("syscall_pctl(): ok, proceeding...\n");

   cli();
   switch(cmd) {
	case PC_KILL:
		kill_proc(pid, val);
		break;
	case PC_SLEEP:
		p->t->state = TS_WAITING;
		break;
	case PC_WAKEUP:
		p->t->state = TS_READY;
		break;
	case PC_SETPRIO:
		proc_prio(pid, val);
		f->eax = 1;
		return;
	default:
		f->eax = -1;
		return;
   }

   f->eax = 1;
   if(p->t == cur_tcb) {
	printf("syscall_pctl(): needa switch...\n");
	schedule(f);
   }
   if(get_cr3() != cur_tcb->pa_pd)
      ldcr3(cur_tcb->pa_pd);
}

void
syscall_thrctl(ulong stack)
{
   struct frame* f = (struct frame*)&stack;
   ulong* pstk = (ulong*)f->esp;
   uint tid = (uint)*++pstk;
   uint cmd = (uint)*++pstk;
   ulong val = (ulong)*++pstk;

   switch(cmd) {
   }
}
