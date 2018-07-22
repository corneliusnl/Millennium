// ------------------------------------------------------------
// Raster Font
// Copyright 1997 (c) Leprechaun Software.  All Rights Reserved.
// ------------------------------------------------------------

int LoadFont(char *FontFile)
{
	RASTERFONTHEADER	Header;

	fread(&Header, sizeof(RASTERFONTHEADER), 1, fontfp);
	if(Header.Magic != 0x4652)
	{
	}
	fread(wtable, Header.Size, 1, fontfp);
};