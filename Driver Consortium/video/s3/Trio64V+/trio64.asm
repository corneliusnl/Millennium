; Millennium
; Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
;
; Authors:
;  Jeremiah Cornelious McCarthy		lepresoft@nf.sympatico.ca
;

;
; Description
;  Probes for the S3 Trio64V+ chipset using the exact code presented in
;  their technical documentation.
;
PROC _TRIO_Probe
	mov	dx, 3D4h	; copy index register address into dx
	mov	al, 2Eh		; copy index for CR2E register into al
	out	dx, al		; write index to index register
	inc	dx		; increment dx to 3D5h (data register address)
	in	al, dx		; read content of CR2E into al
	cmp	al, 11h		; compare chip ID to the desired chip ID (11h)
	jne	TrioNotPresent	; exit if chip ID does not match desired ID
	mov	dx, 3D4h	; copy index register address into dx
	mov	al, 2Fh		; copy index for CR2F register into al
	out	dx, al		; write index to index register
	inc	dx		; increment dx to 3D5h (data register address)
	in	al, dx		; read content of CR2F into al
	and	al, 40h		; mask out all but bit 6
	cmp	al, 40h		; compare chip revision to the desired chip revision (Exh)
	jne	TrioNotPresent	; exit if revision is not correct

	mov	ax, 1
	ret

TrioNotPresent:
	mov	ax, 0
	ret	
ENDP _TRIO_Probe

;
; NOTE - Locking and Unlocking
;
;  Locking and unlocking functions are used because the chipsets
;  provide additional registers beyond those in the standard VGA
;  set and their access is restricted by these functions to
;  prevent accidental access.
;

;
; Description
;  Enables access to the S3 extended registers
;
PROC _TRIO_Unlock
	; Enable access to the extended Sequencer registers
	mov	dx, 3C4h	; copy index register address into dx
	mov	al, 08h		; copy index for SR8 register into al
	out	dx, al		; write index to index register
	inc	dx		; increment dx to 3C5h (data register address)
	mov	al, 06h		; copy unlocking code (xxxx0110b, x=don't care) to al
	out	dx, al		; write the unlocking code to the data register
	dec	dx		; restore the index register address to dx

	; Enable access to the extended CRTC registers (CR2D-CR3F)
	mov	dx, 3D4h	; copy index register address into dx
	mov	al, 38h		; copy index for CR38 into al
	out	dx, al		; write index to index register
	inc	dx		; increment dx to 3D5h (data register address)
	mov	al, 48h		; copy unlocking code (01xx10xxb, x=don't care) to al
	out	dx, al		; write the unlocking code to the data register
	dec	dx		; restore the index register address to dx

	; Enable access to the extended CRTC registers (CR40-CRFF)
	mov	al, 39h		; copy index for CR39
	out	dx, al		; write index to index register
	inc	dx		; increment dx to 3D5h (data register address)
	mov	al, 0A5h	; copy unlocking code to al (the code A5H also unlocks
				; access to configuration registers CR36, CR37, and CR68)
	out	dx, al		; write unlocking code to the data register
	dec	dx		; restore the index register address to dx

	; Enable access to the Enhansed Programming registers
	mov	al, 40h		; copy index for CR40
	out	dx, al		; write index to index register
	inc	dx		; increment dx to 3D5h (data register address)
	in	al, dx		; read register data for read/modify/write operation
	or	al, 1		; set bit 0 to 1
	dec	dx, al		; write the unlocking code to the data register
	dec	dx		; restore the index register address to dx

	ret			; return
ENDP _TRIO_Unlock

;
; Description
;  Prevents the S3 extended registers from being accessed
;
PROC _TRIO_Lock
	mov	dx, 3D4h	; copy index register address into dx
	mov	al, 40h		; copy index for CR40 register into al
	out	dx, al		; write index to index register
	inc	dx		; increment dx to 3D5h (data register address)
	in	al, dx		; read content of CR40 into al
	and	al, 0FEh	; clear bit 0 to 0
	out	dx, al		; write CR40 to lock the Enhansed Commands register
	dec	dx		; restore the index register address to dx

	ret			; return
ENDP _TRIO_Lock