// ------------------------------------------------------------
// Create Window
// Copyright 1997 (c) Leprechaun Software. All Rights Reserved.
// ------------------------------------------------------------
#include <WinSystem.h>
#include "Controls.h"
#include "Vga.h"

// ------------------------------------------------------------
// WND CreateWindow(char *Caption, long Style, int X, int Y,
//			int Width, int Height)
// ------------------------------------------------------------
// Parameters
//  Caption		- Specifies the windows caption
//  Style		- Window properties
//  X				- Horizontal coordinate
//  Y				- Vertical coordinate
//  Width		- Window Width
//  Height		- Widow Height
//
// Returns
//  Window structure
// ------------------------------------------------------------
WND CreateWindow(char *Caption, long Style, int X, int Y, int Width, int Height)
{
	WND	Wnd;

	//
	// Set Window Parameters
	//
	strcpy(Wnd.Caption, Caption);
	Wnd.Style			= Style;
	Wnd.Rect.Top		= Y;
	Wnd.Rect.Left		= X;
	Wnd.Rect.Bottom	= Y+Height;
	Wnd.Rect.Right		= X+Width;

	return Wnd;
};
