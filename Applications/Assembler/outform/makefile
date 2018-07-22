#
# Millennium Operating System Assembler
# Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
#

CC	= c++
CFLAGS	= -Wall -DDEBUG

%.o:%.cpp
	$(CC) $(CFLAGS) $<

OUTFORM = outform.o

outform.o:	outform.cpp outform.h
