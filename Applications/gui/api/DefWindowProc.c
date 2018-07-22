// ------------------------------------------------------------
// Default Window Procedure
// Copyright 1997 (c) Leprechaun Software. All Rights Reserved.
// ------------------------------------------------------------
#include <WinSystem.h>

// ------------------------------------------------------------
// long DefWindowProc(WND Wnd, int Message)
// ------------------------------------------------------------
// Parameters
//  Wnd			- Window handle
//  Message		- Message ID
//
// Returns
//  Window structure
// ------------------------------------------------------------
long DefWindowProc(WND Wnd, int Message)
{
	RECT	Rect;

	switch(Message)
	{
		case WM_CREATE:
			return 1L;

		case WM_NCPAINT:
//			FillRect(Wnd.X+1, Wnd.Y+1, Wnd.X+Wnd.Width-2, Wnd.Y+Wnd.Height-2, 7);
//			Rectangle(Wnd.X, Wnd.Y, Wnd.X+Wnd.Width-1, Wnd.Y+Wnd.Height-1, 0);
//			Rectangle(Wnd.X+3, Wnd.Y+3, Wnd.X+Wnd.Width-4, Wnd.Y+Wnd.Height-4, 0);
//			Rectangle(Wnd.X+6, Wnd.Y+6, Wnd.X+Wnd.Width-7, Wnd.Y+30, 0);
			GetClientRect(Wnd, &Rect);
			Draw3DBox((int)Rect.Left, (int)Rect.Top, (int)Rect.Right, (int)Rect.Bottom, 2, 0);
			return 1L;

		case WM_LBUTTONDOWN:
			return 1L;

		case WM_MOUSEMOVE:
			return 1L;
	}
};