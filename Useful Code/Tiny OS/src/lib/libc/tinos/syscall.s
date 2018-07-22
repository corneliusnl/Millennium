#include <sys/syscall.h>

#define USER_SYSCALL(name, vector)       \
   .globl   _##name                    ; \
   .align   4, 0x90                    ; \
_##name:                               ; \
   int   $##vector                     ; \
   ret      

USER_SYSCALL(os_kernstat, SYSCALL_KERNSTAT)
USER_SYSCALL(os_getch, SYSCALL_GETCH)
USER_SYSCALL(os_exit, SYSCALL_EXIT)
USER_SYSCALL(os_open, SYSCALL_OPEN)
USER_SYSCALL(os_read, SYSCALL_READ)
USER_SYSCALL(os_write, SYSCALL_WRITE)
USER_SYSCALL(os_seek, SYSCALL_SEEK)
USER_SYSCALL(os_close, SYSCALL_CLOSE)
USER_SYSCALL(os_chdir, SYSCALL_CHDIR)
USER_SYSCALL(os_exec, SYSCALL_EXEC)
USER_SYSCALL(os_pctl, SYSCALL_PCTL)
USER_SYSCALL(os_thrctl, SYSCALL_THRCTL)

USER_SYSCALL(getcwd, SYSCALL_GETCWD)
USER_SYSCALL(os_allocmsg, SYSCALL_ALLOCMSG)
USER_SYSCALL(os_connectmsg, SYSCALL_CONNECTMSG)
USER_SYSCALL(os_sendmsg, SYSCALL_SENDMSG)
USER_SYSCALL(os_waitpid, SYSCALL_WAITPID)
USER_SYSCALL(os_reply, SYSCALL_REPLY)
USER_SYSCALL(os_wait, SYSCALL_WAIT)

USER_SYSCALL(os_mmap, SYSCALL_MMAP)
USER_SYSCALL(os_munmap, SYSCALL_MUNMAP)

USER_SYSCALL(sleep, SYSCALL_SLEEP)
USER_SYSCALL(wakeup, SYSCALL_WAKEUP)

USER_SYSCALL(os_exec2, SYSCALL_EXEC2)

/*
 * test syscall (call gate)
 */

   .align   4, 0x90
__off:   .long 0x00000000
__sec:   .space   2

   .globl   _syscall
   .align   4, 0x90
_syscall:
   movl  $0x30, %eax
   movw  %ax, __sec
   lcall __off
   ret
