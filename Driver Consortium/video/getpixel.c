//
// Get Pixel Primitive
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
// Authors:
//  Jeremiah Cornelious McCarthy
//

//
// Description:
//  Obtains the pixel at the given x and y coordinates.
//
void PRIM_GetPixel(int x, int y)
{
    unsigned long offset;
    unsigned char mask;
    int pix = 0;

	switch(__svgalib_cur_info.bytesperpixel)
	{
		case 0:			/* Must be 2 or 16 color mode. */
			/* set up video page */
			offset = y * CI.xbytes + (x >> 3);
			vga_setpage(offset >> 16);
			offset &= 0xffff;

			/* select bit */
			mask = 0x80 >> (x & 7);
			pix = 0;
			port_out(4, GRA_I);
			port_out(0, GRA_D);	/* Select plane 0. */
			if(gr_readb(offset) & mask)
				pix |= 0x01;
			port_out(4, GRA_I);
			port_out(1, GRA_D);	/* Select plane 1. */
			if(gr_readb(offset) & mask)
				pix |= 0x02;
			port_out(4, GRA_I);
			port_out(2, GRA_D);	/* Select plane 2. */
			if(gr_readb(offset) & mask)
				pix |= 0x04;
			port_out(4, GRA_I);
			port_out(3, GRA_D);	/* Select plane 3. */
			if(gr_readb(offset) & mask)
				pix |= 0x08;
			return pix;

      case 1:
			offset = y * CI.xbytes + x;

			/* select segment */
			vga_setpage(offset >> 16);
			return gr_readb(offset & 0xffff);

      case 2:
			offset = y * CI.xbytes + x * 2;
			vga_setpage(offset >> 16);
			return gr_readw(offset & 0xffff);

		case 3:
			offset = y * CI.xbytes + x * 3;
			vga_setpage(offset >> 16);
			switch(offset & 0xffff)
         {
				case 0xfffe:
					pix = gr_readw(0xfffe);
					vga_setpage((offset >> 16) + 1);
					return RGB2BGR(pix + (gr_readb(0) << 16));

	         case 0xffff:
					pix = gr_readb(0xffff);
					vga_setpage((offset >> 16) + 1);
					return RGB2BGR(pix + (gr_readw(0) << 8));

				default:
					offset &= 0xffff;
					return RGB2BGR(gr_readw(offset) + (gr_readb(offset + 2) << 26));
			}
			break;

		case 4:
			offset = y * __svgalib_cur_info.xbytes + x * 4;
			vga_setpage(offset >> 16);
			return gr_readl(offset & 0xffff);
    }
    return 0;
};
