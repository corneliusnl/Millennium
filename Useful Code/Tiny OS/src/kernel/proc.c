/*
 * process handling stuff
 *
 * $Id: PROC.C 1.3 1996/12/15 08:35:36 bart Exp $
 *
 */

#include <types.h>
#include <bcopy.h>
#include <coff.h>
#include <sys/wd.h>
#include <sys/mem.h>
#include <sys/thread.h>
#include <sys/proc.h>
#include <sys/vmm.h>
#include <sys/argframe.h>
#include <i386/inlines.h>
#include <sys/dosfs.h>
#include <assert.h>

#define MUL_NPTE(x)  ((x) << 10)

//#define _DEBUG
#include <trace.h>
#undef _DEBUG

#define open   new_open
#define read   new_read
#define seek   new_seek
#define close  new_close

/*
 * kernel working dir
 */
extern struct dir_info cur_dir;
extern struct fcb kernwd;

//struct tcb* create_thread(struct pcb*, ulong);
struct tcb* _create_thread(struct pcb*, ulong);
struct tcb* kill_thread(struct pcb*, ulong);

ulong _alloc_tstack(struct pcb*);

static ulong kernel_pt;
static ulong video_pt;

static struct pcb pcb_pool[MAX_PROC]; // = {0};
static ulong nproc = 0;

static FILHDR hdr;
static AOUTHDR aout;
static SCNHDR tscn;
static SCNHDR dscn;
static SCNHDR bscn;

static uchar sector[512];
static int hp;

static uchar* penv_buf = NULL;

/*
 * protos
 */
void dump_pd(struct pcb*);
void dump_pt(ulong*);
ulong va2phys(ulong);

/*
 * thread waiting for process to terminate
 */
struct waiter {
   ulong    pid;
   struct tcb*    t; 
};

struct waiter wait_pool[MAX_PROC] = {0};

/*
 * get free waiter slot
 */
static struct waiter*
alloc_waiter()
{
   struct waiter* w = wait_pool;
   ulong c;

   c = MAX_PROC;
   while(c--) {
      if(w->pid == 0) {
         return(w);
      }
      w++;
   }
   return(NULL);
}

/*
 * set thread waiting for a process to terminate
 */
void
set_wait(struct tcb* t, ulong pid)
{
   struct waiter* w = alloc_waiter();

   if(w == NULL)
      return;

   /*
    * printf("set_wait(): tid = %d, pid = %d\n", t->tid, pid);
    */

   /*
    * store the thread and to wait for
    */
   w->pid = pid;
   w->t = t;

   /*
    * block that thread
    */
   t->state = TS_WAITING;
}

/*
 * unblock waiting thread
 * pass it exit code
 */
static void
release_wait(ulong pid, ulong ecode)
{
   struct waiter* w = wait_pool;
   ulong c = MAX_PROC;

   /*
    * printf("trying to release wait for pid %d\n", pid);
    */

   while(c--) {
      if(w->pid == pid) {
         /*
          * printf("release_wait(): tid = %d\n", w->t->tid);
          */
      
         w->t->state = TS_READY;
         w->t->f.eax = ecode;
         w->pid = 0;

         /*       
         if(idle)
            stop_idle(w->t);        
         */

         return;
      }
      w++;
   }
}

static void
__mem_free(ulong addr)
{
   if(addr & PTE_SYSBITS)
      printf("WARNING! trying to free wierd address...");

   TRACE1("mem_free(%x)\n", addr);
   mem_free(addr);
}

/*
 * find an empty pcb slot, set its pid, return pointer to it
 * pids are just indexes in pcb table + 1
 */
static struct pcb*
alloc_pcb()
{
   ulong c, i;
   struct pcb* p = pcb_pool;

   c = MAX_PROC;
   i = 1;
   while(c--) {
      if(p->pid == 0) {
         p->pid = i;
         return (p);
      }
      p++;
      i++;
   }

   return (NULL);
}

/*
 * initializes page dir and vas pages
 */
