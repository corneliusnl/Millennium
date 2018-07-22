#ifndef _SYSCALL_H
#define _SYSCALL_H

/*
 * system calls
 * invoked via int system_call_vector
 */

#define SYSCALL_KERNSTAT	(0x69)
#define	SYSCALL_GETCH		(0x70)
#define	SYSCALL_EXIT		(0x71)
#define	SYSCALL_OPEN		(0x72)
#define	SYSCALL_READ		(0x73)
#define	SYSCALL_WRITE		(0x74)
#define	SYSCALL_SEEK		(0x75)
#define	SYSCALL_CLOSE		(0x76)
#define	SYSCALL_CHDIR		(0x77)
#define	SYSCALL_GETCWD		(0x78)
#define	SYSCALL_EXEC		(0x79)
#define SYSCALL_PCTL		(0x80)
#define SYSCALL_THRCTL		(0x81)

#define	SYSCALL_ALLOCMSG	(0x90)
#define	SYSCALL_CONNECTMSG	(0x91)
#define	SYSCALL_SENDMSG		(0x92)
#define SYSCALL_REPLY		(0x93)
#define SYSCALL_WAIT		(0x94)
#define SYSCALL_WAITPID		(0x95)

#define SYSCALL_MMAP		(0xA0)
#define SYSCALL_MUNMAP		(0xA1)
#define SYSCALL_SLEEP		(0xA2)
#define SYSCALL_WAKEUP		(0xA3)
#define SYSCALL_EXEC2		(0xA4)

#define SYSCALL(name)		(_syscall_##name)

#endif /* _SYSCALL_H */
