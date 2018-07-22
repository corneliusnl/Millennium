CC = gcc

CFLAGS	= -nostdinc -I../../../include
CRT0	= ../../../lib/crt0.o
LDFLAGS = -nostdlib -L../../../lib -lc -Ttext 0x800000

OBJS	= cp.o

all : 	cp install

cp:     $(OBJS)
	$(CC) -o $@ $(CRT0) $(OBJS) $(LDFLAGS)
	del $@.exe

install:
	strip cp
	copy cp ..\..\..\bin
clean:	
	del *.o 
