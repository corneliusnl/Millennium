/*
 * queued io
 * provides higher-level interface to disk driver
 *
 * $Id: QIO.C 1.4 1996/12/15 08:36:00 bart Exp $
 *
 */

#include <types.h>
#include <sys/thread.h>
#include <i386/inlines.h>
#include <assert.h>

//#define _DEBUG
#include <trace.h>
#undef _DEBUG

#define IO_READ		(0x01)
#define IO_WRITE	(0x02)

struct waiter {
   uint		  op;
   struct tcb*    t;
   void*          buf;
   uint           sec;
   uint           nsec;
   struct waiter* next;
   struct waiter* next_free;
};

#define NWAITERS  (10)
static struct waiter wpool[NWAITERS];

static struct waiter* first;
static struct waiter* last;

static struct waiter* free;

static struct waiter* cur_req = NULL;

extern struct tcb* cur_tcb;

void
qio_init()
{
   uint c;
   /*
    * put all into free queue
    */
   for(c = 0; c < NWAITERS; c++) {
     wpool[c].t = NULL;
     wpool[c].buf = NULL;
     wpool[c].sec = 0;
     wpool[c].nsec = 0;
     wpool[c].next_free = &wpool[c+1];
     wpool[c].next = NULL;
   }
   wpool[c-1].next_free = NULL;

   free = wpool;
   /*
    * init queue pointers
    */
   first = last = NULL;
}

static struct waiter*
q_getfree()
{
   struct waiter* w = free;

   if(free == NULL)
      return (NULL);

   free = free->next_free;
   w->next_free = NULL;

   return (w);
}

static void
q_putfree(struct waiter* w)
{
   w->next_free = free;
   free = w;
   if(first == w)
	first = NULL;
   if(last == w)
	last = NULL;
}

static struct waiter*
enq(struct tcb* t, uint op, void* buf, uint sec, uint nsec)
{
   struct waiter* w = q_getfree();

   ASSERT(w);
   bzero(w, sizeof(struct waiter));

   if(last)
      last->next = w;

   w->next = NULL;

   w->op = op;
   w->t = t;
   w->buf = buf;
   w->sec = sec;
   w->nsec = nsec;

   last = w;

   if(first == NULL)
      first = w;

   return (w);
}

static void
deq(struct waiter* w)
{
   first = w->next;
   q_putfree(w);
}

/*
void
gothru()
{
   struct waiter* p;
   for(p = first; p != NULL; p = p->next) {
      printf("p->id: %d; p->data: %d\n", p->id, p->data);
   }
}

void
qdump()
{
   uint c;
   for(c = 0; c < NWAITERS; c++) {
      printf("&w: %d; w.id: %d; w.data: %d; w.next: %d; w.next_free: %d\n",
              (ulong)&wpool[c], wpool[c].id, wpool[c].data, (ulong)wpool[c].next, (ulong)wpool[c].next_free);
   }
   printf("free: %d\n", (ulong)free);
}
*/

void
io_start(uint op, void* buf, uint sec, uint nsec)
{
   struct waiter* req;

   cli();

   cur_tcb->state = TS_WAITING;

   TRACE1("io_start(): enqueuing request tid: %d...\n", cur_tcb->tid);
   req = enq(cur_tcb, op, buf, sec, nsec);

   if( !wd_is_busy() ) {
      TRACE1("io_start(): launching IO for tid: %d\n", cur_tcb->tid);	
      cur_req = req;
      TRACE3("buf: %x, sec: %d, nsec: %d\n", (ulong)buf, sec, nsec);
      op == IO_READ ? wd_io(buf, sec, nsec) : wd_iow(buf, sec, nsec);
   } 
#ifdef _DEBUG
   else
      TRACE("io_start(): disk is busy, next time...\n");
#endif

   TRACE("io_start(): going to sleep...\n");
   schedule(NULL);
   TRACE("io_start(): resumed...\n");
}

/*
 * called from disk interrupt
 * interrupts are disabled here
 * XXX maybe we can enable them somewhere?
 */
void
io_done()
{
   struct tcb* t;

   if(cur_req == NULL)
      return;

   t = cur_req->t;

   TRACE2("io_done(): for tid %d, (interrupted tid:%d)\n", t->tid, cur_tcb->tid);
   deq(cur_req);
   cur_req = NULL;
   
   if(first) {
      TRACE1("io_done(): there's something in the queue, tid:%d\n", first->t->tid);
      cur_req = first;
      cur_req->op == IO_READ ? 
	wd_io(cur_req->buf, cur_req->sec, cur_req->nsec) :
	wd_iow(cur_req->buf, cur_req->sec, cur_req->nsec);
   } 
   else
      TRACE("io_done(): ok, queue's empty...\n");

   t->state = TS_READY;
   return;

/*
 * -------------------------------
 */
   if(cur_tcb == t) {
	TRACE("io_done(): cur_tcb == t\n");
	goto resume;
   }
   if(setjmp(cur_tcb->ctx)) {
	TRACE1("io_done(): tid %d resumed ...\n", cur_tcb->tid);
   	return;
   }

   TRACE1("io_done(): tid %d context saved, switching to next...\n", cur_tcb->tid);

resume:
   swtch(t);
   
   /*
    * it is not obvious to me, that we should swtch() here
    * that may cause hell of a problems.
    *
    * BUT, if we don't swtch() here, the IO response is lousy.
    * SO, we have to swtch() here, but first we need to protect some
    * code in various places (enqueue, dequeue) with cli(), sti() pairs
    * coz otherwise everything can be screwed up pretty easily.
    *
    */
}
