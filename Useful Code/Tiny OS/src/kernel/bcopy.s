/*
 * bzero, bcopy
 * from VSTa
 * Copyright (c) 1996 Andy Valencia
 *
 * $Id: BCOPY.S 1.1 1996/11/26 15:44:49 bart Exp $
 *
 */

/*
 * bzero()
 * Zero some memory
 */
   .globl   _bzero
   .align   4

_bzero:
   pushl %edi
   movl  0x8(%esp),%edi
   movl  0xC(%esp),%ecx
   cld
   xorl  %eax,%eax
   cmpl  $4,%ecx           /* Zero longwords */
   jb 1f
   shrl  $2,%ecx           /* Scale to longword count */
   rep
   stosl
   movl  0xC(%esp),%ecx    /* Calculate byte resid */
   andl  $3,%ecx
   jnz   1f
   popl  %edi
   ret

1: rep            
   stosb
   popl  %edi
   ret

/*
 * bcopy()
 * Copy some memory
 */
   .globl   _bcopy
   .align   4

_bcopy:
   pushl %esi
   pushl %edi
   movl  0xC(%esp),%esi
   movl  0x10(%esp),%edi
   movl  0x14(%esp),%ecx
   cmpl  %edi,%esi      
   jb 2f

1: cld
   cmpl  $4,%ecx        /* Move longwords */
   jb 5f
   shrl  $2,%ecx        /* Scale units */
   rep
   movsl
   movl  0x14(%esp),%ecx
   andl  $3,%ecx
   jnz   5f
   popl  %edi           /* Restore registers and return */
   popl  %esi
   ret

5: rep         
   movsb
   popl  %edi           /* Restore registers and return */
   popl  %esi
   ret

   .align   4,0x90

2: addl  %ecx,%esi
   cmpl  %edi,%esi
   jae   4f
   subl  %ecx,%esi      
   jmp   1b       

4: std            
   decl  %esi
   addl  %ecx,%edi
   decl  %edi
   rep
   movsb
   cld
   popl  %edi           /* Restore registers and return */
   popl  %esi
   ret
