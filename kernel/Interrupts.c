//
// Interrupts
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
// Authors:
//  Jeremiah Cornelious McCarthy
//
// References:
//  Intel Architecture Software Developers Manual
//
#include "Interrupts.h"

IDTDESC	Interrupt[256];

//
// Description:
//  Initializes the Interrupt Descriptor Table
//
void IDT_Initialize()
{
};

//
// Description:
//
//
void IDT_SetInterrupt(unsigned char Int, unsigned long Offset, unsigned short Type)
{
   Interrupt[Int].Selector		=
   Interrupt[Int].ZeroByte		= 0;
   Interrupt[Int].Flags			=
	Interrupt[Int].Offset0_15	= Offset;
   Interrupt[Int].Offset16_31	= Offset >> 16;
};

//
// Description:
//
void IDT_GetInterrupt(unsigned char Int)
{
};
