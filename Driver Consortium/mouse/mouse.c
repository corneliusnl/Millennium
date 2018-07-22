//
// Mouse Handler Demonstration
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
#include <stdio.h>
#include <dos.h>
#include <conio.h>
#include "io.h"

//
// Variables and defines
//
#define COM1INTR 0x0C
#define COM1PORT 0x3F8

void interrupt (*OldHandler)();
unsigned char	combytes[3];
int		bytenum;
int		x, y;
int		button1, button2;

//
// Mouse interrupt handler
//
void interrupt MyMouseHandler()
{
	int	dx, dy;
	unsigned char inbyte;

	// Get the port byte
	inbyte = inb(COM1PORT);

	// Make sure we are properly "synched"
	if((inbyte & 64) == 64)
		bytenum = 0;

	// Store the byte and adjust bytenum
	combytes[bytenum] = inbyte;
	bytenum++;

	// Have we received all 3 bytes?
	if(bytenum == 3)
	{
		// Yes, so process them
		dx = ((combytes[0] & 3) << 6) + combytes[1];
		dy = ((combytes[0] & 12) << 4) + combytes[2];
		if(dx >= 128) dx = dx - 256;
		if(dy >= 128) dy = dy - 256;
		x = x + dx;
		y = y + dy;
		button1 = (combytes[0] & 32);
		button2 = (combytes[0] & 16);

		// And start on first byte again
		bytenum = 0;
	}

	// Acknowledge the interrupt
	outb(0x20, 0x20);
};

//
// Entry point
//
void main()
{
	OldHandler = getvect(COM1INTR);
	setvect(COM1INTR, MyMouseHandler);

	// Initialize the normal mouse handler
	asm
	{
		mov ax, 0
		int 0x33
	};

	// Initialize some of the variables we'll be using
	bytenum = 0;
	x = 0;
	y = 0;
	button1 = 0;
	button2 = 0;

	while(1)
	{
		printf("x:%d y:%d button1 : %d button2: %d", x, y, button1, button2);
	}

	setvect(COM1INTR, OldHandler);
};
