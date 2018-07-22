//
// Get Palette
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
// Authors:
//  Jeremiah Cornelious McCarthy
//

//
// Description:
//  Sets the palette
//
int PRIM_SetPalette(int index, int red, int green, int blue)
{
	if(__svgalib_grayscale)
   {
		if((unsigned) index >= sizeof(__svgalib_green_backup) / sizeof(__svgalib_green_backup[0]))
      {
			printf("vga_setpalette: color index %d out of range\n", index);
		}
		__svgalib_green_backup[index] = green;

		green = 0.299 * red + 0.587 * green + 0.114 * blue;
		if(green < 0)
		    green = 0;
		if(green > 255)
			green = 255;
	}

   return set_lut(index, red, green, blue);
};
