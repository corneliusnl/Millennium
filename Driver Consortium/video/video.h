#ifndef KERNEL_VIDEO_H
#define KERNEL_VIDEO_H
//
// Millennium Video Subsystem
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//

//
// Video driver structure
//
typedef struct
{
	char	*Description;
	int	(*Probe)		();
	int	(*Init)			();
	int	(*SetMode)		(int Mode);
//	int	(*test)			(void);
//	void	(*SetPage)		(int Page);
//	void	(*SetRdPage)		(int Page);
//	void	(*SetWrPage)		(int Page);
//	int	(*ModeAvailable)	(int Mode);
//	void	(*SetDisplayStart)	(int Address);
//	void	(*SetLogicalWidth)	(int Width);
//	void	(*GetModeInfo)		(int Mode, vga_modeinfo * modeinfo);

} VIDEODRIVER;

//
// Video Mode
//
typedef struct
{
	char	*Description;
	int	Width;
	int	Height;
	int	NumColors;
	int	BytesPerLine;
} MODE;

//
// Mode table
//
typedef struct
{
	int		Mode;
	unsigned char	*Registers;

} MODETABLE;

//
// Video Modes
//
#define T80x25x16	0
// 320x200
#define G320x200x16	1
#define G320x200x256	2
#define G320x200x32K	3
#define G320x200x64K	4
#define G320x200x16M 	5
// 640x480
#define G640x480x16	6
#define G640x480x256	7
#define G640x480x32K	8
#define G640x480x64K	9
#define G640x480x16M	10
// 800x600
#define G800x600x16	11
#define G800x600x256	12
#define G800x600x32K	13
#define G800x600x64K	14
#define G800x600x16M	15
// 1024x768
#define G1024x768x16	16
#define G1024x768x256	17
#define G1024x768x32K	18
#define G1024x768x64K	19
#define G1024x768x16M	20
// 1280x1024
#define G1280x1024x16	21
#define G1280x1024x256	22
#define G1280x1024x32K	23
#define G1280x1024x64K	24
#define G1280x1024x16M	25
// 1600x1200
#define G1600x1200x16	26
#define G1600x1200x256	27
#define G1600x1200x32K	28
#define G1600x1200x64K	29
#define G1600x1200x16M	30

#endif