void
create_vas(ulong* ppd, ulong* pvas, ulong* pstk,
           ulong pa_pd, ulong pa_vas, ulong pa_stk)
{
   uint c;

   /*
    * first, invalidate 'em all
    */
   for(c = 0; c < NPTE; c++) {
      ppd[c] = FREE_PTE;
      pvas[c] = FREE_PTE;
      pstk[c] = FREE_PTE;
   }

   /*
    * init vas page table
    * replica of page dir
    */
   pvas[0] = kernel_pt | PA_USER;
   pvas[PD_VAS] = pa_vas | PA_KERN;
   pvas[PD_STACK] = pa_stk | PA_KERN;

   /*
    * setup page dir virtual addr
    */
   pstk[NPTE-2] = pa_pd | PA_KERN;
   pstk[NPTE-1] = mem_alloc() | PA_USER;

   /*
    * setup page dir
    */
   ppd[0] = kernel_pt | PA_USER;
   ppd[PD_VAS] = pa_vas | PA_KERN;
   ppd[PD_STACK] = pa_stk | PA_USER;
}

/*
 * allocate and initialize vas while in kernel space
 * NOTE: in kernel space we use physical addresses
 */
static ulong
kern_init_vas()
{
   uint c;
   ulong pa_pd, pa_vas, pa_stk;
   ulong* ppd, *pvas, *pstk;

   pa_pd = mem_alloc();
   pa_vas = mem_alloc();
   pa_stk = mem_alloc();

   ppd = (ulong*)pa_pd;
   pvas = (ulong*)pa_vas;
   pstk = (ulong*)pa_stk;

   
   TRACE3("pa_pd = %x, pa_vas = %x, pa_stk = %x\n", 
           pa_pd, pa_vas, pa_stk); 

   create_vas(ppd, pvas, pstk, pa_pd, pa_vas, pa_stk);

   return(pa_pd);
}

/*
 * initialize VAS for a process
 */
static ulong
init_vas(struct pcb* p)
{
   uint c;
   ulong pd, vas, stk;
   ulong* va_pd, *va_vas, *va_stk;
   ulong pa_pd, pa_vas, pa_stk;

   pd = vmm_alloc(p, 3, VM_ALLOC);
   vas = pd + PAGESZ;
   stk = vas + PAGESZ;

   va_pd = (ulong*)pd;
   va_vas = (ulong*)vas;
   va_stk = (ulong*)stk;

   pa_pd = va2phys(pd);
   pa_vas = va2phys(vas);
   pa_stk = va2phys(stk);

   TRACE3("pd = %x, vas = %x, stk = %x\n", pd, vas, stk);
   TRACE3("pa_pd = %x, pa_vas = %x, pa_stk = %x\n", 
           pa_pd, pa_vas, pa_stk);

   create_vas(va_pd, va_vas, va_stk, pa_pd, pa_vas, pa_stk);

   vmm_free(p, pd, 3, VM_SETFREE);

   return(pa_pd);
}


/*
 * loads process image into memory
 */
int
load_proc(struct pcb* p)
{
   uint c;
   uchar* va;
   ulong sz, npg;

   sz = (bscn.s_vaddr + bscn.s_size) - VA_CODE;
   //sz = tscn.s_size + dscn.s_size + bscn.s_size;

   npg = btorp(sz);

   TRACE1("load_proc(): process needs %d pages...\n", npg);
   
   va = (uchar*)vmm_alloc(p, npg, VM_ALLOC);
   if(va == NULL) {
      TRACE("load_proc(): va = NULL\n");
      goto failure;
   }

   if((ulong)va != VA_CODE) {
      printf("load_proc(): weird, va != VA_CODE\n");
      goto failure;
   }

   TRACE("testing newly alloc'd memory...");
   for(c = 0; c < PAGESZ*npg; c++)
      va[c] = 0x00;
   TRACE("ok\n");

   if(seek(hp, tscn.s_scnptr) == -1) {
      printf("load_proc(): seek to text failed\n");
      goto failure;
   }

   if(read(hp, va, tscn.s_size) == -1) {
      printf("load_proc(): read text failed\n");
      goto failure;
   }
#ifdef _DEBUG
   printf("text begin: %x, %x, %x, %x\n",
      va[0], va[1], va[2], va[3]);
#endif

   if(seek(hp, dscn.s_scnptr) == -1) {
      printf("load_proc(): seek to data failed\n");
      goto failure;
   }

   va = (void*)dscn.s_vaddr;
   if(read(hp, va, dscn.s_size) == -1) {
      printf("load_proc(): read data failed\n");
      goto failure;
   }
#ifdef _DEBUG
   printf("data begin: %x, %x, %x, %x\n",
      va[0], va[1], va[2], va[3]);
#endif

   close(hp);

   TRACE("load_proc(): process image has been loaded...\n");
   return(1);

failure:
   close(hp);
   return(0);
}

