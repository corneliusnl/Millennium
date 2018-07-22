                Comment &

                Amiga Module Player v0.3b by Carlos Hasan.

                July 23th, 1993.

                &

                Jumps
                .Model  Small
                .286


;--------------------------------------------------------------------------
;               Publics.
;--------------------------------------------------------------------------

                Global  StartPlaying:Proc, StopPlaying:Proc
                Global  GetSamples:Proc, MixSpeed:Word, ModInfo:Byte


;--------------------------------------------------------------------------
;               Equates.
;--------------------------------------------------------------------------

NumTracks       equ     4
DefTempo        equ     6
DefBpm          equ     125
MidCRate        equ     8448
MixBufSize      equ     4096


;--------------------------------------------------------------------------
;               Structures.
;--------------------------------------------------------------------------

TrackInfo       Struc

Samples         dd      ?
Position        dw      ?
Len             dw      ?
Repeat          dw      ?
RepLen          dw      ?
Volume          db      ?
Error           db      ?
Period          dw      ?
Pitch           dw      ?
Effect          dw      ?
PortTo          dw      ?
PortParm        db      ?
VibPos          db      ?
VibParm         db      ?
OldSampOfs      db      ?
Arp             dw      ?,?,?
ArpIndex        dw      ?

TrackInfo       EndS


;--------------------------------------------------------------------------
;               Resources and Data.
;--------------------------------------------------------------------------

                .Data

Credits         db      'Amiga Module Player v0.3b by Carlos Hasan.'

SinTable        db      0,25,50,74,98,120,142,162,180,197,212,225
                db      236,244,250,254,255,254,250,244,236,225
                db      212,197,180,162,142,120,98,74,50,25

PeriodTable     dw      856,808,762,720,678,640,604,570,538,508,480,453
                dw      428,404,381,360,339,320,302,285,269,254,240,226
                dw      214,202,190,180,170,160,151,143,135,127,120,113

MixSpeed        dw      ?

ModInfo         Label   Byte

OrderLen        db      ?
ReStart         db      ?
Order           db      128 dup (?)
Patterns        dd      ?

SampOfs         dw      31 dup (?)
SampSeg         dw      31 dup (?)
SampLen         dw      31 dup (?)
SampRep         dw      31 dup (?)
SampRepLen      dw      31 dup (?)
SampVol         dw      31 dup (?)

OrderPos        db      ?
Tempo           db      ?
TempoWait       db      ?
Bpm             db      ?
Row             db      ?
BreakRow        db      ?
BpmSamples      dw      ?
BufPtr          dw      ?
BufLen          dw      ?
BufRep          dw      ?
Note            dd      ?
Tracks          TrackInfo NumTracks dup (?)

PitchTable      dw      857 dup (?)
VolTable        db      16640 dup (?)
MixBuffer       db      MixBufSize dup (?)


;--------------------------------------------------------------------------
;               Start of Code.
;--------------------------------------------------------------------------

                .Code


;--------------------------------------------------------------------------
; BeatTrack:  Process the next beat in one track.
;  In:
;    ds:di -  Track info Address.
;--------------------------------------------------------------------------

BeatTrack       Proc    Near

                mov     dx,[di+Effect]
                test    dx,dx
                je      None
                cmp     dh,00h
                je      Arpeggio
                cmp     dh,01h
                je      PortUp
                cmp     dh,02h
                je      PortDown
                cmp     dh,03h
                je      TonePort
                cmp     dh,04h
                je      Vibrato
                cmp     dh,05h
                je      PortSlide
                cmp     dh,06h
                je      VibSlide
                cmp     dh,0Ah
                je      VolSlide
None:           ret


Arpeggio:       mov     bx,[di+ArpIndex]
                mov     ax,[di+Arp+bx]
                mov     [di+Pitch],ax
                add     bx,2
                cmp     bx,6
                jb      SetArpIndex
                xor     bx,bx
SetArpIndex:    mov     [di+ArpIndex],bx
                ret


PortUp:         xor     dh,dh
                mov     bx,[di+Period]
                sub     bx,dx
                cmp     bx,113
                jge     NotSmall
                mov     bx,113
NotSmall:       mov     [di+Period],bx
                add     bx,bx
                mov     ax,[PitchTable+bx]
                mov     [di+Pitch],ax
                ret


PortDown:       xor     dh,dh
                mov     bx,[di+Period]
                add     bx,dx
                cmp     bx,856
                jle     NotBig
                mov     bx,856
NotBig:         mov     [di+Period],bx
                add     bx,bx
                mov     ax,[PitchTable+bx]
                mov     [di+Pitch],ax
                ret


TonePort:       xor     dh,dh
                mov     ax,[di+PortTo]
                mov     bx,[di+Period]
                cmp     bx,ax
                je      NoPort
                jg      PortToUp
