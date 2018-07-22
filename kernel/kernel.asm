; $Header$
;
; Millennium Operating System Kernel
;
; Purpose:
;  Put the processor into protected mode.
;

	%include "kernel.inc"
	%include "pmode.inc"

SEGMENT RealSeg USE16
	;
	; Print protected mode message
	;
	mov	ah, 03h			; AH = Read cursor position
	xor	bh, bh			; BH = Video page
	int	10h			; Video BIOS

	mov	ax, 1301h		; AH = Write string
					; AL = Write mode
	mov	bx, 0007h		; BH = Video page number
					; BL = Attribute
	mov	cx, ProtMode_Len	; CX = String length
	mov	bp, ProtMode		; ES:BP String pointer
	int	10h			; Video BIOS

	cli				; Clear interrupts

	;
	; Initialize Segment Descriptor Tables
	;
	xor	eax, eax
	mov	ax, cs			; AX = CS
	add	ax, RealSeg_SIZE	; Add protected segment offset
	mov	ds, ax			; DS = AX
	shl	eax, 4			; Convert to linear address
	mov	edx, eax		; EDX = EAX

	mov	[gdt_Code32.Base0_15], ax	; ????XXXXh
	mov	[gdt_Data32.Base0_15], ax	; ????XXXXh
	shr	eax, 16
	mov	[gdt_Code32.Base16_23], al	; ??XX????h
	mov	[gdt_Data32.Base16_23], al	; ??XX????h
	mov	[gdt_Code32.Base24_31], ah	; XX??????h
	mov	[gdt_Data32.Base24_31], ah	; XX??????h

	mov	eax, edx		; EAX = EDX
	add	eax, offset gdt_Null	; Add GDT offset
	mov	[GDTDesc.Base], eax	; Set GDT's base address
	mov	eax, edx		; EAX = EDX
	add	eax, offset idt_Int0	; Add IDT offset
	mov	[IDTDesc.Base], eax	; Set IDT's base address

	;
	; Load descriptor tables
	;
	lgdt	[GDTDesc]		; Load global descriptor table
	lidt	[IDTDesc]		; Load interrupt descriptor table

	;
	; Enable A20 gate
	;
	call	Clear8042
	mov	al, 0D1h		; Command write
	out	64h, al
	call	Clear8042
	mov	al, 0DFh		; A20 on
	out	60h, al
	call	Clear8042

	;
	; Enable protected mode bit
	;
	mov	eax, cr0		; EAX = CR0
	or	al, 1			; Enable PE = Protected Enable bit
	mov	cr0, eax		; CR0 = EAX

	;
	; Jump to 32-bit segment
	;
	db	66h			; Operand size override
	db	0EAh			; Jump far
	dd	offset Start32		; Offset
	dw	Code32_Idx		; Segment

	;
	; Segment Data
	;

Clear8042	PROC
	in	al, 64h	; 8042 status port
	test	al, 1		; Output buffer?
	jz	Empty		; Buffer is empty
	in	al, 60h		; Read it
	jmp	Clear8042	; Try again

Empty:
	test	al, 2		; Is input buffer full?
	jnz	Clear8042	; yes - loop
	ret			; Return
Clear8042	ENDP

	ProtMode		db	'Attempting to enter protected mode...'
	ProtMode_Len	EQU	$ - ProtMode

	ALIGN 16
	RealSeg_SIZE	EQU (($-RealSeg) SHR 4)
ENDS RealSeg

;--------------------------------------------------------------
; Protected Mode Code Segment
;--------------------------------------------------------------
SEGMENT ProtectedSeg USE32
	ASSUME	cs:ProtectedSeg, ds:ProtectedSeg

