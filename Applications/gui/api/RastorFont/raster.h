#ifndef GUI_RASTER_H
#define GUI_RASTER_H
// ------------------------------------------------------------
// Raster Font
// Copyright 1997 (c) Leprechaun Software.  All Rights Reserved.
// ------------------------------------------------------------

typedef struct
{
	int	Magic;			// Font file magic number (0x4652)
	long	Size;				// Bitmap size
	short	Width;			// Width
	short	Height;			// Height
	char	FontFace[16];	// Font face name

} RASTERFONTHEADER;

#endif