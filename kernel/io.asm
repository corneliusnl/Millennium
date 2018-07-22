	IDEAL
	MODEL	large

	CODESEG

	PUBLIC _outb
	PUBLIC _outw
	PUBLIC _inb
	PUBLIC _inw

;
; void outb(int port, unsigned char value)
;
PROC	_outb
	ARG	port:word, value:byte

	push	bp
	mov	bp, sp

	mov	dx, [port]
	mov	al, [value]
	out	dx, al

	pop	bp
	ret
ENDP	_outb

;
; void outw(int port, int value)
;
PROC	_outw
	ARG	port:word, value:word

	push	bp
	mov	bp, sp

	mov	dx, [port]
	mov	ax, [value]
	out	dx, ax

	pop	bp
	ret
ENDP	_outw

;
; unsigned char inb(int port)
;
PROC	_inb
	ARG	port:word

	push	bp
	mov	bp, sp

	mov	dx, [port]
	xor	ax, ax
	in		al, dx

	pop	bp
	ret
ENDP	_inb

;
; int inw(int port)
;
PROC	_inw
	ARG	port:word

	push	bp
	mov	bp, sp

	mov	dx, [port]
	in		ax, dx

	pop	bp
	ret
ENDP	_inw

	END
