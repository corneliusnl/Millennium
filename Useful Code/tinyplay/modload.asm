                Comment &

                Amiga Module Loader v0.1b by Carlos Hasan.

                July 10th, 1993.

                &

                Jumps
                .Model  Small
                .286


;               Publics.

                Global  LoadModule: Proc, FreeModule: Proc, ErrorInfo: Word
                Global  StartPlaying: Proc, StopPlaying: Proc
                Global  ModInfo: ModInfoRec


;               Structures.

ModSample       Struc

msName          db      22 dup (?)
msLength        dw      ?
msFinetune      db      ?
msVolume        db      ?
msRepeat        dw      ?
msRepLen        dw      ?

ModSample       EndS


ModHeader       Struc

mhName          db      20 dup (?)
mhSamples       ModSample 31 dup (?)
mhOrderLen      db      ?
mhReStart       db      ?
mhOrder         db      128 dup (?)
mhSign          dw      ?,?

ModHeader       EndS


ModInfoRec      Struc

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

ModInfoRec      EndS


;               Resources and Data.

                .Data

FileHandle      dw      ?
ErrorInfo       dw      ?
Header          ModHeader ?


;               Code.

                .Code

LoadModule      Proc
                Arg     FileName: DWord = FrameSize

                push    bp
                mov     bp,sp
                pusha
                push    ds
                push    es

                mov     ax,@Data
                mov     ds,ax

                mov     [ErrorInfo],1

                call    ClearModInfo

OpenFile:       push    ds
                mov     ax,3D00h
                lds     dx,[FileName]
                int     21h
                pop     ds
                jc      Failed
                mov     [FileHandle],ax

ReadHeader:     mov     ax,3F00h
                mov     bx,[FileHandle]
                mov     cx,Size ModHeader
                lea     dx,[Header]
                int     21h
                jc      CloseFile

CheckMK:        cmp     [Header.mhSign],'.M'
                jne     CheckFLT4
                cmp     [Header.mhSign+2],'.K'
                je      IsModFile

CheckFLT4:      cmp     [Header.mhSign],'LF'
                jne     Is15Inst
                cmp     [Header.mhSign+2],'4T'
                je      IsModFile

Is15Inst:       mov     si,(Offset Header.mhSamples) + 15*(Size ModSample)
                mov     di,(Offset Header.mhOrderLen)
                mov     ax,ds
                mov     es,ax
                cld
                mov     cx,130
                rep     movsb
                mov     di,(Offset Header.mhSamples) + 15*(Size ModSample)
                xor     ax,ax
                mov     cx, 16*(Size ModSample)
                rep     stosb

SeekPatterns:   mov     ax,4200h
                mov     bx,[FileHandle]
                mov     cx,0
                mov     dx,600
                int     21h

IsModFile:      mov     al,[Header.mhOrderLen]
                mov     [ModInfo.OrderLen],al

                mov     al,[Header.mhReStart]
                cmp     al,[Header.mhOrderLen]
                jb      SetReStart
                mov     al,7Fh
SetReStart:     mov     [ModInfo.ReStart],al

                mov     cx,128
                xor     ax,ax
                xor     bx,bx
CopyOrder:      mov     ah,[Header.mhOrder+bx]
                mov     [ModInfo.Order+bx],ah
                cmp     ah,al
                jb      NextOrder
                mov     al,ah
NextOrder:      inc     bx
                loop    CopyOrder

AllocPatterns:  xor     bx,bx
                mov     bl,al
                inc     bx
                push    bx
                shl     bx,6
                mov     ax,4800h
                int     21h
                pop     bx
                jc      CloseFile
                mov     Word Ptr [ModInfo.Patterns],0
                mov     Word Ptr [ModInfo.Patterns+2],ax

ReadPatterns:   push    ds
                mov     ax,3F00h
                mov     cx,bx
                shl     cx,10
                mov     bx,[FileHandle]
                lds     dx,[ModInfo.Patterns]
                int     21h
                pop     ds
                jc      CloseFile

                lea     si,[Header.mhSamples]
                xor     di,di
CopySamples:    mov     ax,[si+msLength]
                xchg    al,ah
                shl     ax,1
                mov     [ModInfo.SampLen+di],ax
                mov     al,[si+msVolume]
                xor     ah,ah
                mov     [ModInfo.SampVol+di],ax
                mov     ax,[si+msRepeat]
                xchg    al,ah
                shl     ax,1
                mov     [ModInfo.SampRep+di],ax
                mov     ax,[si+msRepLen]
                xchg    al,ah
                shl     ax,1
                mov     [ModInfo.SampRepLen+di],ax
                add     si,Size ModSample
                add     di,2
                cmp     di,2*31
                jb      CopySamples

                xor     si,si
AllocSamples:   mov     bx,[ModInfo.SampLen+si]
                shr     bx,4
                je      NextSample
                push    si
                inc     bx
                mov     ax,4800h
                int     21h
                pop     si
                jc      CloseFile
                mov     [ModInfo.SampOfs+si],0
                mov     [ModInfo.SampSeg+si],ax
ReadSample:     push    si
                push    ds
                mov     ax,3F00h
                mov     bx,[FileHandle]
                mov     cx,[ModInfo.SampLen+si]
                mov     dx,[ModInfo.SampOfs+si]
                mov     ds,[ModInfo.SampSeg+si]
                int     21h
                pop     ds
                pop     si
                jc      CloseFile
NextSample:     add     si,2
                cmp     si,2*31
                jb      AllocSamples

                mov     [ErrorInfo],0

CloseFile:      mov     ax,3E00h
                mov     bx,[FileHandle]
                int     21h

Failed:         pop     es
                pop     ds
                popa
                pop     bp
                ret

LoadModule      EndP


FreeModule      Proc

                pusha
                push    ds
                push    es

                mov     ax,@Data
                mov     ds,ax

FreePatterns:   mov     ax,4900h
                mov     bx,Word Ptr [ModInfo.Patterns+2]
                test    bx,bx
                je      FreeSamples
                mov     es,bx
                int     21h

FreeSamples:    xor     si,si
FreeLoop:       mov     ax,[ModInfo.SampLen+si]
                mov     bx,[ModInfo.SampSeg+si]
                test    ax,ax
                je      FreeNext
                test    bx,bx
                je      FreeNext
                push    si
                mov     ax,4900h
                mov     es,bx
                int     21h
                pop     si
FreeNext:       add     si,2
                cmp     si,2*31
                jb      FreeLoop

                call    ClearModInfo

                pop     es
                pop     ds
                popa
                ret

FreeModule      EndP


ClearModInfo    Proc    Near

                pusha
                push    es
                mov     ax,ds
                mov     es,ax
                lea     di,[ModInfo]
                mov     cx,Size ModInfoRec
                cld
                xor     ax,ax
                rep     stosb
                pop     es
                popa
                ret

ClearModInfo    EndP

                End
