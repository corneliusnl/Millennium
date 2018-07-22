;
; Millennium Operating System Assembler
; Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
;

STRUC	TestStruct
	Element1	db	0
ENDS	TestStruct

PROC	TestProc
ENDP	TestProc

TestEqu	EQU	0

SEGMENT	TestSegment USE16
	ORG	0
TestLabel:
	mov	ah, FFh
ENDS	TestSegment
