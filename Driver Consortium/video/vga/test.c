//
// Standard VGA Driver Test
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
#include <stdio.h>
#include "vga.h"
#include "..\video.h"

void VGA_Clear4();

void main()
{
	VGA_SetMode(G640x480x16);
	VGA_SetPalette();
	getch();
};
