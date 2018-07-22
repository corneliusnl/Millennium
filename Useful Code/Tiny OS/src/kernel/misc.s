/*
 * misc routines worth or required to be coded in assembly
 * from VSTa
 * Copyright (c) 1996 Andy Valencia
 *
 * $Id: MISC.S 1.1 1996/11/26 15:45:10 bart Exp $
 *
 */

/*
 * cli
 *	Disable maskable processor interrupts
 */
	.globl	_cli
	.align	4
_cli:
	cli
	ret

/*
 * sti
 *	Enable maskable processor interrupts
 */
	.globl	_sti
	.align	4
_sti:
	sti
	ret

/*
 * ltr(uint selector) load task register
 */
	.globl	_ltr
	.align	4
_ltr:
	movl	4(%esp), %eax
	ltr	%ax
	ret


/*
 * ld_cr3(pd) switch address space
 */
	.globl	_ld_cr3
	.align	4
_ld_cr3:
	movl	4(%esp), %eax
	movl	%eax, %cr3
	ret

/*
 * enable_paging
 */
	.globl	_enable_paging
	.align	4
_enable_paging:
	movl	%cr0, %eax
	orl	$0x80000000, %eax
	movl	%eax, %cr0
	jmp	__cpfq
__cpfq:
	ret

/*
	.globl	_test_syscall
	.align	4
_test_syscall:
	mov	$0x20, %eax
	movw	%ax, _sec
	lcall	_off
	ret
*/

/*
 * uint get_cr2()
 */
	.globl	_get_cr2
	.align 	4
_get_cr2:
	movl	%cr2, %eax
	ret

/*
 * uint get_cr3()
 */
	.globl	_get_cr3
	.align 	4
_get_cr3:
	movl	%cr3, %eax
	ret
