#
# Millennium Operating System Assembler
# Copyright 1997-1998 (c) Leprechaun Software.  All Rights Reserved.
#

CC	= g++
CFLAGS	= -Wall -DDEBUG
MAKE	= make

%.o: %.cpp
	$(CC) -c $(CFLAGS) $<

MOSA =	mosa.o input.o parser.o assembler.o symbols.o \
	list.o tokenizer.o types.o outform/outform.o

all: mosa

mosa: $(MOSA) 
	$(CC) -o mosa $(MOSA)

clean:
	rm -f *.o

outform:
	$(MAKE) -C outform/

mosa.o:		mosa.cpp mosa.h
input.o:	input.cpp input.h
parser.o:	parser.cpp parser.h assembler.h tokenizer.h symbols.h types.h
assembler.o:	assembler.cpp assembler.h
symbols.o:	symbols.cpp symbols.h
list.o:		list.cpp list.h
tokenizer.o:	tokenizer.cpp tokenizer.h input.h assembler.h
symbols.o:	symbols.cpp symbols.h
types.o:	types.cpp types.h