/*
 * creates new process ( new version )
 * p is current process or NULL if we do it from kernel space
 * on success, returns pid of newly created process and we're
 * in new vas
 */
ulong
_create_proc(struct pcb* p, char* s)
{
   ulong c;
   struct pcb* newp;

   /*
    * read process image file header
    */
   if(raout(s) == -1) { 
      TRACE("cannot open exec image\n");
      return (0);
   }

   /*
    * allocate process control block
    */
   newp = alloc_pcb();
   if(!newp) {
      return (0);
   }

   cli();
   /*
    * initialize virtual address space
    */
   if(p) {
      newp->pa_pd = init_vas(p);
      /*
       * make a copy of parent process environment page
       */
      bcopy((void*)VA_PENV, penv_buf, PAGESZ);
   } else {
      /*
       * we're doing this from kernel so this process
       * inherits some env from kernel
       */
      struct proc_env* penv = (struct proc_env*)penv_buf;

      bzero((void*)penv_buf, PAGESZ);
      bcopy((void*)&kernwd, (void*)&penv->pwd, sizeof(struct fcb));

      newp->pa_pd = kern_init_vas();
   }
   TRACE("create_process(): vas initialized...\n");

   /*
    * NOTE: 's' is invalid when we switch to new address space
    *  so we setup process name here
    */
   strncpy(newp->name, s, strlen(s)+1);

   /*
    * enter new virtual address space
    */
   ldcr3(newp->pa_pd);
   sti();
   TRACE("whoa, in new address space...\n");

   /*
    * load process image into new vas
    */
   if(!load_proc(newp))
      return(0);

   cli();

   /*
    * create main thread
    */
   newp->t = _create_thread(newp, aout.entry);

   if(!newp->t)
      return(0);

   TRACE("create_process(): main thread created...\n");

   /*
   printf("t->kstack: %x\n", newp->t->kstack);
   for(c = 0; c < 100000000; c++);
   */

   /*
    * now copy parent env to a new child
    */
   bcopy(penv_buf, (void*)VA_PENV, PAGESZ);
   ((struct proc_env*)VA_PENV)->pid = newp->pid;
   /*newp->penv = (struct proc_env*)VA_PENV;*/

   nproc++;

   return(newp->pid);
}

/*
 * creates new process
 */
ulong
_create_proc2(struct pcb* p, char* s, void* va, uint npg, ulong entry)
{
   ulong c;
   struct pcb* newp;

   /*
    * allocate process control block
    */
   newp = alloc_pcb();
   if(!newp) {
      return (0);
   }

   /*
    * initialize virtual address space
    */
   ASSERT(p);
   newp->pa_pd = init_vas(p);

   /*
    * make a copy of parent process environment page
    */
   bcopy((void*)VA_PENV, penv_buf, PAGESZ);

   /*
    * NOTE: 's' is invalid when we switch to new address space
    *  so we setup process name here
    */
   strncpy(newp->name, s, strlen(s)+1);

   /*
    * 
    * this is the best place to copy address space from parent
    * coz we're in parent's address space and then we switch to new
    * one
    */

   vmm_copy(p, newp, va, VA_CODE, npg);
   //printf("_create_proc2(): ok, space copied.\n");

   /*
    * enter new virtual address space
    */
   //ldcr3(newp->pa_pd);
   //printf("_create_proc2(): whoa, in new address space...\n");

   /*
    * create main thread
    */
   newp->t = _create_thread(newp, entry);

   if(!newp->t)
      return(0);

   //printf("_create_proc2(): main thread created...\n");

   /*
    * now copy parent env to a new child
    */
   bcopy(penv_buf, (void*)VA_PENV, PAGESZ);
   ((struct proc_env*)VA_PENV)->pid = newp->pid;

   nproc++;

   return(newp->pid);
}

