;==========================================================================
;
;               Sound Blaster DMA Driver v0.1b by Carlos Hasan.
;
;               July 24th, 1993.
;
;==========================================================================

                Jumps
                .Model  Small
                .286


;--------------------------------------------------------------------------
;               Publics.
;--------------------------------------------------------------------------

                Global  SbInit:Proc, SbDone:Proc, SbPoll:Proc
                Global  SbAddr:Word, SbIrq:Word, DmaBuffer:Word
                Global  StartPlaying:Proc, StopPlaying:Proc
                Global  GetSamples:Proc, MixSpeed:Word


;--------------------------------------------------------------------------
;               Equates.
;--------------------------------------------------------------------------

DmaBufSize      equ     2048


;--------------------------------------------------------------------------
;               Resources and Data.
;--------------------------------------------------------------------------

                .Data

SbAddr          dw      220h
SbIrq           dw      7

DmaFlag         dw      ?
DmaBuffer       dw      ?
DmaHandler      dd      ?
TimerHandler    dd      ?

DoubleBuffer    db      2*DmaBufSize dup (?)



;--------------------------------------------------------------------------
;               Start of Code.
;--------------------------------------------------------------------------

                .Code


;--------------------------------------------------------------------------
; SbOut:  Send a Byte to the Sound Blaster.
;  In:
;   DX -  Sound Blaster Write Port.
;--------------------------------------------------------------------------

SbOut           Macro   Data
                Local   Wait
Wait:           in      al,dx
                or      al,al
                js      Wait
                mov     al,Data
                out     dx,al
                EndM



;--------------------------------------------------------------------------
; SbIrqHandler:  Sound Blaster IRQ handler.
;--------------------------------------------------------------------------

SbIrqHandler    Proc

                push    ax
                push    dx
                push    ds

                mov     ax,@Data
                mov     ds,ax

                mov     dx,[SbAddr]
                add     dx,0Eh
                in      al,dx

                sub     dx,02h
                SbOut   14h
                SbOut   0FFh
                SbOut   0FFh

                mov     al,20h
                out     20h,al

                pop     ds
                pop     dx
                pop     ax
                iret

SbIrqHandler    EndP



;--------------------------------------------------------------------------
; SbPoll:  Sound Blaster Polling.
;--------------------------------------------------------------------------

SbPoll          Proc

                pusha
                push    ds
                push    es

                mov     ax,@Data
                mov     ds,ax

GetDmaCount:    in      al,03h
                mov     cl,al
                in      al,03h
                mov     ch,al

                mov     ax,[DmaFlag]
                test    ax,ax
                jne     SecondHalf

FirstHalf:      cmp     cx,DmaBufSize/2
                jae     Bye
                mov     si,[DmaBuffer]
                mov     cx,DmaBufSize/2
                call    GetSamples Pascal,ds,si,cx
                inc     [DmaFlag]
                jmp     Bye

SecondHalf:     cmp     cx,DmaBufSize/2
                jb      Bye
                mov     si,[DmaBuffer]
                mov     cx,DmaBufSize/2
                add     si,cx
                call    GetSamples Pascal,ds,si,cx
                dec     [DmaFlag]

Bye:            pop     es
                pop     ds
                popa
                ret     ;<==== IRET

SbPoll          EndP



;--------------------------------------------------------------------------
; SbInit: Initializes the Sound BBlaster Driver.
;--------------------------------------------------------------------------

SbInit          Proc

                pusha
                push    ds
                push    es

                call    StartPlaying

                cli

                in      al,21h
                push    ax

                mov     al,11111111b
                out     21h,al

SetBuffer:      mov     [DmaFlag],0
                mov     ax,offset DoubleBuffer
                mov     [DmaBuffer],ax
                mov     dx,ds
                mov     bx,dx
                shr     dh,4
                shl     bx,4
                add     ax,bx
                adc     dh,0
                mov     cx,ax
                neg     cx
                cmp     cx,DmaBufSize
                jae     SetDma
                add     [DmaBuffer],cx
                add     ax,cx
                adc     dh,0

