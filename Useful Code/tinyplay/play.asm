                Comment &

                Tiny MOD Player v0.1b by Carlos Hasan.

                July 14th, 1993.

                &

                DosSeg                          ; Use Standalone DOS
                                                ; segment ordering.

                Jumps                           ; resolve out of range
                                                ; jumps.

                .Model  Small                   ; Use SMALL memory model.

                .286                            ; enable 80286 code.

                .Stack  0400h                   ; 1Kb for Stack Space.


;               Publics.

                Global  StartPlaying: Proc, StopPlaying: Proc, DmaBuffer: Word
                Global  LoadModule: Proc, FreeModule: Proc, ErrorInfo: Word
                Global  SbInit:Proc, SbDone:Proc, SbPoll: Proc, SbAddr: Word, SbIrq: Word, MixSpeed: Word


;               Resourrces and Data.

                .Data

Credits         db      'Tiny MOD Player v0.1b by Carlos Hasan. July 1993.',10,13,'$'
ErrorMesg       db      'Error loading Module file.',10,13,'$'
MsgNotFound     db      'Sound Blaster not found or IRQ error.',10,13,'$'
MsgFound        db      'Sound Blaster found at Address 2'
PortText        db      'x0h, IRQ '
IrqText         db      'x.',10,13,'$'

Psp             dw      ?
ProgSize        dw      ?

Scope           dw      320 dup (?)
RowOfs          dw      256 dup (?)


;               Start of Code.

                .Code

Main            Proc

                mov     ax,@Data                ; DS points to my DATA
                mov     ds,ax                   ; segment.

                mov     [Psp],es                ; Save the PSP address.

                mov     bx,ss                   ; Shrinks my Memory Block
                mov     cx,sp                   ; to get more core for
                shr     cx,4                    ; the MODule data.
                add     bx,cx
                sub     bx,[Psp]
                inc     bx
                mov     [ProgSize],bx           ; Save my Program Size.

ShrinkProg:     mov     ax,4A00h                ; Shrink!
                mov     bx,[ProgSize]
                int     21h

PrintMesg:      mov     ax,0900h                ; Prints the Credits Text.
                lea     dx,[Credits]
                int     21h

                call    DetectSb                ; Detect the SB Addr, Irq.

GetFileName:    mov     es,[Psp]                ; Parse  the Command line...
                mov     si,80h
                mov     al,es:[si]
                mov     bl,al
                xor     bh,bh
                inc     bx
                mov     Byte Ptr es:[si+bx],0   ; make AsciiZ filename.
ScanName:       inc     si
                mov     al,es:[si]
                test    al,al
                je      Exit
                cmp     al,20h
                je      ScanName                ; scan start of name.
                mov     di,si
ScanPeriod:     inc     si
                mov     al,es:[si]
                cmp     al,'.'                  ; if period NOT found,
                je      LoadMod                 ; then add a .MOD extension.
                test    al,al
                jne     ScanPeriod

SetExt:         mov     Byte Ptr es:[si+0],'.'
                mov     Byte Ptr es:[si+1],'M'
                mov     Byte Ptr es:[si+2],'O'
                mov     Byte Ptr es:[si+3],'D'
                mov     Byte Ptr es:[si+4],0

LoadMod:        call    LoadModule Pascal,es,di ; Load the MODule...

                cmp     [ErrorInfo],0           ; any error loading?
                je      PlayNow

                mov     ax,0900h                ; yes, print error and Exit.
                lea     dx,[ErrorMesg]
                int     21h
                jmp     Exit

PlayNow:        mov     [MixSpeed],22000        ; Mixing at 22 kHz

;                call    StartPlaying            ; PLAY!

                 call    SbInit

ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

                mov     ax,0013h                ; Set Mode 320x200x256
                int     10h

                mov     cx,128                  ; Make a lookup table
                xor     bx,bx                   ; for fastest pixel
                mov     dx,320*(100-64)         ; addressing.
MakeOfs:        mov     [RowOfs+bx],dx
                mov     [RowOfs+bx+2],dx
                add     dx,320
                add     bx,4
                loop    MakeOfs

                mov     ax,0A000h               ; ES points to VideoMem
                mov     es,ax                   ; segment area.

                cld

; Note: Normally IRQ 0 calls the ModPlay Polling at 18.2Hz thru
;       the software interrupt 1Ch. If the IRQ 0 is disabled, then
;       the INT 1Ch MUST BE CALLED at least MixSpeed/1024 times per
;       second, or the module will sound "looped".
;       Because we need better sync with the ModPlayer to draw the scope,
;       the polling is called from my routine, and then the irq 0 must be
;       disabled. The [DmaBuffer] points to the current buffer of 8-bit
;       samples played by the Sound Blaster. Note that some samples are
;       discarded in the next code, just for fun!

                ;in      al,21h                  ; disable irq 0!
                ;or      al,00000001b
                ;out     21h,al

ScopeLoop:      ;int     1Ch                     ; ModPlay Polling!
                call     SbPoll

                mov     si,[DmaBuffer]         ; get current samples
                xor     cx,cx                   ; to be drawed ...
                xor     dx,dx

DrawLoop:       mov     bx,dx                   ; (save Index)
                mov     di,[Scope+bx]           ; get old SCOPE pixel address
                mov     Byte Ptr es:[di],0      ; erase it!
                lodsb                           ; get a sample (8-bit)
                mov     bl,al                   ; calc new pixel address...
                xor     bh,bh
                shl     bx,1
                mov     di,[RowOfs+bx]
                add     di,cx
                mov     bx,dx                   ; (restore Index)
                mov     [Scope+bx],di           ; save new address...
                mov     Byte Ptr es:[di],10     ; and DRAW.
                add     dx,2                    ; the next pixel...
                inc     cx
                cmp     cx,320                  ; 320 pixels drawed?
                jb      DrawLoop

