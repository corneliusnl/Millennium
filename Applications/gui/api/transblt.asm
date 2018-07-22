;--------------------------------------------------------------
; OMEGOS Transparent Blit Primitive
; Copyright 1997 (c) Leprechaun Software. All Rights Reserved
;
; Version: 0.0.0
;
; Purpose:
;
; Author: Jeremiah McCarthy <jmccarth@thezone.net>
;--------------------------------------------------------------

;--------------------------------------------------------------
;  void TransBlt( int x1, int y1, int x2, int y2, void far* sbuffer,
;                 void far* dbuffer );
;--------------------------------------------------------------
; NOTE: assumes dbuffer is 64000 byte 320 x 200 image buffer, and sbuffer
; is a linear buffer holding the image data to be displayed transparently
;--------------------------------------------------------------
_TransBlt	PROC
	ARG	x1:WORD, y1:WORD, x2:WORD, y2:WORD, sbuffer:DWORD, dbuffer:DWORD
	push	bp						; Save bp register
	mov	bp,sp					; Setup stack
	push	bx						; Save bx register
	push	cx						; Save cx register
	push	es						; Save es register
	push	ds						; Save ds register
	push	di						; Save di register
	push	si						; Save si register

	les	di, dbuffer
	lds	si, sbuffer
	mov	ax, 320
	mov	bx, y1
	mul	bx
	add	ax, x1
	add	di, ax
	mov	ax, y2
	sub	ax, y1
	inc	ax
	mov	cx, ax
	mov	bx, x2
	sub	bx, x1
	inc	bx
nextln:
	push	cx
	mov	cx, bx
	push	di
drawln:
	cmp	byte ptr [ds:si], 0
	je		skip
	mov	al, byte ptr [ds:si]
	mov	byte ptr [es:di], al
skip:
	inc	si
	inc	di
	loop	drawln
	pop	di
	add	di, 320
	pop	cx
	loop	nextln

	pop	si						; Restore si register
	pop	di						; Restore di register
	pop	ds						; Restore ds register
	pop	es						; Restore es register
	pop	cx						; Restore cx register
	pop	bx						; Restore bx register
	pop	bp						; Restore bp register
	ret							; Return
_TransBlt 	ENDP
