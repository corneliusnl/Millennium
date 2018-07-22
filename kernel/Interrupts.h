#ifndef KERNEL_INTERRUPTS_H
#define KERNEL_INTERRUPTS_H
//
// Interrupts
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//

//
// Interrupt Descriptor
//
typedef struct
{
	unsigned short		Offset0_15;
   unsigned short		Selector;
   unsigned char		ZeroByte;
   unsigned char		Flags;
   unsigned short		Offset16_31;
} INTDESC;

#endif