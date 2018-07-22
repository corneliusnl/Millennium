CC = gcc

CFLAGS	= -I../../../include
LDFLAGS = -nostdlib -L../../../lib -lc -Ttext 0x800000
CRT0 	= ../../../lib/crt0.o

OBJS	= args.o

all : 	args

args:   $(OBJS)
	$(CC) -o $@ $(CRT0) $(OBJS) $(LDFLAGS)
	del $@.exe

install:
	strip args
	copy args ..\..\..\bin

clean:	
	del *.o 
