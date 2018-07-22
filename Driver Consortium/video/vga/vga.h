#ifndef VGA_VGA_H
#define VGA_VGA_H
//
// Standard VGA Video Driver
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//

int	VGA_Probe();
void	VGA_Initialize();
int	VGA_ModeAvailable();
int	VGA_SetMode(int Mode);
void	VGA_SetPalette();

//
// VGA Port Addresses
//
#define ATTRIB_ADDR	0x3C0
#define MISC_ADDR	0x3C2
#define VGAENABLE_ADDR	0x3C3
#define SEQ_ADDR	0x3C4
#define GFXC_ADDR	0x3CE
#define CRTC_ADDR	0x3D4
#define STATUS_ADDR	0x3DA

#endif
