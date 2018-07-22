; $Header$
;
; Millennium Operating System
; Copyright 1997-1998 (c) Leprechaun Software.  All Rights Reserved.
;

	INCLUDE	ints.inc

SEGMENT ProtectedSeg PUBLIC USE32
;--------------------------------------------------------------
; Interrupt 0 - Divide Error
;--------------------------------------------------------------
; Causes:
;  -Divisor is zero
;--------------------------------------------------------------
Except_Div	PROC
;	mov	ecx, DivZero_Len
;	mov	esi, offset DivZero
;	mov	edi, 0
;	call	Print
	iretd

;DivZero	db	'Divide by zero!'
;DivZero_Len	EQU	$ - DivZero
Except_Div	ENDP

;--------------------------------------------------------------
; Interrupt 1 - Debug
;--------------------------------------------------------------
; Causes:
;--------------------------------------------------------------
Except_Debug	PROC
	iretd
Except_Debug	ENDP

;--------------------------------------------------------------
; Interrupt 2 - NMI (Non-Maskable Interrupt)
;--------------------------------------------------------------
; Causes:
;  -Hardware disaster such as a power down or parity check error
;--------------------------------------------------------------
Except_NMI	PROC
;	mov	ecx, NMI_LEN
;	mov	esi, offset NMI
;	mov	edi, 0
;	call	Print
	iretd

;NMI		db	'Non-Maskable Interrupt!'
;NMI_Len	EQU	$ - NMI
Except_NMI	ENDP

;--------------------------------------------------------------
; Interrupt 3 - Breakpoint
;--------------------------------------------------------------
; Causes:
;--------------------------------------------------------------
Except_BreakPt	PROC
	iretd
Except_BreakPt	ENDP

;--------------------------------------------------------------
; Interrupt 4 - Overflow
;--------------------------------------------------------------
; Causes:
;  -When the INTO instruction is executed with the OF flag set
;--------------------------------------------------------------
Except_Overflow	PROC
	iretd
Except_Overflow	ENDP

;--------------------------------------------------------------
; Interrupt 5 - BOUND Range Exceeded
;--------------------------------------------------------------
; Causes:
;  -When the BOUND instruction's operand is out of range
;--------------------------------------------------------------
Except_RangeExc	PROC
	iretd
Except_RangeExc	ENDP

;--------------------------------------------------------------
; Interrupt 6 - Invalid OpCode
;--------------------------------------------------------------
; Causes:
;  -When an attempt to execute a command is not identified by the processor
;  -When an operand is illegal for this command
;--------------------------------------------------------------
Except_InvOpCode	PROC
	iretd
Except_InvOpCode	ENDP

;--------------------------------------------------------------
; Interrupt 7 - Device Not Available
;--------------------------------------------------------------
; Causes:
;  -The processor executes a floating-point instruction and the EM
;	(emulation) bit in CR0 is set.  This allows for the emulation
;	of floating-point arithmetic on machines without a coprocessor.
;  -The processor executes a floating-point instruction and both the
;	MP (math present) and TS (task switched) bits in the CR0 are
;	set.  This preserves the coprocessor state in a multitasking
;	system.
;--------------------------------------------------------------
Except_DeviceNA	PROC
	iretd
Except_DeviceNA	ENDP

;--------------------------------------------------------------
; Interrupt 8 - Double Fault
;--------------------------------------------------------------
; Causes:
;  -Generated whe the processor detects an exception while calling
;	a handler forr a prior exception.  Always push a error code
;	on stack, but always zero.
;--------------------------------------------------------------
Except_Double	PROC
	iretd
Except_Double	ENDP

;--------------------------------------------------------------
; Interrupt 9 - Coprocessor Segment Overrun
;--------------------------------------------------------------
; Causes:
;  -When a part of a floating-point operand violates page or
;	segment protection.  486+ machine generate INT 13 instead.
;--------------------------------------------------------------
Except_Coprocess	PROC
	iretd
Except_Coprocess	ENDP

;--------------------------------------------------------------
; Interrupt 10 - Invalid Task State Segment
;--------------------------------------------------------------
; Causes:
;  -When a task switch to a segment with an invalid TSS is attempted.
;--------------------------------------------------------------
Except_TSS	PROC
	iretd