/*
 * execute process
 */
void
_exec_proc(ulong pid, char* pwd)
{
   struct pcb* p = &pcb_pool[pid-1];

   /*
    * some sanity assertions
    */

   /*setpwd(p, pwd);*/
   exec_thread(p->t);
}

/*
 * 2nd version
 * used by exec call
 * XXX what a f.. mess!
 * now we should teach kernel to use this one ...
 */
void
__exec_proc(ulong pid, ulong len, char* args)
{
   ulong c;
   char* ptr;
   struct pcb* p = &pcb_pool[pid-1];
   struct arg_frame* af = (struct arg_frame*)(dscn.s_vaddr);

   af->argc = *(ulong*)args;
   bcopy(args + sizeof(ulong), af->args, len);
   ptr = af->args;
   for(c = 0; c < af->argc; c++) {
      af->argv[c] = ptr;
      ptr += (strlen(ptr)+1);
   }

   exec_thread(p->t);
}

void
exec_proc2(ulong pid)
{
   struct pcb* p = &pcb_pool[pid-1];
   exec_thread(p->t);
}

#define KPGNO  (512) /* no of kernel pages 64   */
#define VPGNO  (6)   /* no of video pages       */

#define VIDSTART  (512)
#define VIDKOFF   (VIDSTART-KPGNO) /* video offset from kernel in pages */

/*
 * initializes page table that is shared among
 * all processes: kernel and video
 * ran once at system initialization
 */
void
init_sys_pt()
{
   ulong c;
   ulong* kpt; /* kernel page table */
   ulong kbase = 0x00000000;
   ulong vbase = 0xb8000;
   ulong shmem_base = 0x90000;

   /* 
    * use kernel heap 
    */
   kernel_pt = halloc(PAGESZ);

   kpt = (ulong*)kernel_pt;
   
   c = NPTE;
   while(c--) {
      *kpt++ = INVALID_PTE;
   }

   /* 
    * map 1M of kernel 
    */
   kpt = (ulong*)kernel_pt;

   c = KPGNO;
   while(c--) {
      *kpt++ = (kbase | PA_KERN);
      kbase += PAGESZ;
   }
   
   /* 
    * map video mem 
    */
   kpt = (ulong*)kernel_pt;
   kpt += VIDSTART;
   c = VPGNO;
   while(c--) {   
      *kpt++ = (vbase | PA_USER);
      vbase += PAGESZ;
   }
}

/*
 * initialize process subsystem
 * called once at kernel initialization
 */
void
proc_init()
{
   init_sys_pt();
   penv_buf = (uchar*)halloc(PAGESZ);
}

/*
 * go thru all ptes, free pages if needed
 * does not free pt itseft
 */
void
free_pt(ulong* pt)
{
   ulong c;

   for(c = 0; c < NPTE; c++) {
      if(*pt != INVALID_PTE) {
         __mem_free(PADDR(*pt));
      }
      pt++;
   }
}

/*
 * go thru p->pa_pd, deallocate all pages
 * including ptes and pd itself
 * (!!!) this must be done while in kernel space
 */
/*
void
free_pd(struct pcb* p)
{  
   ulong c;
   ulong* pd = (ulong*)p->pa_pd;
   
   pd += 2;*/ /* skip kernel and video */
/* for(c = 2; c < NPTE; c++) { 
      if(*pd != INVALID_PTE) {*/ /* there's sth here! */
