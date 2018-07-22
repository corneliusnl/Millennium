// ------------------------------------------------------------
// Load Bitmap
// Copyright 1997 (c) Leprechaun Software. All Rights Reserved.
// ------------------------------------------------------------
#include <StdLib.h>
#include <StdIO.h>
#include <WinSystem.h>

// ------------------------------------------------------------
// void LoadBitmap(char *Filename)
// ------------------------------------------------------------
// Parameters
//  Filename	- Name of file to be opened
//
// Returns
//  Nothing
// ------------------------------------------------------------
int LoadBitmap(char *Filename, BITMAP *Bitmap)
{
	BITMAPFILEHEADER	FileHeader;
	BITMAPINFOHEADER	InfoHeader;
	RGBQUAD				*Palette;
	FILE	*fp;
	char	*TempData;
	char	*Bits;
	long	i, w, h, SizeImage;
	int	NumColors;

	//
	// Open file
	//
	fp = fopen(Filename, "rb");

	if(!fp)
	{
		printf("Unable to open \"%s\"!", Filename);
		fclose(fp);
		return 0;
	}

	//
	// Read bitmap header
	//
	fread(&FileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
	if(FileHeader.bfType != 0x4D42)
	{
		printf("Invalid bitmap format!");
		fclose(fp);
		return 0;
	}

	//
	// Read bitmap information
	//
	fread(&InfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	Bitmap->bmWidth	  	= InfoHeader.biWidth;
	Bitmap->bmHeight    	= InfoHeader.biHeight;
	Bitmap->bmWidthBytes	= (Bitmap->bmWidth + 3) & ~3;
	Bitmap->bmBitCount  	= InfoHeader.biBitCount;
	SizeImage				= Bitmap->bmWidthBytes * Bitmap->bmHeight;

	switch(Bitmap->bmBitCount)
	{
		case 1:	NumColors = 2;		break;
		case 4:	NumColors = 16;	break;
		case 8:	NumColors = 256;	break;
		default:
			printf("Invalid number of bits per pixel!");
			fclose(fp);
			return 0;
	}

	//
	// Read bitmap palette
	//
	Bitmap->bmPalette = (RGBTRIPLE *)malloc(NumColors * sizeof(RGBTRIPLE));
	Palette = (RGBQUAD *)malloc(NumColors * sizeof(RGBQUAD));
	if((!Bitmap->bmPalette) || (!Palette))
	{
		printf("Unable to allocate memory for palette!");
		fclose(fp);
		return 0;
	}
//	printf("sizeof(bmPal): %d Malloc: %d", sizeof(Bitmap->bmPalette), NumColors*sizeof(RGBQUAD));
	fread(Palette, sizeof(RGBTRIPLE), NumColors, fp);
	for(i = 0; i < NumColors; i++)
	{
		Bitmap->bmPalette[i].rgbRed 	= Palette[i].rgbRed;
		Bitmap->bmPalette[i].rgbGreen	= Palette[i].rgbGreen;
		Bitmap->bmPalette[i].rgbBlue	= Palette[i].rgbBlue;
	}

	//
	// Read bitmap data
	//
	TempData = (char *)malloc(SizeImage * sizeof(char));
	if(!Bitmap->bmBits)
	{
		printf("Unable to allocate memory for image!");
		fclose(fp);
		return 0;
	}
	fread(TempData, sizeof(char), SizeImage, fp);

	Bitmap->bmBits = (void *)malloc(SizeImage * sizeof(char));
	if(!Bitmap->bmBits)
	{
		printf("Unable to allocate memory for image!");
		fclose(fp);
		return 0;
	}
	Bits = (char *)Bitmap->bmBits;

	// Correct Bitmap Alignment
	for(h = 0; h < Bitmap->bmHeight; h++)
		for(w = 0; w < Bitmap->bmWidthBytes; w++)
			*(Bits + (h * Bitmap->bmWidthBytes) + w) = TempData[SizeImage - ((h+1) * Bitmap->bmWidthBytes) + w];

	free(TempData);
	fclose(fp);
	return 1;
};
