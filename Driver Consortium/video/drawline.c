//
// Drawline Primitive
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
// Authors:
//  Jeremiah Cornelious McCarthy
//
// References:
//  Svgalib
//

int PRIM_DrawLine(int x1, int y1, int x2, int y2)
{
	int dx = x2 - x1;
	int dy = y2 - y1;
	int ax = ABS(dx) << 1;
	int ay = ABS(dy) << 1;
	int sx = (dx >= 0) ? 1 : -1;
	int sy = (dy >= 0) ? 1 : -1;

	int x = x1;
	int y = y1;

	if(ax > ay)
   {
		int d = ay - (ax >> 1);

		while(x != x2)
		{
			PRIM_DrawPixel(x, y);

			if(d > 0 || (d == 0 && sx == 1))
         {
				y += sy;
				d -= ax;
			}
			x += sx;
			d += ay;
		}
	}
   else
   {
		int d = ax - (ay >> 1);
		while (y != y2)
      {
			PRIM_DrawPixel(x, y);

			if (d > 0 || (d == 0 && sy == 1))
         {
				x += sx;
				d -= ay;
			}
			y += sy;
			d += ax;
		}
	}
	PRIM_DrawPixel(x, y);

	return 0;
}