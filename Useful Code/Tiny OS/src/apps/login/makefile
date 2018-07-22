CC = gcc

CFLAGS	= -I../../../include
CRT0	= ../../../lib/crt0.o
LDFLAGS = -nostdlib -L../../../lib -lc -Ttext 0x800000

OBJS	= login.o

all : 	login install

login:  $(OBJS)
	$(CC) -o $@ $(CRT0) $(OBJS) $(LDFLAGS)
	del $@.exe

login.o:login.c
	$(CC) $(CFLAGS) -c $<

clean:	
	del *.o 
	del login

install:
	strip login
	copy login ..\..\..\bin

