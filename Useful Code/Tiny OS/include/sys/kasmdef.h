/*
 * kernel assembly defines
 *
 * $Header$
 * $Log$
 *
 */
#ifndef _KASMDEF_H_
#define _KASMDEF_H_


#define ENTER_KERNEL \
	cld		; \
	pushal  	; \
	pushw %es 	; \
	pushw %ds

#define LEAVE_KERNEL \
	popw	%ds	; \
	popw	%es	; \
	popal

#define KERNEL_SEGS \
	movl	$0x10, %eax	; \
	movw	%ax, %ds	; \
	movw	%ax, %es	

#endif /* _KASMDEF_H_ */
