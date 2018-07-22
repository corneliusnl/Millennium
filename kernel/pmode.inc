; $Header$
;
; Millennium Operating System
; Copyright 1997-1998 (c) Leprechaun Software.  All Rights Reserved.
;

;
; Segment Descriptor
;
SegmentDesc	STRUC
	Limit0_15	dw	?
	Base0_15	dw	?
	Base16_23	db	?
	Misc1		db	?
	Misc2		db	?
	Base24_31	db	?
SegmentDesc	ENDS

;
; Interrupt Descriptor
;
IntDesc STRUC
	Offset0_15	dw	?
	Selector0_15	dw	?
	ZeroByte	db	0
	Flags		db	?
	Offset16_31	dw	?
IntDesc ENDS

;
; Descriptor Table
;
DescTable	STRUC
	Limit		dw	?
	Base		dd	?
DescTable	ENDS

;
; Task State Segment
;
TSS	STRUC
	LINK		dw	0,0
	_ESP0		dd	0
	_SS0		dw	0,0
	_ESP1		dd	0
	_SS1		dw	0,0
	_ESP2		dd	0
	_SS2		dw	0,0
	_CR3		dd	0
	_EIP		dd	0
	_EFLAGS	dd	0
	_EAX		dd	0
	_ECX		dd	0
	_EDX		dd	0
	_EBX		dd	0
	_ESP		dd	0
	_EBP		dd	0
	_ESI		dd	0
	_EDI		dd	0
	_ES		dw	0,0
	_CS		dw	0,0
	_SS		dw	0,0
	_DS		dw	0,0
	_FS		dw	0,0
	_GS		dw	0,0
	_LDT		dw	0,0
	_TBIT		dw	0
	_IOMAP	dw	0
TSS	ENDS