Start32:
	mov	ax, Data32_Idx		; AX = Data32_Idx
	mov	ds, ax			; DS = AX
	mov	es, ax			; ES = AX
	mov	ss, ax			; SS = AX
	mov	esp, offset Stack32	; Setup stack

	mov	ah, 0E4h
	call	SetAttrib

	call	CLS			; Clear screen
	mov	ecx, Welcome_Len	; ECX = string lengthh
	mov	ebp, offset Welcome	; DS:EBP = pointer to string
	call	Print			; Print
	
	jmp	$			; Hang forever

	;
	; Segment Data
	;
	Welcome	db	'ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿',10
			db	'³       ê OMEGOS 32-bit Operating System        ³',10
			db	'ÆÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍÍµ',10
			db	'³    Copyright (c) 1997 Leprechaun Software     ³',10
			db	'ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´',10
			db	'³ What has occured is:                          ³',10
			db	'³ 1. The machine loaded the boot sector         ³',10
			db	'³ 2. This boot sector then loaded the kernel    ³',10
			db	'³ 3. The kernel placed the processor into       ³',10
			db	'³    protected mode, in which the processor     ³',10
			db	'³    can directly address up to 4GB of memory.  ³',10
			db	'³    It also set up the Interrupt Descriptor    ³',10
			db	'³    table, and associated routines to handle   ³',10
			db	'³    the interrupts.                            ³',10
			db	'³ 4. The kernel then called an interrupt to     ³',10
			db	'³    print this message to the screen.          ³',10
			db	'³                                               ³',10
			db	'³ While all of the components have yet to be    ³',10
			db	'³ intergrated with each other... the source code³',10
			db	'³ and documentation is sure to ensure OMEGOS    ³',10
			db	'³ success!                                      ³',10
			db	'ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ'
	Welcome_Len	EQU	$ - Welcome

	;--------------------------------------------------------------
	; Global Descriptor Table
	; SegmentDesc <Limit0_15, Base0_15, Base16_23, Misc1, Misc2, Base24_31>
	;--------------------------------------------------------------
	; Misc1	- [TYPE][S][DPL][P]
	;--------------------------------------------------------------
	; S		- Descriptor type (0 = system, 1 = application)
	; DPL		- Descriptor Privilage Level (2 bits)
	; P		- Segment present
	;--------------------------------------------------------------
	; Misc2	- [G][D/B][0][AVL][SEG LIMIT 19:16]
	;--------------------------------------------------------------
	; G		- Granularity
	; D/B		- Default operation size (0 = 16-bit, 1 = 32-bit)
	; 0		- Reserved
	; AVL		- Available for use by system software
	;--------------------------------------------------------------
	GDTDesc		DescTable	<GDT_Size, 000000000h>
	gdt_Null	SegmentDesc	<00000h, 00000h, 000h, 000h, 000h, 000h>
	gdt_Code32	SegmentDesc	<0FFFFh, 00000h, 000h, 09Ah, 0CFh, 000h>
	gdt_Data32	SegmentDesc	<0FFFFh, 00000h, 000h, 092h, 0CFh, 000h>
	gdt_Video	SegmentDesc	<0FFFFh, 08000h, 00Bh, 092h, 0CFh, 000h>
	GDT_Size	EQU		$ - gdt_Null - 1

	;--------------------------------------------------------------
	; Interrupt Descriptor Table
	; IntDesc <Offset0_15, Selector0_15, ZeroByte, Flags, Offset16_31>
	;--------------------------------------------------------------
	; Flags	- [TYP][DPL][P]
	;--------------------------------------------------------------
	; DPL		- Descriptor Privilage Level
	; P		- Segment present
	; TYP		- IDT type (10100 = Task gate, 01110 = Interrupt Gate, 11110 = Trap Gate)
	;--------------------------------------------------------------
	IDTDesc		DescTable	<IDT_Size, 000000000h>
	idt_Int0	IntDesc	<offset low Except_Div,		Code32_Idx, 000h, 08Eh, 00000h>
	idt_Int1	IntDesc	<offset low Except_Debug,	Code32_Idx, 000h, 08Eh, 00000h>
	idt_Int2	IntDesc	<offset low Except_NMI,		Code32_Idx, 000h, 08Eh, 00000h>
	idt_Int3	IntDesc	<offset low Except_BreakPt,	Code32_Idx, 000h, 08Eh, 00000h>
	idt_Int4	IntDesc	<offset low Except_Overflow,	Code32_Idx, 000h, 08Eh, 00000h>
	idt_Int5	IntDesc	<offset low Except_RangeExc,	Code32_Idx, 000h, 08Eh, 00000h>
	idt_Int6	IntDesc	<offset low Except_InvOpCode,	Code32_Idx, 000h, 08Eh, 00000h>
	idt_Int7	IntDesc	<offset low Except_DeviceNA,	Code32_Idx, 000h, 08Eh, 00000h>
	idt_Int8	IntDesc	<offset low Except_Double,	Code32_Idx, 000h, 08Eh, 00000h>
	idt_Int9	IntDesc	<offset low Except_CoProcess,	Code32_Idx, 000h, 08Eh, 00000h>
	idt_Int10	IntDesc	<offset low Except_TSS,		Code32_Idx, 000h, 08Eh, 00000h>
	idt_Int11	IntDesc	<offset low Except_Segment,	Code32_Idx, 000h, 08Eh, 00000h>
	idt_Int12	IntDesc	<offset low Except_Stack,	Code32_Idx, 000h, 08Eh, 00000h>
	idt_Int13	IntDesc	<offset low Except_GP,		Code32_Idx, 000h, 08Eh, 00000h>
	idt_Int14	IntDesc	<offset low Except_Page,	Code32_Idx, 000h, 08Eh, 00000h>
	idt_Int15	IntDesc	<offset low Except_Reserved,	Code32_Idx, 000h, 08Eh, 00000h>
	idt_Int16	IntDesc	<offset low Except_FloatingPt,	Code32_Idx, 000h, 08Eh, 00000h>
	idt_Int17	IntDesc	<offset low Except_AlignChk,	Code32_Idx, 000h, 08Eh, 00000h>
	idt_Int18	IntDesc	<offset low Except_MachineChk,	Code32_Idx, 000h, 08Eh, 00000h>
	IDT_Size	EQU	$ - idt_Int0 - 1

	db	100h DUP(?)
Stack32	LABEL	NEAR

ENDS ProtectedSeg
