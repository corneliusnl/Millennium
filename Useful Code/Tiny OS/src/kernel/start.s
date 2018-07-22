/*
 * kernel starts its life here
 *
 * $Id: START.S 1.1 1996/11/26 15:45:17 bart Exp $
 *
 */

/*
 * initial stack
 */
	.globl	_os_stack
.data
	.space 0x1000
_os_stack:
	.long	0

/*
 * so like... let's roll
 */	
	.globl	start
.text
start:
	/*
 	 * clear NT, learned the hard way...
	 */
	pushl	$0
	popfl

	/*
 	 * setup our stack
	 */
	lea	_os_stack, %eax
	movl	%eax, %esp

	/*
	 * clear bss
	 */

	xorl	%eax, %eax
	movl	$edata, %edi
	movl	$end, %ecx
	subl	%edi, %ecx
	cld
	rep	
	stosb

	/*
	 * call our C code initialization ...
	 */

	call	_entry