PortToDown:     add     bx,dx
                cmp     bx,ax
                jle     SetPort
FixPort:        mov     bx,ax
                jmp     SetPort
PortToUp:       sub     bx,dx
                cmp     bx,ax
                jl      FixPort
SetPort:        mov     [di+Period],bx
                add     bx,bx
                mov     ax,[PitchTable+bx]
                mov     [di+Pitch],ax
NoPort:         ret


Vibrato:        mov     dh,dl
                and     dl,0Fh
                shr     dh,4
                shl     dh,2
                add     [di+VibPos],dh
                mov     dh,[di+VibPos]
                mov     bl,dh
                shr     bl,2
                and     bx,1Fh
                mov     al,[SinTable+bx]
                mul     dl
                rol     ax,1
                xchg    al,ah
                and     ah,1
                test    dh,dh
                jns     VibUp
                neg     ax
VibUp:          add     ax,[di+Period]
                mov     bx,ax
                cmp     bx,113
                jge     NoLoVib
                mov     bx,113
NoLoVib:        cmp     bx,856
                jle     NoHiVib
                mov     bx,856
NoHiVib:        add     bx,bx
                mov     ax,[PitchTable+bx]
                mov     [di+Pitch],ax
                ret


PortSlide:      call    VolSlide
                mov     dl,[di+PortParm]
                jmp     TonePort


VibSlide:       call    VolSlide
                mov     dl,[di+VibParm]
                jmp     Vibrato


VolSlide:       mov     dh,dl
                and     dl,0Fh
                shr     dh,4
                mov     al,[di+Volume]
                sub     al,dl
                jge     NoLoVol
                xor     al,al
NoLoVol:        add     al,dh
                cmp     al,64
                jbe     NoHiVol
                mov     al,64
NoHiVol:        mov     [di+Volume],al
                ret

BeatTrack       EndP




;--------------------------------------------------------------------------
; GetTrack:   Get the next Note from a pattern.
;  In:
;    ds:di -  Track info Address.
;    es:si -  Pattern Note Address.
; Out:
;    es:si -  The Next Pattern Note address.
;--------------------------------------------------------------------------

GetTrack        Proc    Near

                seges   lodsw
                xchg    al,ah
                mov     bl,ah
                and     ah,0Fh
                mov     cx,ax
                seges   lodsw
                xchg    al,ah
                mov     bh,ah
                and     ah,0Fh
                mov     dx,ax
                mov     [di+Effect],dx
                and     bl,0F0h
                shr     bh,4
                or      bl,bh
                je      SetPeriod

SetSample:      xor     bh,bh
                dec     bx
                add     bx,bx
                mov     ax,[SampVol+bx]
                mov     [di+Volume],al
                mov     ax,[SampOfs+bx]
                mov     Word Ptr [di+Samples],ax
                mov     ax,[SampSeg+bx]
                mov     Word Ptr [di+Samples+2],ax
                mov     ax,[SampLen+bx]
                mov     [di+Len],ax
                mov     ax,[SampRep+bx]
                mov     [di+Repeat],ax
                mov     ax,[SampRepLen+bx]
                mov     [di+RepLen],ax

SetPeriod:      test    cx,cx
                je      SetEffect

                mov     [di+PortTo],cx
                cmp     dh,03h
                je      SetEffect

                mov     [di+Period],cx
                mov     bx,cx
                add     bx,bx
                mov     ax,[PitchTable+bx]
                mov     [di+Pitch],ax
                mov     [di+Position],0

SetEffect:      test    dx,dx
                je      InitNone
                cmp     dh,00h
                je      InitArpeggio
                cmp     dh,03h
                je      InitTonePort
                cmp     dh,04h
                je      InitVibrato
                cmp     dh,09h
                je      SampleOfs
                cmp     dh,0Bh
                je      PosJump
                cmp     dh,0Ch
                je      SetVolume
                cmp     dh,0Dh
                je      Break
                cmp     dh,0Fh
                je      SetSpeed
InitNone:       ret


InitTonePort:   test    dl,dl
                jne     SetPortParm
                mov     dl,[di+PortParm]
SetPortParm:    mov     [di+PortParm],dl
                mov     [di+Effect],dx
                ret


InitVibrato:    mov     al,[di+VibParm]
                mov     ah,al
                and     al,0Fh
                and     ah,0F0h
                test    dl,0Fh
                jne     OkDepth
                or      dl,al
OkDepth:        test    dl,0F0h
                jne     OkRate
                or      dl,ah
OkRate:         mov     [di+VibParm],dl
                mov     [di+Effect],dx
                test    cx,cx
                je      OkPos
                mov     [di+VibPos],0
OkPos:          ret


