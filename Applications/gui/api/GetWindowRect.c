// ------------------------------------------------------------
// GetWindowRect
// Copyright 1997 (c) Leprechaun Software. All Rights Reserved.
// ------------------------------------------------------------
#include <WinSystem.h>

// ------------------------------------------------------------
// void GetWindowRect(WND Wnd, RECT *pRect)
// ------------------------------------------------------------
// Parameters
//  Wnd		- Window structure
//  pRect	- Pointer to a RECT to receive window rectangle
//
// Returns
//  Nothing
// ------------------------------------------------------------
void GetWindowRect(WND Wnd, RECT *pRect)
{
	pRect->Top		= Wnd.Rect.Top;
	pRect->Left		= Wnd.Rect.Left;
	pRect->Bottom	= Wnd.Rect.Bottom;
	pRect->Right	= Wnd.Rect.Right;
};
