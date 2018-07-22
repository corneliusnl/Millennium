#ifndef _MSG_H_
#define _MSG_H_

#include <types.h>

#define MAX_MSG		(100)
#define MSG_BUFSZ	(512)

/*
 * protos
 */
int alloc_msg(struct tcb* t, ulong msg);
int connect_msg(ulong msg);
int send_msg(int port, ulong id, void* va, ulong count);
int wait_msg(int port, void* va, ulong max_count);

#endif /* _MSG_H_ */