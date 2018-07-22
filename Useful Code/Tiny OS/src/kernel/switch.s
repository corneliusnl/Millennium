/*
 * switch.S       (c) 1996 Bart Sekura
 * 
 * routines used when switching context
 *
 * $Id: SWITCH.S 1.1 1996/11/26 15:45:24 bart Exp $
 *
 */

/*
 * savestate(void* src, void* dst)
 * copies context frame
 */

#define FRAMESZ	14

   .globl   _savestate
   .align   4
_savestate:
   pushl %esi
   pushl %edi
   movl  0xC(%esp), %esi
   movl  0x10(%esp), %edi
   movl  $FRAMESZ, %ecx
   rep
   movsl
   popl  %edi
   popl  %esi
   ret

/*
 * resume_current() - resumes thread pointed by cur_tcb
 */
   .globl   _resume_current
   .align   4

_resume_current:
   movl  _cur_tcb, %esp
   popw  %es
   popw  %ds
   popal
   iret

/*
 * resume_context(frame* f) - resumes thread from frame f
 */
   .globl   _resume_context
   .align   4
_resume_context:
   movl  0x04(%esp), %eax
   movl  %eax, %esp
   popw  %es
   popw  %ds
   popal
   iret
