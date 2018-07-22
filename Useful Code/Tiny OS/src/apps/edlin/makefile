CC = gcc

CFLAGS	= -nostdinc -I../../../include
CRT0	= ../../../lib/crt0.o
LDFLAGS = -nostdlib -L../../../lib -lc -Ttext 0x800000

OBJS	= edlin.o

all : 	edlin install

edlin:  $(OBJS)
	$(CC) -o $@ $(CRT0) $(OBJS) $(LDFLAGS)
	del $@.exe

install:
	strip edlin
	copy edlin ..\..\..\bin
clean:	
	del *.o 