/*       ulong* pt = (ulong*)PTE(*pd);
         TRACE1("pd[%d] != INVALID_PTE\n", c);
         free_pt(pt);
         __mem_free((ulong)pt);
      }
      pd++;
   }
   __mem_free(p->pa_pd);
}
*/

/*
 * free_pd ( new version, using vas )
 * go thru p->pa_pd, deallocate all pages
 * including ptes and pd itself
 */
void
_free_pd(struct pcb* p)
{  
   ulong c;
   ulong* pd = (ulong*)VA_MAPD;
   ulong* vas = (ulong*)VA_VAS;
   ulong* pt;
   
   /*
    * skip system page table and vas
    */
   pd += 2;
   pt = vas + MUL_NPTE(2);

   TRACE2("&pd:%x, &pt:%x\n", (ulong)pd, (ulong)pt);
   for(c = 2; c < NPTE; c++) { 
      if(*pd != INVALID_PTE) { 
         /* 
          * there's sth here! 
          */
         TRACE2("PT(*pd) = %x, index: %d\n", *pd, c);    

         free_pt(pt);

         TRACE1("FREEING PT: %x\n", *pd);
         __mem_free((ulong)PADDR(*pd));
      }
      pd++;
      pt += NPTE;
   }
   /*
    * now dealloc vas page
    */
   pd = (ulong*)VA_MAPD;
   TRACE1("FREEING vas: %x\n", pd[PD_VAS]);
   __mem_free(PADDR(pd[PD_VAS]));
   
   /*
    * NOTE: we do not have to free page dir coz it's
    * mapped at virtual VA_MAPD, and is already freed
    */
   /*
   printf("FREEING pd: %x\n", p->pa_pd);
   __mem_free(p->pa_pd);
   */ 
}

/*
 * setup virtual address space for a process
 */
void
setup_vas(struct pcb* p)
{
   ulong* pd = (ulong*)p->pa_pd;
   ulong* pt = (ulong*)PADDR(pd[PD_STACK]);

   /*
    * virtual location of process page dir
    */
   pt[NPTE-2] = p->pa_pd | PA_KERN;

   /*
    * setup VAS page table
    */   
   pt = (ulong*)PADDR(pd[PD_VAS]);

   /*
    * first page - kernel stuff
    * XXX is it really needed?
    *     we could rather use it for page dir
    */
   pt[0] = kernel_pt | PA_KERN;

   /*
    * map itself as second page
    * this way we can fiddle in vas page table
    * while in process's virtual space
    * (memory allocs use it)
    */
   pt[PD_VAS] = ((ulong)pt) | PA_KERN;
}

/*
 * this one is called by create_thread routine
 * it allocates a page for a thread stack and attaches it to
 * process's stack page table
 * return va of the stack
 */
/*
ulong
alloc_tstack(struct pcb* p)
{
   ulong c;
   ulong* pd = (ulong*)p->pa_pd;
   ulong* pt = (ulong*)PTE(pd[PD_STACK]);
*/ /*ulong tid = p->last_t->tid;*/
/*
   for(c = 0; c < NPTE; c++) {
      if(*pt == INVALID_PTE) {
         TRACE1("alloc_tstack: free pte at %d\n", c);
         *pt = mem_alloc() | PA_USER;
*/       /*return(VA_STACK+((c+1)*PAGESZ)-4);*/
/*       return(c);
      }
      pt++;
   }

   printf("oops, out of thread stack space!\n");
   while(1);
}  
*/

/*
 * this one is called by create_thread routine
 * it allocates a page for a thread stack and attaches it to
 * process's stack page table
 * return va of the stack
 */
