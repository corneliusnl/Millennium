CC = gcc

CFLAGS	= -I../../../include
LDFLAGS = -nostdlib -L../../../lib -lc -Ttext 0x800000
CRT0 	= ../../../lib/crt0.o

OBJS	= init.o

all : 	init

init:   $(OBJS)
	$(CC) -o $@ $(CRT0) $(OBJS) $(LDFLAGS)
	del $@.exe

install:
	strip init
	copy init ..\..\..\bin

clean:	
	del *.o 
