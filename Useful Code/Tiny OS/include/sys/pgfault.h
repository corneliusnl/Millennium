#ifndef _PGFAULT_H_
#define _PGFAULT_H_

/*
 * page fault recovery
 */

#define 	PFL_MSGCLNT	(0x02)
#define 	PFL_MSGSRV	(0x04)

extern ulong pf_lock;

#define		PF_LOCK(x) 	pf_lock = (x)
#define 	PF_UNLOCK	pf_lock = 0	

#endif /* _PGFAULT_H_ */


