#include <sys/arg.h>
#include <sys/syscall.h>

	.data
	.globl	__magic
__magic:.long	0xDEADBEEF
argc:	.long	0x0
	.space	MAX_ARGSZ
argv:	.space	MAX_ARGNO*4

	.text
	.globl	start, _main, ___main
start:
	/*
	 * clear BSS
	 */
	xorl 	%eax, %eax
	movl	$edata, %edi
	movl	$end, %ecx
	subl	%edi, %ecx
	cld
	rep
	stosb

	call	_get_curpos
	
	/*
	 * setup stack with argc, argv
	 */
	pushl	$argv
	pushl	argc

	/*
	 * call main()
 	 */
	call	_main

	/*
	 * embedded exit
	 */
	push	$0
	int	$SYSCALL_EXIT		

___main:	
	ret

