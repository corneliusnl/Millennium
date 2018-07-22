CC = gcc

CFLAGS	= -I../../../include
LDFLAGS = -nostdlib -L../../../lib -lc -Ttext 0x800000
CRT0 	= ../../../lib/crt0.o

OBJS	= cursor.o

all : 	cursor

cursor: $(OBJS)
	$(CC) -o $@ $(CRT0) $(OBJS) $(LDFLAGS)
	del $@.exe

install:
	strip cursor
	copy cursor ..\..\..\bin

clean:	
	del *.o 
