	IDEAL
	MODEL large

	DATASEG

BasePort	dw	220h

	CODESEG

	PUBLIC _DSP_Initialize
	PUBLIC _DSP_GetVersion
	PUBLIC _DSP_Read
	PUBLIC _DSP_Write

;
; void DSP_Initialize()
;
; Description
;  Resets the DSP (Digitial Signal Processor)... which leaves
;  the speakers disabled.
;
; Parameters
;  None
;
; Returns
;  If successful ? 1 : 0
;
PROC	_DSP_Initialize
	mov     dx, [BasePort]
	add     dx, 6		; Reset port, Base + 6
	mov     al, 1
	out     dx, al		; Send 1 to reset port

	mov     cx, 500		; Wait a little...

WaitLoop:
	loop    WaitLoop

	xor     al, al
	out     dx, al		; Send 0 to reset port

	xor     cx, cx		; Try up to 10000h times
	add     dx, 8		; Read-Buffer Status port
				; Base + Eh
NoData:
	in      al, dx		; Read Data-Ready status
	or      al, al		; Is bit 7 set?
	jns     TryAgain	; No, data not ready

	sub     dx, 4		; Read Data port, Base + Ah
	in      al, dx		; Read DSP data
	cmp     al, 0AAh	; Is it AAh?
	je      Success         ; Yes, O.K.

TryAgain:
	loop    NoData          ; Try more...

	mov     ax, 0		; Sound Blaster not found...
	ret

Success:
	mov     ax, 1		; Sound Blaster initialized successfully!
	ret
ENDP	_DSP_Initialize

;
; void DSP_Write(unsigned char value)
;
; Description
;  Writes a byte to the DSP port
;
; Parameters
;  Value	- Value to be written
;
PROC	_DSP_Write
	ARG	Value:byte

	push	bp
	mov	bp, sp
	push	dx

	mov	dx, [BasePort]
	add	dx, 0Ch		; Write status port, Base + 0Ch

WriteBusy:
	in	al, dx		; Get status
	or	al, al
	js	WriteBusy	; Bit 7 set, wait...

	mov	al, [Value]	; Get command or data into AL
	out	dx, al		; Send it to DSP

	pop	dx
	pop	bp
	ret
ENDP	_DSP_Write

;
; unsigned char DSP_Read()
;
; Description
;  Reads byte from DSP and returns it
;
; Parameters
;  None
;
; Returns
;  Byte read
;
PROC	_DSP_Read
	push	dx
	mov	dx, [BasePort]
	add	dx, 0Eh		; Write status port, Base + 0Eh

ReadBusy:
	in	al, dx		; Get status
	or	al, al
	jns	ReadBusy	; Bit 7 clear, wait...

	sub	dx, 4	      	; Read Data port, Base + 0Ah
	in	al, dx		; Read byte from DSP
	pop	dx

	xor	ah, ah
	ret
ENDP	_DSP_Read

;
; int DSP_GetVersion();
;
; Description
;  Obtains the version of the DSP
;
; Parameters
;  None
;
; Returns
;  Version with major revision in high byte and the minor in the low byte
;
PROC	_DSP_GetVersion
	push	0E1h		; Command : Get DSP Version
	call	_DSP_Write	; Send the command
	call	_DSP_Read	; Read version's major byte
	mov	ah, al		; Major into AH
	call	_DSP_Read	; Read version's minor byte

	ret
ENDP	_DSP_GetVersion

	END