CheckAnyKey:    mov     ah,1                    ; any key pressed?
                int     16h                     ; no, Loop.
                je      ScopeLoop

                mov     ah,0                    ; flush key buffer...
                int     16h

                ;in      al,21h                  ; enable irq 0!
                ;and     al,11111110b
                ;out     21h,al

                mov     ax,0003h                ; Set Text Mode 80x25x16
                int     10h

ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ

                ;call    StopPlaying             ; STOP!
                call     SbDone

Exit:           call    FreeModule              ; Free MODule core.

                mov     ax,4C00h                ; Bye!
                int     21h

Main            EndP



DetectSb        Proc    Near

                pusha
                push    es

ScanPort:       mov     bx,210h                 ; start scanning ports
                                                ; 210h, 220h, .. 260h

ResetDSP:       mov     dx,bx                   ; try to reset the DSP.
                add     dx,06h
                mov     al,1
                out     dx,al
                in      al,dx
                in      al,dx
                in      al,dx
                in      al,dx
                xor     al,al
                out     dx,al
                add     dx,08h
                mov     cx,100
WaitID:         in      al,dx
                or      al,al
                js      GetID
                loop    WaitID
                jmp     NextPort
GetID:          sub     dx,04h
                in      al,dx
                cmp     al,0AAh
                je      Found
                add     dx,04h
                loop    WaitID

NextPort:       add     bx,10h                  ; if not response,
                cmp     bx,260h                 ; try the next port.
                jbe     ResetDSP
                jmp     Fail

Found:          mov     [SbAddr],bx             ; SB Port Address Found!

ScanIRQ:        cli

                in      al,21h                  ; save the IMR.
                mov     bl,al

                mov     al,11111111b            ; disable all the IRQs.
                out     21h,al

                xor     ax,ax                   ; trap the IRQs 2,3,5,7.
                mov     es,ax

SaveIrqs:       mov     ax,es:[28h]             ; irq2
                mov     dx,es:[2Ah]
                push    ax
                push    dx

                mov     ax,es:[2Ch]             ; irq3
                mov     dx,es:[2Eh]
                push    ax
                push    dx

                mov     ax,es:[34h]             ; irq5
                mov     dx,es:[36h]
                push    ax
                push    dx

                mov     ax,es:[3Ch]             ; irq7
                mov     dx,es:[3Eh]
                push    ax
                push    dx

SetIrqs:        mov     ax,offset TrapIrq2      ; irq2
                mov     es:[28h],ax
                mov     es:[2Ah],cs

                mov     ax,offset TrapIrq3      ; irq3
                mov     es:[2Ch],ax
                mov     es:[2Eh],cs

                mov     ax,offset TrapIrq5      ; irq5
                mov     es:[34h],ax
                mov     es:[36h],cs

                mov     ax,offset TrapIrq7      ; irq7
                mov     es:[3Ch],ax
                mov     es:[3Eh],cs

EnableIrqs:     mov     al,bl                   ; enable IRQs 2,3,5,7.
                and     al,01010011b
                out     21h,al

                sti

                mov     [SbIrq],0               ; clear the IRQ level.

                mov     dx,[SbAddr]             ; tells to the SB to
                add     dx,0Ch                  ; generate a IRQ!
WaitSb:         in      al,dx
                or      al,al
                js      WaitSb
                mov     al,0F2h
                out     dx,al

                xor     cx,cx                   ; wait until IRQ level
WaitIRQ:        cmp     [SbIrq],0               ; is changed or timeout.
                jne     IrqOk
                loop    WaitIRQ

IrqOk:          mov     al,bl                   ; restore IMR.
                out     21h,al

RestoreIrqs:    cli                             ; restore IRQ vectors.

                xor     ax,ax
                mov     es,ax

                pop     dx                      ; irq7
                pop     ax
                mov     es:[3Ch],ax
                mov     es:[3Eh],dx

                pop     dx                      ; irq5
                pop     ax
                mov     es:[34h],ax
                mov     es:[36h],dx

                pop     dx                      ; irq3
                pop     ax
                mov     es:[2Ch],ax
                mov     es:[2Eh],dx

                pop     dx                      ; irq2
                pop     ax
                mov     es:[28h],ax
                mov     es:[2Ah],dx

                cli

                cmp     [SbIrq],0               ; IRQ level was changed?
                je      Fail                    ; no, fail.

Success:        mov     dx,[SbAddr]             ; Print Sucessful message.
                mov     cx,[SbIrq]
                shr     dl,4
                add     dl,'0'
                mov     [PortText],dl
                add     cl,'0'
                mov     [IrqText],cl
                mov     ah,9
                mov     dx,offset MsgFound
                int     21h
                pop     es
                popa                            ; Return to caller.
                ret

Fail:           mov     ah,9                    ; Print Failed Message,
                mov     dx,offset MsgNotFound   ; and exit to DOS.
                int     21h
                mov     ax,4C00h
                int     21h

DetectSb        EndP


TrapIrq         Proc    Far

                push    dx                      ; General IRQ trapper
                push    ds                      ; used for IRQ autodetect.
                mov     dx,@Data
                mov     ds,dx
                mov     [SbIrq],ax              ; save IRQ level.
                mov     dx,[SbAddr]
                add     dx,0Eh
                in      al,dx                   ; SB acknowledge.
                mov     al,20h
                out     20h,al                  ; Hardware acknowledge.
                pop     ds
                pop     dx
                pop     ax
                iret                            ; bye!

                IRP Level,<2,3,5,7>
TrapIrq&Level:  push    ax
                mov     ax,Level
                jmp     TrapIrq
                EndM

TrapIrq         EndP

                End     Main