Except_TSS	ENDP

;--------------------------------------------------------------
; Interrupt 11 - Segment Not Present
;--------------------------------------------------------------
; Causes:
;  -Attempting to load the CS,DS,ES,FS, or GS registers; loading
;	the SS register, however, causes a stack fault.
;  -While attempting to load the LDT register using and LLDT
;	instruction; loading the LDT register during a task switch
;	operation, however, causes an invalid TSS exection.
;  -While attempting to use a gate descriptor which is marked
;	segment not present.
;--------------------------------------------------------------
Except_Segment	PROC
	iretd
Except_Segment	ENDP

;--------------------------------------------------------------
; Interrupt 12 - Stack Exception
;--------------------------------------------------------------
; Causes:
;  -If any command reffering to the stack via SS register,
;	implicity (PUSH, POP, ENTER, LEAVE) or explicity
;	(MOV EAX,[EBP+8] or MOV EAX, SS:[ESI+8]) results
;	in a stack limit violation.
;  -On an attempt to load SS with a selector of a non-present segment
;--------------------------------------------------------------
Except_Stack	PROC
	iretd
Except_Stack	ENDP

;--------------------------------------------------------------
; Interrupt 13 - General Protection Fault
;--------------------------------------------------------------
; Causes:
;  -Exceeding the segment limit when using the CS,DS,ES,FS, or GS segments
;  -Exceeding the segment limit when referencing a descriptor table
;  -Transferring execution to a segment which is not executable
;  -Writing to a read-only data segment or a code segment
;  -Reading from an execute-only code segment
;  -Loading the SS register with a selector for a read-only
;	segment (unless the selector comes from a TSS during
;	a task switch, in which case an invalid-TSS exeception occurs)
;  -Loading the SS,DS,ES,FS,or GS register with a selector for a
;	system segment.
;  -Loading the DS,ES,FS, or GS register with a selector for an
;	execute only code segment.
;  -Loading the SS register with the selector of an executable segment
;  -Accessing memory using the DS,ES,FS, or GS register when it
;	contains a null selector.
;  -Switching to a busy task
;  -Violating privilage rules
;  -Exceeding the instruction length limit of 15 bytes (this can
;	occur when redundant prefixes are placed beforean instruction.)
;  -Loading the CR0 register with a set PG bit (paging enabled)
;	and a clear PE bit (protection disabled).
;  -Interrupt or exception through an interrupt or trap gate from
;	virtual-8086 mode to a handler at a privilagee level other
;	than 0.
;  -Attempting to write a one into a reserved bit of CR4
;  -An operand of the instructionn
;  -A selector from a gate which is the operand of the instruction
;  -A selector from a TSS involved in a task switch
;--------------------------------------------------------------
Except_GP	PROC
	iretd
Except_GP	ENDP

;--------------------------------------------------------------
; Interrupt 14 - Page Fault
;--------------------------------------------------------------
; Causes:
;  -When a page is not present in memory
;  -Running code has insufficient privilagee to access a page
;--------------------------------------------------------------
Except_Page	PROC
	iretd
Except_Page	ENDP

;--------------------------------------------------------------
; Interrupt 15 - Reserved
;--------------------------------------------------------------
Except_Reserved	PROC
	iretd
Except_Reserved	ENDP

;--------------------------------------------------------------
; Interrupt 16 - Floating Point Error
;--------------------------------------------------------------
; Causes:
;  -Any error in a floating point instruction
;--------------------------------------------------------------
Except_FloatingPt	PROC
	iretd
Except_FloatingPt	ENDP

;--------------------------------------------------------------
; Interrupt 17 - Alignment Check (486 only)
;--------------------------------------------------------------
; Causes:
;  -On a reference to an unaligned operand
;--------------------------------------------------------------
Except_AlignChk	PROC
	iretd
Except_AlignChk	ENDP

;--------------------------------------------------------------
; Interrupt 18 - Machine Check
;--------------------------------------------------------------
Except_MachineChk	PROC
	iretd
Except_MachineChk ENDP

ENDS ProtectedSeg
