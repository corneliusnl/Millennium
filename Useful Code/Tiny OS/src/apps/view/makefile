CC = gcc

CFLAGS	= -I../../../include
LDFLAGS = -nostdlib -L../../../lib -lc -Ttext 0x800000
CRT0 	= ../../../lib/crt0.o

OBJS	= view.o

all : 	view

view:   $(OBJS)
	$(CC) -o $@ $(CRT0) $(OBJS) $(LDFLAGS)
	del $@.exe

install:
	strip view
	copy view ..\..\..\bin

clean:	
	del *.o 
