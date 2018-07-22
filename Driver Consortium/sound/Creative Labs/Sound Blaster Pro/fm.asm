;
; FM Music Chips
; Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
;

	IDEAL
	MODEL	SMALL

	CODESEG

	PUBLIC _FM_Reset
	PUBLIC _FM_Write

;
; void FM_Write(int Data)
;
PROC	_FM_Write
	ARG	Data:word

	push	bp
	mov	bp, sp

	mov	dx, fm_addr;
	mov	ax, [Data];

	xchg	al, ah;
	out	dx, al;

	mov	cx, mue3;
loop1:
	loop	loop1;

	inc	dx;
	mov	al, ah;
	out	dx, al;

	mdelay(mue23);

	pop	bp
	ret
ENDP	_FM_Write
