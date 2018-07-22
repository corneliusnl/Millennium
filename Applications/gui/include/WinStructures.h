#ifndef HALUI_WINSTRUCT_H
#define HALUI_WINSTRUCT_H
// ------------------------------------------------------------
// HAL GUI Windowing Structures
// Copyright 1997 (c) Leprechaun Software. All Rights Reserved.
// ------------------------------------------------------------

// ------------------------------------------------------------
// POINT
// ------------------------------------------------------------
// Description
//  Defines a points X and Y coordinates
//
// Parameters
//  X			- Specifies a points X coordinate
//  Y			- Specifies a points Y coordinate
// ------------------------------------------------------------
typedef struct
{
	long	X;
	long	Y;
} POINT;

// ------------------------------------------------------------
// DC
// ------------------------------------------------------------
// Description
//  Defines a devices contexts limits
//
// Parameters
//  Top			- Specifies the top x-coordinate
//  Left			- Specifies the top y-coordinate
//  Bottom		- Specifies the bottom x-coordinate
//  Right		- Specifies the bottom y-coordinate
// ------------------------------------------------------------
typedef struct
{
	int	Top;
	int	Left;
	int	Bottom;
	int	Right;
} DC;

// ------------------------------------------------------------
// RECT
// ------------------------------------------------------------
// Description
//  Defines the corners of a two dimensional rectangle
//
// Parameters
//  Top			- Specifies the top x-coordinate
//  Left			- Specifies the top y-coordinate
//  Bottom		- Specifies the bottom x-coordinate
//  Right		- Specifies the bottom y-coordinate
// ------------------------------------------------------------
typedef struct
{
	long	Top;
	long	Left;
	long	Bottom;
	long	Right;
} RECT;

// ------------------------------------------------------------
// WND
// ------------------------------------------------------------
// Description
//  Defines a windows properties
//
// Parameters
//  Caption		- Specifies the windows caption
//  Style		- Specifies the characteristics of the window
//  Frame		- Specifies the window frames device context
//  Client		- Specifies the window clients device context
// ------------------------------------------------------------
typedef struct
{
	char	Caption[256];
	long	Style;
	RECT	Rect;

} WND;

// ------------------------------------------------------------
// CURSOR
// ------------------------------------------------------------
// Description
//  Defines a mouse cursor
//
// Parameters
//  Top			- Specifies the top x-coordinate
//  Left			- Specifies the top y-coordinate
//  Bottom		- Specifies the bottom x-coordinate
//  Right		- Specifies the bottom y-coordinate
// ------------------------------------------------------------
typedef struct
{
	int	XHotSpot;
	int	YHotSpot;
	void	*Cursor;
	void	*Mask;
} CURSOR;

// ------------------------------------------------------------
// BITMAPFILEHEADER
// ------------------------------------------------------------
// Description
//  Bimtap file header structure
//
// Parameters
//  bfType			- Specifies the type (must be BM)
//  bfSize			- Specifies the size of the file in bytes
//  bfReserved1	- Reserved (must be zero)
//  bfReserved2	- Reserved (must be zero)
//  bfOffBits		- Specifies the offset, in bytes, from the
//						BITMAPFILEHEADER to the bitmaps bits
// ------------------------------------------------------------
typedef struct
{
	short	bfType;
	long	bfSize;
	short	bfReserved1;
	short	bfReserved2;
	long	bfOffBits;
} BITMAPFILEHEADER;

// ------------------------------------------------------------
// BITMAPINFOHEADER
// ------------------------------------------------------------
// Description
//  Contains information about a bitmap
//
// Parameters
//  biSize				- Specifies the number of bytes required by
//							the structure
//  biWidth				-Specifies the width of the bitmap, in pixels
//  biHeight			-Specifies the height of the bitmap, in pixels
//							If positive, it is a bottom up bitmap
//							If negative, it is a top down bitmap
//  biPlanes			-Specifies the number of planes (must be 1)
//  biBitCount			-Specifies the number of bits per pixel
//  biCompression		-Specifies the compression scheme used
//  biSizeImage		-Specifies the size of the bitmap in bytes
//  biXPelsPerMeter	- Specifies the horizontal resolution, in
//							pixels per meter
//  biYPelsPerMeter	- Specifies the vertical resolution, in
//							pixels per meter
//  biClrUsed			-Specifies the actual number of unique colors used
//  biClrImportant	-Specifies the number of colors considered to be
//							most important in the displaying of the image
// ------------------------------------------------------------
typedef struct
{
	long	biSize;
	long	biWidth;
	long	biHeight;
	short	biPlanes;
	short	biBitCount;
	long	biCompression;
	long	biSizeImage;
	long	biXPelsPerMeter;
	long	biYPelsPerMeter;
	long	biClrUsed;
	long	biClrImportant;
} BITMAPINFOHEADER;

// ------------------------------------------------------------
// RGBQUAD
// ------------------------------------------------------------
// Description
//  Defines a colors red, green, and blue components as well as
//  reserved component
//
// Parameters
//  rgbBlue			- Blue component
//  rgbGreen		- Green component
//  rgbRed			- Red component
//  rgbReserved	- Reserved
// ------------------------------------------------------------
typedef struct
{
	unsigned char rgbBlue;
	unsigned char rgbGreen;
	unsigned char rgbRed;
	unsigned char rgbReserved;
} RGBQUAD;

// ------------------------------------------------------------
// RGBTRIPLE
// ------------------------------------------------------------
// Description
//  Defines a colors red, green, and blue components
//
// Parameters
//  rgbtRed			- Red component
//  rgbtGreen		- Green component
//  rgbtBlue		- Blue component
// ------------------------------------------------------------
typedef struct
{
	unsigned char rgbRed;
	unsigned char rgbGreen;
	unsigned char rgbBlue;
} RGBTRIPLE;

// ------------------------------------------------------------
// BITMAP
// ------------------------------------------------------------
// Description
//  Contains relevant information about a bitmap
//
// Parameters
//  bmWidth			- Specifies the bitmaps width, in pixels
//  bmHeight		- Specifies the bitmaps height, in pixels
//  bmWidthBytes	- Specifies the bitmaps width in bytes
//  bmBitCount		- Specifies the number of bits per pixel
//  bmBits			- Specifies the bitmaps bits
//  bmPalette		- Specifies the bitmaps palette
// ------------------------------------------------------------
typedef struct
{
	long			bmWidth;
	long			bmHeight;
	long			bmWidthBytes;
	short			bmBitCount;
	void			*bmBits;
	RGBTRIPLE	*bmPalette;
} BITMAP;

#endif
