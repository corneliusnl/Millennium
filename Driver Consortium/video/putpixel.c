//
// Put Pixel Primitive
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
// Authors:
//  Jeremiah Cornelious McCarthy
//

//
// Description:
//  Plots a pixel at the given x and y coordinates.
//
int PRIM_PutPixel(int x, int y)
{
	unsigned long offset;
	int c;

	switch(__svgalib_cur_info.bytesperpixel)
   {
		case 0:/* Must be 2 or 16 color mode. */
			/* set up video page */
			offset = y * CI.xbytes + (x >> 3);
			vga_setpage(offset >> 16);
			offset &= 0xffff;

			/* select bit */
			port_out(8, GRA_I);
			port_out(0x80 >> (x & 7), GRA_D);

			/* read into latch and write dummy back */
			read_write(offset);
			break;

		case 1:
			offset = y * CI.xbytes + x;

			/* select segment */
			vga_setpage(offset >> 16);

			/* write color to pixel */
			gr_writeb(COL, offset & 0xffff);
			break;

      case 2:
			offset = y * CI.xbytes + x * 2;
			vga_setpage(offset >> 16);
			gr_writew(COL, offset & 0xffff);
			break;

		case 3:
			c = RGB2BGR(COL);
			offset = y * CI.xbytes + x * 3;
			vga_setpage(offset >> 16);

			switch(offset & 0xffff)
         {
				case 0xfffe:
					gr_writew(c, 0xfffe);
					vga_setpage((offset >> 16) + 1);
					gr_writeb(c >> 16, 0);
					break;

				case 0xffff:
					gr_writeb(c, 0xffff);
					vga_setpage((offset >> 16) + 1);
					gr_writew(c >> 8, 0);
					break;

				default:
					offset &= 0xffff;
					gr_writew(c, offset);
					gr_writeb(c >> 16, offset + 2);
					break;
			}
			break;

		case 4:
			offset = y * __svgalib_cur_info.xbytes + x * 4;
			vga_setpage(offset >> 16);
			gr_writel((MODEFLAGS & RGB_MISORDERED) ? (COL << 8) : COL, offset & 0xffff);
			break;
    }
    return 0;
};
