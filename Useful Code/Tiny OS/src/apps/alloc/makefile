CC = gcc

CFLAGS	= -I../../../include
LDFLAGS = -nostdlib -L../../../lib -lc -Ttext 0x800000
CRT0 	= ../../../lib/crt0.o

OBJS	= alloc.o

all : 	alloc

alloc:  $(OBJS)
	$(CC) -o $@ $(CRT0) $(OBJS) $(LDFLAGS)
	del $@.exe

install:
	strip alloc
	copy alloc ..\..\..\bin

clean:	
	del *.o 
