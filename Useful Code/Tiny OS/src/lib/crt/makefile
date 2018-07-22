include ../../../include/makrules

CC = gcc
CFLAGS = -O2 -nostdinc -I../../../include

all : crt0.o install

crt0.o: crt0.S
	$(CC) $(CFLAGS) -c $< -o $@

install:
	copy crt0.o ..\..\..\lib

clean:	
	del *.o
	
