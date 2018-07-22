/*
 * messaging subsystem
 *
 * $Id: MSG.C 1.1 1996/11/26 15:42:05 bart Exp $
 *
 */

#include <types.h>
#include <bcopy.h>
#include <sys/thread.h>
#include <sys/msg.h>
#include <sys/pgfault.h>
#include <i386/inlines.h>

#define _DEBUG
#include <trace.h>
#undef _DEBUG

extern struct tcb* cur_tcb;

struct msgwait {
	struct tcb*	t;
	ulong		busy;	
	ulong		msg;		/* msg */
	ulong		max_count;	/* server's max msg size */
	void*		va;		/* server's buf pointer */
};

void copy_msg(struct msgwait*, void*, ulong);

/*
 * or maybe use halloc() here
 */
/*
static struct msgwait wait_pool[MAX_MSG] = {0};
static uchar msg_buf[MSG_BUFSZ] = {0};
*/

static struct msgwait* wait_pool = NULL;
static uchar* msg_buf = NULL;

void
init_msg()
{
	wait_pool = (struct msgwait*)halloc(MAX_MSG * sizeof(struct msgwait));
	msg_buf = (uchar*)halloc(MSG_BUFSZ);
}

struct msgwait*
alloc_msgwait()
{
	struct msgwait* m = &wait_pool[0];
	ulong c = MAX_MSG;

	while(c--) {
		if(m->t == NULL) {
			return(m);
		}
		m++;
	}
	return (NULL);
}

/*
 * allocate message, return port number
 * used by server
 */
int
alloc_msg(struct tcb* t, ulong msg)
{
	/*
 	 * add thread to msg waiters
	 */
	struct msgwait* m = alloc_msgwait();

	if(m == NULL) {
		TRACE("wait_msg(): out of messages\n");
		return (-1);
	}

	bzero(m, sizeof(struct msgwait));

	m->t = t;
	m->msg = msg;

	return((int)((int)m-(int)&wait_pool[0]));
}

/*
 * connects to message, returns port number
 * used by client
 */
int
connect_msg(ulong msg)
{
	ulong c;
	ulong i;
	struct msgwait* m = &wait_pool[0];

	c = MAX_MSG;
	i = 0;
	while(c--) {
		if((m->t) && (m->msg == msg)) {
			break;
		}
		m++;
		i++;
	}
	if(i >= MAX_MSG)
		return (0);

	return(i);
}

/*
 * send message
 * used by client
 */
int                      
send_msg(int port, ulong id, void* va, ulong count)
{
	/*
 	 * for(any thread awaiting for msg)
	 * set it running
	 */
	struct msgwait* m = &wait_pool[port];

	/*
	 * some sanity checks
	 */
	if((port > MAX_MSG - 1) || (!m->t) || (m->va == NULL)) {
		TRACE("send_msg(): invalid port number\n");
		return (0);
	}
	
	/*printf("%d, %d\n", cur_tcb->state, m->t->state);*/
	if(cur_tcb->state == m->t->state) {
		printf("cur_tcb->state:%d, m->t->state:%d\n",
			cur_tcb->state, m->t->state);
		printf("that shouldn't happen...\n");
	}

	/*
	 * check if server is idle
	 * if so, copy the message and unblock server immediately
	 * else enqueue message 
	 */

	/*
	 * copy the message contents
	 */
	copy_msg(m, va, count);

	/*
	 * unblock server, give it message id
	 */
	m->t->state = TS_READY;
	m->t->f.eax = id;

	/*m->t->prio = 20;*/

	return (1);
}

void
process_msg(struct msgwait* m, void* va, ulong count)
{
}

/*
 * wait for message to come
 * returns 0 if failed
 * used by server
 */
int
wait_msg(int port, void* va, ulong max_count)
{
	struct msgwait* m = &wait_pool[port];

	if(port > MAX_MSG - 1) {
		TRACE("wait_msg(): invalid port number\n");
		return (0);
	}

	if(max_count > MSG_BUFSZ) {
		TRACE("wait_msg(): too big buffer size specified\n");
		return (0);
	}

	/*
	 * setup receiving buffer params
	 */
	m->va = va;
	m->max_count = max_count;
	
	/*
	 * check message queue
	 * if there's msg for this server (m->t) then process it
	 */

	/*
	 * block server and mark it not busy
	 */
	m->t->state = TS_WAITING;
	m->t->cnt++;
	m->busy = 0;

	return (1);
}	

/*
 * copy message
 * NOTE: we're in client's address space
 */
void
copy_msg(struct msgwait* m, void* va, ulong count)
{
	/*
	 * 1) copy data to kernel
	 * 2) enter waiter addr space
	 * 3) give him data
	 * 4) restore addr space
	 */
	struct tcb* tsave = cur_tcb;
	int n = count > m->max_count ? m->max_count : count; 
	
	if(count == 0 || va == NULL)
		return;

	/*
	 * lock page fault
	 */
	PF_LOCK(PFL_MSGCLNT);
	/*
	 * 1)
	 */
	bcopy(va, msg_buf, n);

	/*
	 * 2)
	 */
	ldcr3(m->t->pa_pd);
	cur_tcb = m->t;		/* tricky */
	/*
	 * 3)
	 */
	
	PF_LOCK(PFL_MSGSRV);
	bcopy(msg_buf, m->va, n);

	/*
	 * high-priority messages may cause switch directy to waiter...
	 * XXX but what about multiple waiters?
	 */ 
	
	cur_tcb = tsave;
	ldcr3(cur_tcb->pa_pd);
	PF_UNLOCK;
}
