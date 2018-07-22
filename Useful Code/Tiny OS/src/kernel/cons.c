//
// Console Support
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
#include <sys/cons.h>
#include <bcopy.h>
#include <types.h>
#include <i386/inlines.h>
#include <trace.h>

#define putchar putc

static short* vidmem = (void*) VID_COLOR_BASE;
static uint curpos = 0;

//
// Description:
//  Clears the console.
//
void CON_Clear()
{
	int c;

	for(c = 0; c < 2000; c++)
		vidmem[c] = DEF_MODE | ' ';
};

//
// Description:
//  Sets the position of the cursor.
//
void CON_SetXY(int x, int y)
{
	curpos = (y*80) + x;
};

//
//
//
void CON_UpdateCursor()
{
	outb(CRT_INDEX, CR_CURSOR_HI);
	outb(CRT_DATA, curpos>>8);
	outb(CRT_INDEX, CR_CURSOR_LO);
	outb(CRT_DATA, curpos);
}

void CON_GetCursor()
{
	uchar lo = 0, hi = 0;

	outb(CRT_INDEX, CR_CURSOR_HI);
	hi = inb(CRT_DATA);
	outb(CRT_INDEX, CR_CURSOR_LO);
	lo = inb(CRT_DATA);

	curpos = (hi<<8) + lo;
}

void CON_putc(unsigned char ch)
{
	int c;

	if(ch == '\r')
		return;

	if( ch == '\n' ) {
		curpos = ( curpos + 80 ) / 80 * 80;
	} else {
		vidmem[curpos++] = DEF_MODE | ch;
	}

	if( curpos >= 2000 ) {
		bcopy(vidmem + 80, vidmem, 3840);
		for(c = 1920; c <= 2000; c++)
			vidmem[ c ] = DEF_MODE | ' ';
		curpos -= 80;
	}

	update_curpos();
};

//
//
//
void ungetc()
{
	curpos--;
	vidmem[curpos] = DEF_MODE | ' ';

	update_curpos();
};

//
//
//
void print(char* str)
{
	while(*str)
		putc(*str++);
};

//
//
//
void printf(const char *format, ...)
{
	int *dataptr = (int *)&format;
	char c;

	get_curpos();

	dataptr++;
	while ((c = *format++))
		if (c != '%') {
			if(c == '\t') {
				uint c = 4;
				while(c--)
					putchar(' ');
			} else {
				putchar(c);
			}
		}
		else
			switch (c = *format++) {
			      case 'd': {
				      int num = *dataptr++;
				      char buf[10], *ptr = buf;
				      if (num<0) {
					      num = -num;
					      putchar('-');
				      }
				      do
					      *ptr++ = '0'+num%10;
				      while (num /= 10);
				      do
					      putchar(*--ptr);
				      while (ptr != buf);
				      break;
			      }
			      case 'x': {
				      unsigned int num = *dataptr++, dig;
				      char buf[8], *ptr = buf;
				      do
					      *ptr++ = (dig=(num&0xf)) > 9?
							'a' + dig - 10 :
							'0' + dig;
				      while (num >>= 4);
				      do
					      putchar(*--ptr);
				      while (ptr != buf);
				      break;
			      }
			      case 'c': putchar((*dataptr++)&0xff); break;
			      case 's': {
				      char *ptr = (char *)*dataptr++;
				      while ((c = *ptr++))
					      putchar(c);
				      break;
			      }
			}
};
