CC = gcc

CFLAGS	= -nostdinc -I../../../include
CRT0	= ../../../lib/crt0.o
LDFLAGS = -nostdlib -L../../../lib -lc -Ttext 0x800000

OBJS	= main.o cmds.o

all : 	sh install

sh:     $(OBJS)
	$(CC) -o $@ $(CRT0) $(OBJS) $(LDFLAGS)
	del $@.exe

install:
	strip sh
	copy sh ..\..\..\bin
clean:	
	del *.o 
