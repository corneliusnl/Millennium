CC = gcc
CFLAGS	= -O2 -nostdinc -I../../../include

SUBS = 386 assert ctype stdio stdlib string tinos io

include 386/makefile
include assert/makefile
include ctype/makefile
include stdio/makefile
include stdlib/makefile
include string/makefile
include tinos/makefile
include io/makefile

all: libc.a install

libc.a : $(OBJS)
	ar rsv $@ $(OBJS)

install: libc.a
	copy libc.a ..\..\..\lib\libc.a


clean:
	cd 386
	del *.o
	cd ..\assert
	del *.o
	cd ..\ctype
	del *.o
	cd ..\stdio
	del *.o
	cd ..\stdlib
	del *.o
	cd ..\string
	del *.o
	cd ..\tinos
	del *.o
	cd ..\io
	del *.o
	cd ..