ulong
_alloc_tstack(struct pcb* p)
{
   ulong c;
   ulong* pd = (ulong*)VA_MAPD;
   ulong* vas = (ulong*)VA_VAS;
   ulong* pt = vas + MUL_NPTE(PD_STACK);

   TRACE1("_alloc_tstack(): &pt = %x\n", (ulong)pt);

   for(c = 0; c < NPTE; c++) {
      if(*pt == INVALID_PTE) {
         uint i = NSTKPGS;
         TRACE1("_alloc_tstack(): free pte at %d\n", c);

         /*
          * check if there's space for stack pages
          */
         if(c + i >= NPTE)
            break;

         //*pt = mem_alloc() | PA_USER;
         while(i--) {
            *pt++ = mem_alloc() | PA_USER;
         }

         return(VA_STACK+((c+1)*PAGESZ)-4);
         /*return(c);*/
      }
      pt++;
   }

   printf("oops, out of thread stack space!\n");
   while(1);
}

/*
 * alloc kernel stack for thread
 * XXX for now, we just rip a page from upper 4 megs of system page table
 * should be changed
 */
ulong
alloc_kstack(ulong tid)
{
   ulong* kpt = (ulong*)kernel_pt;

   kpt[768+tid-1] = pmm_alloc() | PA_KERN;

   /*
   kspace();
   printf("zeroing page...kpt[768+tid]:%x\n", kpt[768+tid]);
   bzero((void*)0x301000, PAGESZ);
   printf("ok\n");
   */

   TRACE2("alloc_kstack: tid:%d, kstack:%x\n", 
           tid, 0x300000 + (tid*PAGESZ)-4 );   

   return(0x300000+(tid*PAGESZ)-4);
}

void
free_kstack(ulong tid)
{
   ulong* kpt = (ulong*)kernel_pt;
   uint i = 768+tid-1;

   TRACE1("kstack_free(): %x\n", PTE(kpt[i]));

   __mem_free(PTE(kpt[i]));
   kpt[i] = INVALID_PTE;
}
   
/*
 * free thread stack
 * called when thread terminates
 */
void
free_tstack(struct pcb* p, ulong stk)
{
   ulong* pd = (ulong*)p->pa_pd;
   ulong* pt = (ulong*)PTE(pd[PD_STACK]);
   
   __mem_free(PTE(pt[stk]));
   pt[stk] = INVALID_PTE;
}

/*
 * given virtual address 
 * return physical
 * XXX what is it doing here? put it in vmm.c
 */
ulong
va2phys(ulong va)
{
   ulong ipd, ipt;
   ulong* pd = (ulong*)VA_MAPD;
   ulong* pt;
   ulong* vas = (ulong*)VA_VAS;
   ulong pa;

   ipd = IPD(va);
   ipt = IPT(va);

   TRACE3("va = %x, ipd = %d, ipt = %d\n", va, ipd, ipt);

   pa = PADDR(*(vas + (ipd << 10) + ipt));
   TRACE1("pa = %x\n", pa);

   return(pa);
}

/*
 * kill process, kill all process threads, release waits
 * free page dir (all pages)
 */
void
kill_proc(ulong pid, ulong ecode)
{
   struct pcb* p = &pcb_pool[pid-1];

   if(p->pid == 0)
      return;

   TRACE1("killing thread, tid %d\n", p->t->tid);

   /* 
    * kill 1st thread 
    * XXX kill all process threads
    */
   p->last_t = kill_thread(p, p->t->tid);

   TRACE1("deallocating process's page dir, pa_pd %d\n", p->pa_pd);

   /*
    * ensure we're in proper address space
    * free_pd() requires this
    */
   if(get_cr3() != p->pa_pd)
	ldcr3(p->pa_pd);

   _free_pd(p);

   release_wait(p->pid, ecode);

   bzero(p, sizeof(struct pcb));
   nproc--;

   TRACE1("nfree: %d\n", get_nfree());
}

void
create_thread_api(ulong pid)
{
   struct pcb* p = &pcb_pool[pid-1];
   struct tcb* last_t = p->last_t;
   struct tcb* t = _create_thread(p, 0);

   last_t->plink = t;
   /*p->last_t = t;*/

   /*
    * exec thread here
    */
}

