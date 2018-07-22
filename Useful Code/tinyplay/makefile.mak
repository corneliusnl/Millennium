# makefile
# No Debugging stuff!!

play.exe : play.obj modplay.obj modload.obj sb.obj
   tlink /x play modplay modload sb

clear :
   del *.bak
   del *.obj
   del *.map

.asm.obj :
   tasm -m -q  $<