# TINOS kernel makefile
# Copyright (c) 1995, 1996 Bart Sekura
#
# $Id: makefile 1.1 1996/11/26 15:44:39 bart Exp bart $
#
#

include ../../include/makrules

CC = gcc

DEFS=

CFLAGS	= $(DEFS) -O2 -nostdinc -I../../include
LDFLAGS = -nostdlib -Ttext 0x20000 -Tdata 0x0

%.d : %.o
	objdump -d -h $< > .\dump\$@

%.c : RCS/%.c
	co $<

%.S : RCS/%.s
	co $<

OBJS	= desc.o start.o main.o cons.o pic.o switch.o machtrap.o assert.o \
	  idt.o trap.o isr.o misc.o tss.o kb.o wd.o syscall.o qio.o \
	  thread.o paging.o vmm.o bcopy.o string.o vsprintf.o fd.o \
	  ctype.o msg.o proc.o syscon.o pmm.o heap.o dosfs.o buf.o fsm.o \

DUMPS 	= desc.d start.d main.d cons.d pic.d switch.d machtrap.d assert.d \
	  gate.d idt.d trap.d isr.d misc.d tss.d kb.d wd.d syscall.d qio.d \
	  thread.d ctype.d string.d bcopy.d paging.d fat.d vmm.d \
	  mem.d msg.d proc.d syscon.d tbfs.d pmm.d heap.d pp.d		

all : kernel install

kernel: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

dump: 	$(DUMPS)

install:
	strip kernel
	copy kernel ..\..\bin

clean:	
	del *.o
	del kernel


