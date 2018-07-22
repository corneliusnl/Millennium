;
; SB Mixer
; Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
;

	IDEAL
	MODEL	LARGE
        INCLUDE	sb.inc

	CODESEG

	PUBLIC _MIX_Reset
	PUBLIC _MIX_SetOutput
	PUBLIC _MIX_SetInput
	PUBLIC _MIX_SetVolume
	PUBLIC _MIX_GetVolume

;
; void MIX_Reset()
;
PROC	_MIX_Reset
	mov	dx, io_addr
	add	dx, MIX_ADR_OFF
	mov	al, 0
	out	dx, al
	inc	dx
	out	dx, al

	ret
ENDP	_MIX_Reset

;
; void SetOutput(unsigned char Output)
;
PROC	_MIX_SetOutput
	ARG	Output:byte

	push	bp
	mov	bp, sp

	mov	dx, io_addr
	add	dx, MIX_ADR_OFF
	mov	al, MIX_OUTPUT
	out	dx, al
	inc	dx
	mov	al, [Output]
	out	dx, al

	pop	bp
	ret
ENDP	_MIX_SetOutput

;
; void SetInput()
;
PROC	_MIX_SetInput
	ARG	Input:byte

	push	bp
	mov	bp, sp

	mov	dx, io_addr
	add	dx, MIX_ADR_OFF
	mov	al, MIX_INPUT
	out	dx, al
	inc	dx
	mov	al, opt
	out	dx, al

	pop	bp
	ret
ENDP	_MIX_SetInput

;
; void SetVolume(unsigned char Type, unsigned char Right, unsigned char Left)
;
PROC	_MIX_SetVolume
	ARG	Type:byte, Right:byte, Left:byte

	push	bp
	mov	bp, sp

	mov	dx, io_addr
	add	dx, MIX_ADR_OFF
	mov	al, [reg]
	out	dx, al
	inc	dx
	mov	al, [Left]
	mov	cl, 4
	shl	al, cl
	or	al, [Right]
	out	dx, al

	pop	bp
	ret
ENDP	_MIX_SetVolume

;
; void GetVolume(unsigned char Type, unsigned char Right, unsigned char Left)
;
PROC	_MIX_GetVolume
	ARG	Type:byte, Right:byte, Left:byte

	push	bp
	mov	bp, sp

	mov	dx, io_addr
	add	dx, MIX_ADR_OFF
	mov	al, Type
	out	dx, al
	inc	dx
	in	al, dx
	mov	ah, al
	les	bx, Right
	and	al, 0xf
	mov	es:[bx], al
	mov	al, ah
	les	bx, Left
	mov	cl, 4
	shr	al, cl
	mov	es:[bx], al

	pop	bp
	ret
ENDP	_MIX_GetVolume

	END
