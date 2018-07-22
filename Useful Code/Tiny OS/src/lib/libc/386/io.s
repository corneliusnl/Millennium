/*
 * routines for handling i386 I/O operations
 * from VSTa
 * Copyright (c) 1996 Andy Valencia
 *
 * $Id$
 *
 */

/*
 * uchar inb(port)
 */
   .globl   _inb
   .align   4
_inb:
   movl  4(%esp),%edx
   xorl  %eax,%eax
   inb   %dx,%al
   ret

/*
 * outb(port, data8)
 */
   .globl   _outb
   .align   4
_outb:
   movl  4(%esp),%edx
   movl  8(%esp),%eax
   outb  %al,%dx
   ret
/*
 * ushort inw(port)
 */
   .globl   _inw
   .align   4
_inw:
   movl  4(%esp),%edx
   xorl  %eax,%eax
   inw   %dx,%ax
   ret

/*
 * outw(port, data16)
 */
   .globl   _outw
   .align   4
_outw:
   movl  4(%esp),%edx
   movl  8(%esp),%eax
   outw  %ax,%dx
   ret

/*
 * uint inl(port)
 */
   .globl   _inl
   .align   4
_inl:
   movl  4(%esp),%edx
   xorl  %eax,%eax
   inl   %dx,%eax
   ret

/*
 * outl(port, data32)
 */
   .globl   _outl
   .align   4
_outl:
   movl  4(%esp),%edx
   movl  8(%esp),%eax
   outl  %eax,%dx
   ret
/*
 * repinsw(port, buffer, count)
 */
   .globl   _repinsw
   .align   4
_repinsw:
   pushl %edi
   movl  0x8(%esp),%edx
   movl  0xC(%esp),%edi
   movl  0x10(%esp),%ecx
   rep
   insw
   popl  %edi
   ret

/*
 * repoutsw(port, buffer, count)
 */
   .globl   _repoutsw
   .align   4
_repoutsw:
   pushl %esi
   movl  0x8(%esp),%edx
   movl  0xC(%esp),%esi
   movl  0x10(%esp),%ecx
   rep
   outsw
   popl  %esi
   ret

/*
 * repinsl(port, buffer, count)
 */
   .globl   _repinsl
   .align   4
_repinsl:
   pushl %edi
   movl  0x8(%esp),%edx
   movl  0xC(%esp),%edi
   movl  0x10(%esp),%ecx
   rep
   insl
   popl  %edi
   ret

/*
 * repoutsl(port, buffer, count)
 */
   .globl   _repoutsl
   .align   4
_repoutsl:
   pushl %esi
   movl  0x8(%esp),%edx
   movl  0xC(%esp),%esi
   movl  0x10(%esp),%ecx
   rep
   outsl
   popl  %esi
   ret
