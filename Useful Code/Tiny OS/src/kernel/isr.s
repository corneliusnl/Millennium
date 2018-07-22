/*
 * interrupt service routines
 *
 * $Id: ISR.S 1.1 1996/11/26 15:44:57 bart Exp $
 *
 */

#include <sys/kasmdef.h>

/*
 * spur_isr  - spurious interrupt
 */
   .globl   _spur_isr
   .align   4
_spur_isr:
   ENTER_KERNEL
   KERNEL_SEGS
   movl  $0x07530753, %eax
   movl  %eax, (0xb8000)
   LEAVE_KERNEL
   iret

/*
 * keyboard interrupt
 * call kb_qkey()
 */
   .globl   _kb_isr
   .align   4
_kb_isr:
   ENTER_KERNEL
   KERNEL_SEGS

   /*
   xorl  %eax, %eax
   inb   $0x60, %al

   pushl %eax
   */

   movl  $0x20, %eax
   outb  %al, $0x20

   call  _kb_qkey

   /*
   addl  $4, %esp
   incw  (0xb8008)
   */

   LEAVE_KERNEL
   iret

/*
 * clock interrupt
 * call clock()
 */
   .globl   _clock_isr
   .align   4
_clock_isr:
   ENTER_KERNEL
   KERNEL_SEGS

   movl  $0x20, %eax
   outb  %al, $0x20

#ifdef _DEBUG     
   incw  (0xb800C)
#endif   

   call  _clock
   
   LEAVE_KERNEL
   iret

/*
 * wd hard disk interrupt
 * call wd_isr()
 */
   .globl   __wd_isr
   .align   4
__wd_isr:
   ENTER_KERNEL
   KERNEL_SEGS
   
   call  _wd_isr

   movl  $0x20, %eax
   outb  %al, $0x20
   outb  %al, $0xA0

   LEAVE_KERNEL
   iret

/*
 * floppy controller interrupt
 * call fd_isr()
 */
   .globl   __fd_isr
   .align   4
__fd_isr:
   ENTER_KERNEL
   KERNEL_SEGS

   movl  $0x20, %eax
   outb  %al, $0x20

   call  _fd_isr

   LEAVE_KERNEL
   iret