SetDma:         mov     bx,ax
                mov     cx,DmaBufSize
                dec     cx

                mov     al,05h
                out     0Ah,al

                xor     al,al
                out     0Ch,al

                mov     al,bl
                out     02h,al
                mov     al,bh
                out     02h,al
                mov     al,dh
                out     83h,al

                mov     al,cl
                out     03h,al
                mov     al,ch
                out     03h,al

                mov     al,59h
                out     0Bh,al

                mov     al,01h
                out     0Ah,al

ClearBuffer:    mov     di,[DmaBuffer]
                mov     cx,DmaBufSize
                mov     ax,ds
                mov     es,ax
                mov     al,80h
                cld
                rep     stosb

SetIrq:         xor     ax,ax
                mov     es,ax

                mov     bx,[SbIrq]
                add     bx,08h
                shl     bx,2

                mov     ax,es:[bx]
                mov     dx,es:[bx+2]
                mov     Word Ptr [DmaHandler],ax
                mov     Word Ptr [DmaHandler+2],dx

                mov     ax,offset SbIrqHandler
                mov     es:[bx],ax
                mov     es:[bx+2],cs

SetTimer:       mov     ax,es:[70h]
                mov     dx,es:[72h]
                mov     Word Ptr [TimerHandler],ax
                mov     Word Ptr [TimerHandler+2],dx

                comment &
                mov     ax,offset SbPoll
                mov     es:[70h],ax
                mov     es:[72h],cs
                &

ResetDsp:       mov     dx,[SbAddr]
                add     dx,06h
                mov     al,1
                out     dx,al
                in      al,dx
                in      al,dx
                in      al,dx
                in      al,dx
                xor     al,al
                out     dx,al
                mov     cx,100
WaitId:         mov     dx,[SbAddr]
                add     dx,0Eh
                in      al,dx
                or      al,al
                js      GetID
                loop    WaitId
                jmp     Exit
GetId:          mov     dx,[SbAddr]
                add     dx,0Ah
                in      al,dx
                cmp     al,0AAh
                je      SbOk
                loop    WaitId
                jmp     Exit

SbOk:           mov     dx,[SbAddr]
                add     dx,0Ch
                SbOut   0D1h
                mov     ax,1000
                mul     ax
                div     [MixSpeed]
                neg     al
                mov     ah,al
                mov     dx,[SbAddr]
                add     dx,0Ch
                SbOut   40h
                SbOut   ah

StartDma:       SbOut   14h
                SbOut   0FFh
                Sbout   0FFh

Exit:           pop     ax
                mov     cx,[SbIrq]
                mov     ah,1
                shl     ah,cl
                not     ah
                and     al,ah
                out     21h,al

                sti

                pop     es
                pop     ds
                popa
                ret

SbInit          EndP



;--------------------------------------------------------------------------
; SbDone:  Shut Down the Sound Blaster Driver.
;--------------------------------------------------------------------------

SbDone          Proc

                pusha
                push    ds
                push    es

                cli

                in      al,21h
                push    ax

                mov     al,11111111b
                out     21h,al

                xor     ax,ax
                mov     es,ax

                mov     bx,[SbIrq]
                add     bx,08h
                shl     bx,2

                mov     ax,Word Ptr [DmaHandler]
                mov     dx,Word ptr [DmaHandler+2]
                mov     es:[bx],ax
                mov     es:[bx+2],dx

                mov     ax,Word Ptr [TimerHandler]
                mov     dx,Word ptr [TimerHandler+2]
                mov     es:[70h],ax
                mov     es:[72h],dx

                mov     dx,[SbAddr]
                add     dx,0Ch
                SbOut   0D0h
                SbOut   0D3h

                pop     ax
                mov     cx,[SbIrq]
                mov     ah,1
                shl     ah,cl
                or      al,ah
                out     21h,al

                sti

                call    StopPlaying

                pop     es
                pop     ds
                popa
                ret

SbDone          EndP

                End
