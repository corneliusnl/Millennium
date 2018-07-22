//
// Get Palette Primitive
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
// Authors:
//  Jeremiah Cornelious McCarthy
//

//
// Description:
//  Gets a specified palette entry.
//
int PRIM_GetPalette(int index, int *red, int *green, int *blue)
{
	get_lut(index, red, green, blue);

	if(__svgalib_grayscale)
	{
		if((unsigned) index >= sizeof(__svgalib_green_backup) / sizeof(__svgalib_green_backup[0]))
      {
			printf("vga_getpalette: color index %d out of range\n", index);
		}
		*green = __svgalib_green_backup[index];
	}

   return 0;
};
