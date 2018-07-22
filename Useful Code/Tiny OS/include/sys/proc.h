#ifndef _PROC_H_
#define _PROC_H_

/*
 * process handling structures
 */

#include <types.h>
#include <sys/thread.h>
#include <sys/mem.h>

#include <sys/dosfs.h>
#include <sys/fs.h>

#define MAX_PROC	(64)	/* max. no. of processes in the system */

/*
 * process control commands - pctl syscall
 */
#define PC_KILL		(0x02) /* kills all process threads 	*/
#define PC_SLEEP	(0x04) /* main thread sleeps		*/
#define PC_WAKEUP	(0x08) /* main thread wakes up		*/
#define PC_SETPRIO	(0x10) /* change priority of main thr.	*/

/*
 * process environment structure
 * at virtual address VA_PENV
 */
struct proc_env {
	int			oftab[MAX_FILES];
	struct fcb 		pwd;
	ulong			uid;
	ulong			gid;
	ulong			pid;
};

/*
 * process control block
 *
 */

#define 	MAX_PROCNAME	(20)

struct pcb {
	ulong	pa_pd;				/* phys addr of pagedir */
	ulong	pid;	
	ulong	last_tid;			/* last thread 		*/
	ulong	va_env;

	struct tcb* 	t;			/* first (main) thread 	*/
	struct tcb* 	last_t;			/* last thread 		*/

	char		name[MAX_PROCNAME];	/* process name 	*/
};

#endif /* _PROC_H_ */
