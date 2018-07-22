// ------------------------------------------------------------
// SetRect
// Copyright 1997 (c) Leprechaun Software. All Rights Reserved.
// ------------------------------------------------------------
#include <WinSystem.h>

// ------------------------------------------------------------
// void SetRect(RECT *pRect, int Top, int Left, int Bottom, int Right)
// ------------------------------------------------------------
// Parameters
//  pRect	- Pointer to a RECT to be defined by the points
//  Top		- Specifies the first veritical position
//  Left		- Specifies the first horizontal position
//  Bottom	- Specifies the second vertical position
//  Right	- Specifies the second horizontal position
//
// Returns
//  Nothing
// ------------------------------------------------------------
void SetRect(RECT *pRect, int Top, int Left, int Bottom, int Right)
{
	pRect->Top		= Top;
	pRect->Left		= Left;
	pRect->Bottom	= Bottom;
	pRect->Right	= Right;
};