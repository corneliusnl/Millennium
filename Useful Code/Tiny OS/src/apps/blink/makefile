CC = gcc

CRT0	= ../../../lib/crt0.o
CFLAGS	= -I../../../include
LDFLAGS = -nostdlib -L../../../lib -lc -Ttext 0x800000

OBJS	= blink.o

all : 	blink

blink:  $(OBJS)
	$(CC) -o $@ $(CRT0) $(OBJS) $(LDFLAGS)
	del blink.exe

blink.o:blink.c
	$(CC) $(CFLAGS) -c blink.c

clean:	
	del *.o 
