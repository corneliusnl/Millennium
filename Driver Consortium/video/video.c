//
// Millennium Video Subsystem
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
// Authors:
//  Jeremiah Cornelious McCarthy
//
#include "Video.h"

//
// Video modes
//
MODE	Modes[] =
{
	// 80x25
	{"80x25x16", 80, 25, 16, 40},
	// 320x200
	{"320x200x16", 320, 200, 16, 160},
	{"320x200x256",	320, 200, 256, 320},
	{"320x200x32K",	320, 200, 1 << 15, 320},
	{"320x200x64K",	320, 200, 1 << 16, 320},
	{"320x200x16M", 320, 200, 1 << 24, 320},
	// 640x480
	{"640x480x16", 640, 480, 16, 320},
	{"640x480x256",	640, 480, 256, 640},
	{"640x480x32K",	640, 480, 1 << 15, 640},
	{"640x480x64K",	640, 480, 1 << 16, 640},
	{"640x480x16M",	640, 480, 1 << 24, 640},
	// 800x600
	{"800x600x16",	800, 600, 16, 400},
	{"800x600x256",	800, 600, 256, 800},
	{"800x600x32K",	800, 600, 1 << 15, 800},
	{"800x600x64K",	800, 600, 1 << 16, 800},
	{"800x600x16M",	800, 600, 1 << 24, 800},
	// 1024x768
	{"1024x768x16",	1024, 768, 16, 512},
	{"1024x768x256", 1024, 768, 256, 1024},
	{"1024x768x32K", 1024, 768, 1 << 15, 1024},
	{"1024x768x64K", 1024, 768, 1 << 16, 1024},
	{"1024x768x16M", 1024, 768, 1 << 24, 1024},
	// 1600x1200
	{"1600x1200x16", 1600, 1200, 16, 512},
	{"1600x1200x256", 1600, 1200, 256, 1024},
	{"1600x1200x32K", 1600, 1200, 1 << 15, 1024},
	{"1600x1200x64K", 1600, 1200, 1 << 16, 1024},
	{"1600x1200x16M", 1600, 1200, 1 << 24, 1024},
};

//
// void VIDEO_SetMode(int Mode)
//
// Description
//
// Parameters
//
// Returns
//
void VIDEO_SetMode(int Mode)
{
};

//
// void VIDEO_TurnScreenOn()
//
// Description
//
// Parameters
//
// Returns
//
void VIDEO_TurnScreenOn()
{
	outb(0x3C4, 0x01);
	Data = inb(0x3C5);
	outb(0x3C4, 0x01);
	outb(0x3C5, Data & 0xDF);
};

//
// void VIDEO_TurnScreenOff()
//
// Description
//
// Parameters
//
// Returns
//
void VIDEO_TurnScreenOff()
{
	outb(0x3C4, 0x01);
	Data = inb(0x3C5);
	outb(0x3C4, 0x01);
	outb(0x3C5, Data | 0x20);
};
