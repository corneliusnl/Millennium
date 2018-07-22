; $Header$
;
; Millennium Boot Sector
;
; Copyright 1997-1998 (c) Leprechaun Software.  All Rights Reserved.
;
; Purpose:
;  To detect if the host processor is a 386+ and if so load the
; millennium kernel, otherwise it is to display a message and halt.
;

	BITS	16
	ORG	7C00h

	;
	; Print probe message
	;
	mov	ah, 03h		; AH = Read cursor position
	xor	bh, bh		; BH = Video page
	int	10h		; Video BIOS

	mov	ax, 1301h	; AH = Write string
				; AL = Write mode
	mov	bx, 0007h	; BH = Video page number
				; BL = Attribute
	mov	cx, Check_Len	; CX = String length
	mov	bp, Check	; ES:BP = String pointer
	int	10h		; Video BIOS

	;
	; Ensure processor is 386+
	;
	pushf			; Save flags for later
	xor	ah, ah		; Clear high byte
	push	ax		; Push AX onto stack
	popf			; Pop this value into the flag register
	pushf			; Push flags onto stack
	pop	ax		; ...and get them into AX
	and	ah, 0F0h	; Try to set the high nibble
	cmp	ah, 0F0h	; On a 386, the high nibble can never be 0FF0h
	je	Not386		; Not a 386+ processor
	mov	ah, 70h		; Now try to set NT and IOPL
	push	ax		; Push AX on the stack
	popf			; ...and pop the value into the flags register
	pushf			; Push flags on the stack
	pop	ax		; ...and get flags into AX
	and	ah, 70h		; If they couldn't be modified no 386 is installed
	jz	Not386		; Not a 386+ processor
	popf			; Restore flags
	jmp	Start

Not386:
	;
	; Print unable to load message
	;
	mov	ah, 03h		; AH = Read cursor position
	xor	bh, bh		; BH = Video page
	int	10h		; Video BIOS

	mov	ax, 1301h	; AH = Write string
				; AL = Write mode
	mov	bx, 0007h	; BH = Video page number
				; BL = Attribute
	mov	cx, Failed_Len	; CX = String length
	mov	bp, Failed	; ES:BP = String pointer 
	int	10h

	jmp	$

Start:
	;
	; Print loading message	
	;
	mov	ah, 03h		; AH = Read cursor position
	xor	bh, bh		; BH = Video page
	int	10h		; Video BIOS

	mov	ax, 1301h	; AH = Write string
				; AL = Write mode
	mov	bx, 0007h	; BH = Video page number
				; BL = Attribute
	mov	cx, Loading_Len	; CX = String length
	mov	bp, Loading	; ES:BP = String pointer
	int	10h		; Video BIOS

	;
	; Load kernel from disk
	;
	xor	ah, ah		; AH = Reset disk system
	xor	dl, dl		; DL = Driver number
	int	12h		; Diskette BIOS

	mov	ax, 0212h	; AH = Read disk sectors
				; AL = Number of sectors
	mov	cx, 0002h	; CH = Cylinder number
				; CL = Sector number
	mov	dx, 0000h	; DH = Head number
				; DL = Drive number
	push	word 0060h	; Push 0060h
	pop	es		; ... into es
	xor	bx, bx		;
	int	13h		; Diskette BIOS

	mov	dx, 3F2h	; Kill motor
	xor	al, al
	out	dx, al

	;
	; Jump to kernel
	;
	jmp	0060h:0000h

Check		db	'Checking for 386+ processor... '
Check_Len	EQU	$ - Check

Loading		db	'found!',13,10
		db	'Loading the Millennium Operating System...',13,10
Loading_Len	EQU	$ - Loading

Failed		db	'failed!',13,10,
		db	'A 386+ processor is required for the Millennium Operating System!'
Failed_Len	EQU	$ - Failed

	TIMES	(510-($-$$)) db 0
	dw	0AA55h	; Boot sector magic number to validate boot sector
