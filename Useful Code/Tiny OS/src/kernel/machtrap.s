/*
 * scstubs.S
 * system call stubs
 *
 * $Id: MACHTRAP.S 1.3 1996/12/15 08:40:01 bart Exp $
 *
 */

#include <sys/kasmdef.h>

#define SYSCALL_TRAP(name)       \
   .globl __syscall_##name     ; \
   .align   4                  ; \
__syscall_##name:              ; \
   ENTER_KERNEL                ; \
   KERNEL_SEGS                 ; \
   call  _syscall_##name       ; \
   LEAVE_KERNEL                ; \
   iret

#define MACH_TRAP(name)          \
   .globl __trap_##name        ; \
   .align   4                  ; \
__trap_##name:                 ; \
   ENTER_KERNEL                ; \
   KERNEL_SEGS                 ; \
   call  _trap_##name          ; \
   LEAVE_KERNEL                ; \
   iret

MACH_TRAP(pagefault)

SYSCALL_TRAP(kernstat)
SYSCALL_TRAP(getch)
SYSCALL_TRAP(exit)
SYSCALL_TRAP(open)
SYSCALL_TRAP(read)
SYSCALL_TRAP(write)
SYSCALL_TRAP(seek)
SYSCALL_TRAP(close)
SYSCALL_TRAP(chdir)
SYSCALL_TRAP(getcwd)
SYSCALL_TRAP(exec)
SYSCALL_TRAP(pctl)
SYSCALL_TRAP(thrctl)

SYSCALL_TRAP(allocmsg)
SYSCALL_TRAP(connectmsg)
SYSCALL_TRAP(sendmsg)
SYSCALL_TRAP(waitpid)
SYSCALL_TRAP(reply)
SYSCALL_TRAP(wait)

SYSCALL_TRAP(mmap)
SYSCALL_TRAP(munmap)

SYSCALL_TRAP(sleep)
SYSCALL_TRAP(wakeup)

SYSCALL_TRAP(exec2)

/*
 * test syscall
 */
/*
   .globl   __system_call
   .align   4
__system_call:
   ENTER_KERNEL
   KERNEL_SEGS
   
   incw  (0xb8030)

   call  _system_call

   LEAVE_KERNEL
   lret
*/