SampleOfs:      test    dl,dl
                jne     SetSampleOfs
                mov     dl,[di+OldSampOfs]
SetSampleOfs:   mov     [di+OldSampOfs],dl
                mov     dh,dl
                xor     dl,dl
                mov     [di+Position],dx
                ret


PosJump:        mov     [OrderPos],dl
                mov     [Row],64
                ret


SetVolume:      cmp     dl,64
                jbe     OkVol
                mov     dl,64
OkVol:          mov     [di+Volume],dl
                ret


Break:          mov     dh,dl
                and     dl,0Fh
                shr     dh,4
                add     dh,dh
                add     dl,dh
                shl     dh,2
                add     dl,dh
                mov     [BreakRow],dl
                mov     [Row],64
                ret


SetSpeed:       test    dl,dl
                je      Skip
                cmp     dl,31
                ja      SetBpm
SetTempo:       mov     [Tempo],dl
                mov     [TempoWait],dl
                ret
SetBpm:         mov     [Bpm],dl
                mov     al,103
                mul     dl
                mov     bl,ah
                xor     bh,bh
                mov     ax,[MixSpeed]
                xor     dx,dx
                div     bx
                mov     [BpmSamples],ax
Skip:           ret


InitArpeggio:   mov     dh,dl
                and     dl,0Fh
                shr     dh,4
                mov     cx,36
                xor     bx,bx
                mov     ax,[di+Period]
ScanPeriod:     cmp     ax,[PeriodTable+bx]
                jae     SetArp
                add     bx,2
                loop    ScanPeriod
SetArp:         add     dx,dx
                add     dh,bl
                add     dl,bl
                mov     bx,[PeriodTable+bx]
                add     bx,bx
                mov     ax,[PitchTable+bx]
                mov     [di+Arp],ax
                mov     bl,dh
                xor     bh,bh
                mov     bx,[PeriodTable+bx]
                add     bx,bx
                mov     ax,[PitchTable+bx]
                mov     [di+Arp+2],ax
                mov     bl,dl
                xor     bh,bh
                mov     bx,[PeriodTable+bx]
                add     bx,bx
                mov     ax,[PitchTable+bx]
                mov     [di+Arp+4],ax
                mov     [di+ArpIndex],0
                ret

GetTrack        EndP




;--------------------------------------------------------------------------
; UpdateTracks:  Main code to process the next tick to be played.
;--------------------------------------------------------------------------

UpdateTracks    Proc    Near

                dec     [TempoWait]
                je      GetTracks

BeatTracks:     Ofs = offset Tracks
                Rept NumTracks
                  mov   di,Ofs
                  call  BeatTrack
                  Ofs = Ofs + Size TrackInfo
                EndM

                ret

GetTracks:      mov     al,[Tempo]
                mov     [TempoWait],al

                les     si,[Note]
                cmp     [Row],64
                jb      NoPattWrap

                les     si,[Patterns]
                mov     bl,[OrderPos]
                cmp     bl,[OrderLen]
                jb      NoOrderWrap
                mov     bl,[ReStart]
                mov     [OrderPos],bl
                cmp     bl,[OrderLen]
                jae     NoUpdate
NoOrderWrap:    xor     bh,bh
                mov     bl,[Order+bx]
                shl     bx,10
                add     si,bx
                mov     bl,[BreakRow]
                mov     [Row],bl
                xor     bh,bh
                mov     [BreakRow],bh
                shl     bx,4
                add     si,bx
                mov     Word Ptr [Note],si
                mov     Word Ptr [Note+2],es
                inc     [OrderPos]
NoPattWrap:     inc     [Row]

                cld
                Ofs = offset Tracks
                Rept NumTracks
                  mov   di,Ofs
                  call  GetTrack
                  Ofs = Ofs + Size TrackInfo
                EndM

                mov     Word Ptr [Note],si
NoUpdate:       ret

UpdateTracks    EndP




;--------------------------------------------------------------------------
; MixTrack:  Mixes one track into a CLEAN buffer.
;  In:
;   ds:si -  Track Info Address.
;   ds:di -  Buffer Address.
;    cx   -  Buffer Size.
;--------------------------------------------------------------------------

MixTrack        Proc    Near

                cmp     [si+RepLen],2
                ja      MixLooped

MixNonLooped:   les     dx,[si+Samples]
                mov     bx,[si+Position]
                mov     bp,[si+Len]
                push    dx
                push    si
                add     bx,dx
                add     bp,dx
                mov     dx,[si+Pitch]
                mov     al,[si+Volume]
                mov     ah,[si+Error]
                mov     si,bx
                mov     bh,al
                mov     al,dl
                mov     dl,dh
                xor     dh,dh
