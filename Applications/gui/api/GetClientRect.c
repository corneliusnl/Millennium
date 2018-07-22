// ------------------------------------------------------------
// GetClientRect
// Copyright 1997 (c) Leprechaun Software. All Rights Reserved.
// ------------------------------------------------------------
#include <WinSystem.h>

// ------------------------------------------------------------
// void GetClientRect(WND Wnd, RECT *pRect)
// ------------------------------------------------------------
// Parameters
//  Wnd		- Window structure
//  pRect	- Pointer to a RECT to receive window rectangle
//
// Returns
//  Nothing
// ------------------------------------------------------------
void GetClientRect(WND Wnd, RECT *pRect)
{
	pRect->Top		= Wnd.Rect.Top		+ 28;
	pRect->Left		= Wnd.Rect.Left 	+ 6;
	pRect->Bottom	= Wnd.Rect.Bottom	- 6;
	pRect->Right	= Wnd.Rect.Right	- 6;
};