void
kill_thread_api(ulong pid, ulong tid)
{
   struct pcb* p = &pcb_pool[pid-1];

   p->last_t = kill_thread(p, tid);

   /* 
    * switch to another here
    */
}

/*
 * reads aout(actually coff) exec image
 */
int
raout(char* s)
{
   uint c, cnpg, dnpg, dsz;

   hp = open(s);

   if(hp == -1) {
      TRACE("open failed\n");
      return(-1);
   }
   
   /*
    * read sections
    * ugly goto's for cleaner, faster code
    */

   if(read(hp, (void*)&hdr, sizeof(hdr)) < 0) goto read_failed;

   if(I386BADMAG(hdr)) {
      printf("not an executable format\n");
      return(-1);
   }

   if(read(hp, (void*)&aout, sizeof(aout)) < 0) goto read_failed;
   if(read(hp, (void*)&tscn, sizeof(tscn)) < 0) goto read_failed;
   if(read(hp, (void*)&dscn, sizeof(dscn)) < 0) goto read_failed;
   if(read(hp, (void*)&bscn, sizeof(bscn)) < 0) goto read_failed;

   if(aout.entry != VA_CODE) {
      printf("raout(): weird entry point\n");
      return(-1);
   }

   TRACE2("hdr.magic %x, hdr.nscns %x\n", hdr.f_magic, hdr.f_nscns);
   TRACE4("aout.tsize %x, aout.dsize %x, aout.bsize %x, aout.entry 0x%x\n", 
           aout.tsize, aout.dsize, aout.bsize, aout.entry);
   TRACE4("t.name: %s, t.vaddr: %x, t.size: %x, t.ptr: %x\n", 
           tscn.s_name, tscn.s_vaddr, tscn.s_size, tscn.s_scnptr);
   TRACE4("d.name: %s, d.vaddr: %x, d.size: %x, d.ptr: %x\n", 
           dscn.s_name, dscn.s_vaddr, dscn.s_size, dscn.s_scnptr);
   TRACE4("b.name: %s, b.vaddr: %x, b.size: %x, b.ptr: %x\n", 
           bscn.s_name, bscn.s_vaddr, bscn.s_size, bscn.s_scnptr);

   /*close(hp);*/

   return (1);

read_failed:

   printf("raout(): read() failed.\n");
   return (-1);
}

/*
 * temporary
 */
void
p_stat()
{
   ulong c;
   struct pcb* p = pcb_pool;
   
   printf("pid name                 pd       kstk     tid prio cnt state next tick\n");
   c = MAX_PROC;
   while(c--) {
      char buf[200];
      if(p->pid > 0) {
         char ch;
         switch(p->t->state) {
            case TS_READY: 
               ch = 'R';
               break;
            case TS_WAITING:
               ch = 'W';
               break;
            default:
               ch = '-';
               break;
         }
         sprintf(buf, "%-3d %-20s %-8x %-8x %-3d %-4d %-3d %-5c %-4d %-8d\n",
            p->pid, p->name, p->pa_pd, p->t->kstack, p->t->tid, 
            p->t->prio, p->t->cnt, ch, p->t->next->tid, 
            p->t->ticks);
         printf(buf);
      }
      p++;
   }

   dump_kstat();
   printf("pheap: %x\n", get_heap_ptr());
   show_time();
   show_date();
}

struct pcb*
get_pcb(struct tcb* t)
{
   return(&pcb_pool[t->pid-1]);
}

struct pcb*
get_proc(ulong pid)
{
   return(&pcb_pool[pid-1]);
}

ulong
is_alive(ulong pid)
{
   /*
    * return(pcb_pool[pid-1] ? 1 : 0);
    */
   return(pcb_pool[pid-1].pid);
}

int
proc_prio(ulong pid, uint newprio)
{
   if(pid >= MAX_PROC)
	return (0);

   return (thread_prio(pcb_pool[pid-1].t->tid, newprio));
}

void
dump_pt(ulong* pt)
{
   int c;

   for(c = 0; c < 10; c++)
      printf("%x,", pt[c]);
   printf("\n");
}