nlMixSamp:      cmp     si,bp
                jae     nlMixBye
                mov     bl,es:[si]
                mov     bl,[VolTable+bx]
                add     [di],bl
                inc     di
                add     ah,al
                adc     si,dx
                loop    nlMixSamp
nlMixBye:       mov     bx,si
                pop     si
                pop     dx
                sub     bx,dx
                mov     [si+Position],bx
                mov     [si+Error],ah
                ret

MixLooped:      les     dx,[si+Samples]
                mov     bx,[si+Position]
                mov     bp,[si+RepLen]
                mov     [BufRep],bp
                add     bp,[si+Repeat]
                push    dx
                push    si
                add     bx,dx
                add     bp,dx
                mov     dx,[si+Pitch]
                mov     al,[si+Volume]
                mov     ah,[si+Error]
                mov     si,bx
                mov     bh,al
                mov     al,dl
                mov     dl,dh
                xor     dh,dh
lpMixSamp:      cmp     si,bp
                jb      lpMixNow
                sub     si,[BufRep]
lpMixNow:       mov     bl,es:[si]
                mov     bl,[VolTable+bx]
                add     [di],bl
                inc     di
                add     ah,al
                adc     si,dx
                loop    lpMixSamp
lpMixBye:       mov     bx,si
                pop     si
                pop     dx
                sub     bx,dx
                mov     [si+Position],bx
                mov     [si+Error],ah
                ret

MixTrack        EndP




;--------------------------------------------------------------------------
; GetSamples:  Returns the next chunk of samples to be played.
;  In:
;    Buffer  - Buffer Address.
;    Count   - Buffer Size.
;--------------------------------------------------------------------------

GetSamples      Proc Pascal Buffer:DWord, Count:Word

                pusha
                push    ds
                push    es
                cld

                les     di,[Buffer]
                mov     bx,[Count]

NextChunk:      cmp     [BufLen],0
                jne     CopyChunk

                push    bx
                push    di
                push    es

MixChunk:       lea     di,[MixBuffer]
                mov     cx,[BpmSamples]
                mov     [BufPtr],di
                mov     [BufLen],cx

                mov     ax,ds
                mov     es,ax
                mov     al,80h
                rep     stosb

                Ofs = offset Tracks
                Rept NumTracks
                  mov   si,Ofs
                  mov   di,[BufPtr]
                  mov   cx,[BufLen]
                  call  MixTrack
                  Ofs = Ofs + Size TrackInfo
                EndM
                call    UpdateTracks

                pop     es
                pop     di
                pop     bx

CopyChunk:      mov     cx,[BufLen]
                cmp     cx,bx
                jbe     MoveChunk
                mov     cx,bx

MoveChunk:      mov     si,[BufPtr]
                add     [BufPtr],cx
                sub     [BufLen],cx
                sub     bx,cx
                rep     movsb
                test    bx,bx
                jne     NextChunk

                pop     es
                pop     ds
                popa
                ret

GetSamples      EndP




;--------------------------------------------------------------------------
; StartPlaying: Initializes the Sound System.
;  In:
;   Module Information Resources.
;--------------------------------------------------------------------------

StartPlaying    Proc

                pusha
                push    ds
                push    es

SetModParms:    mov     [OrderPos],0
                mov     [Tempo],DefTempo
                mov     [TempoWait],DefTempo
                mov     [Bpm],DefBpm
                mov     [Row],64
                mov     [BreakRow],0
                mov     ax,[MixSpeed]
                xor     dx,dx
                mov     bx,24*DefBpm/60
                div     bx
                mov     [BpmSamples],ax

ClearTracks:    mov     di,offset Tracks
                mov     ax,ds
                mov     es,ax
                mov     cx,NumTracks * (Size TrackInfo)
                xor     ax,ax
                cld
                rep     stosb

                mov     [BufPtr],ax
                mov     [BufLen],ax

MakePitch:      mov     ax,MidCRate
                mov     bx,428
                mul     bx
                div     [MixSpeed]
                xor     dh,dh
                mov     dl,ah
                mov     ah,al
                xor     al,al
                mov     cx,857
                xor     bx,bx
                mov     di,offset PitchTable
PitchLoop:      push    ax
                push    dx
                cmp     dx,bx
                jae     NoDiv
                div     bx
NoDiv:          stosw
                pop     dx
                pop     ax
                inc     bx
                loop    PitchLoop

MakeVolume:     mov     cx,16640
                mov     bx,cx
VolLoop:        dec     bx
                mov     al,bl
                imul    bh
                mov     [VolTable+bx],ah
                loop    VolLoop

                pop     es
                pop     ds
                popa
                ret

StartPlaying    EndP




;--------------------------------------------------------------------------
; StopPlaying: ShutDown the Sound System.
;--------------------------------------------------------------------------

StopPlaying     Proc

                ret

StopPlaying     EndP


                End
