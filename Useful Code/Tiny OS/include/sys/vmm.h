/*
 * virtual memory manager
 */
#ifndef _VMM_H_
#define _VMM_H_

/*
 * allocations flags
 */
#define 	VM_ALLOC	(0x01)
#define 	VM_COMMIT	(0x02)
#define 	VM_RESERVE	(0x03)
#define 	VM_FREE		(0x04)
#define 	VM_SETFREE	(0x05)
#define 	VM_DECOMMIT	(0x06)

#endif /* _VMM_H_ */


