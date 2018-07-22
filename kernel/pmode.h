//
// Global Descriptor Tables
// Copyright 1997-1998 (c) Leprechuan Software. All Rights Reserved.
//
// Authors:
//  Jeremiah Cornelious McCarthy
//
// References:
//  Intel Architecture Software Developers Manual (order number: 243192)
//

//
// Segment Descriptor
//
typedef struct
{
	unsigned short		Limit0_15;
   unsigned short		Base0_15;
   unsigned char		Base16_23;
   unsigned char		Misc1;
   unsigned char		Misc2;
   unsigned char		Base24_31;
} SEGMENTDESC;

//
// Descriptor Table
//
typedef struct
{
	unsigned short	Limit;
   unsigned long	Base;
} DESCTABLE;

//
// Task State Segment
//
typedef struct
{
	unsigned long	link;
	unsigned long	esp0, ss0;
	unsigned long	esp1, ss1;
	unsigned long	esp2, ss2;
	unsigned long	cr3;
	unsigned long	eip;
	unsigned long	eflags;
	unsigned long	eax, ecx, edx, ebx, esp, ebp;
	unsigned long	esi, edi;
	unsigned long	es, cs, ss, ds, fs, gs;
	unsigned long	ldt;
	unsigned short trap;
	unsigned short	iomap;
} TSS;
