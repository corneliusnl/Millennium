// ------------------------------------------------------------
// FreeBitmap
// Copyright 1997 (c) Leprechaun Software. All Rights Reserved.
// ------------------------------------------------------------
#include <WinSystem.h>

// ------------------------------------------------------------
// void FreeBitmap(BITMAP *Bitmap)
// ------------------------------------------------------------
// Parameters
//  Bitmap		- Pointer to bitmap to be freed
//
// Returns
//  Nothing
// ------------------------------------------------------------
void FreeBitmap(BITMAP *Bitmap)
{
	free(Bitmap->bmBits);
	free(Bitmap->bmPalette);
};