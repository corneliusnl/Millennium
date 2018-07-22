//
// Clear Screen Primitive
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
// Authors:
//  Jeremiah Cornelious McCarthy
//

//
// Description:
//  Clears the screen.
//
int PRIM_Clear(void)
{
	vga_screenoff();

	switch(CM)
   {
		case G320x200x256:
			/* write to all planes */
			port_out(0x02, SEQ_I);
			port_out(0x0F, SEQ_D);

			/* clear video memory */
			memset(GM, 0, 65536);
			break;

		default:
			switch(CI.colors)
         {
				case 2:
				case 16:
					vga_setcolor(0);

					/* write to all bits */
					port_out(0x08, GRA_I);
					port_out(0xFF, GRA_D);

				default:
				{
					int i;
			  		int pages = (CI.ydim * CI.xbytes + 65535) >> 16;

					for(i = 0; i < pages; ++i)
	            {
						vga_setpage(i);

						/* clear video memory */
						memset(GM, 0, 65536);
					}
				}
				break;
		}
		break;
	}

	vga_setcolor(15);
	vga_screenon();

	return 0;
};