//
// Console Support
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
#include "Console.h"

static short* vidmem = (void*) VID_COLOR_BASE;

//
//
//
TConsole::TConsole()
{
};

//
//
//
TConsole::~TConsole()
{
};

//
// Description:
//  Clears the console.
//
void TConsole::Clear()
{
	int c;

	for(c = 0; c < 2000; c++)
		vidmem[c] = DEF_MODE | ' ';
};

//
// Description:
//  Sets the position of the cursor.
//
void TConsole::SetPosition(int x, int y)
{
	CurPos = (y*80) + x;

	outb(CRT_INDEX, CR_CURSOR_HI);
	outb(CRT_DATA, CurPos>>8);
	outb(CRT_INDEX, CR_CURSOR_LO);
	outb(CRT_DATA, CurPos);
};

//
//
//
void TConsole::GetPosition()
{
	uchar lo = 0, hi = 0;

	outb(CRT_INDEX, CR_CURSOR_HI);
	hi = inb(CRT_DATA);
	outb(CRT_INDEX, CR_CURSOR_LO);
	lo = inb(CRT_DATA);

	CurPos = (hi<<8) + lo;
};

//
//
//
void TConsole::PutChar(unsigned char ch)
{
	int c;

	if(ch == '\r')
		return;

	if(ch == '\n')
   {
		CurPos = (CurPos + 80) / 80 * 80;
	}
	else
   {
		vidmem[curpos++] = DEF_MODE | ch;
	}

	if(CurPos >= 2000)
   {
		bcopy(vidmem + 80, vidmem, 3840);
		for(c = 1920; c <= 2000; c++)
			vidmem[c] = DEF_MODE | ' ';
		CurPos -= 80;
	}

	outb(CRT_INDEX, CR_CURSOR_HI);
	outb(CRT_DATA, CurPos>>8);
	outb(CRT_INDEX, CR_CURSOR_LO);
	outb(CRT_DATA, CurPos);
};

//
//
//
void TConsole::UnGetChar()
{
	CurPos--;
	vidmem[curpos] = DEF_MODE | ' ';

	outb(CRT_INDEX, CR_CURSOR_HI);
	outb(CRT_DATA, CurPos>>8);
	outb(CRT_INDEX, CR_CURSOR_LO);
	outb(CRT_DATA, CurPos);
};

//
//
//
void TConsole::Printf(const char *format, ...)
{
	int *dataptr = (int *)&format;
	char c;

	dataptr++;
	while((c = *format++))
   {
		if(c != '%')
      {
			if(c == '\t')
         {
				uint c = 4;
				while(c--)
					putchar(' ');
			}
         else
         {
				putchar(c);
			}
		}
		else
      {
			switch(c = *format++)
         {
			      case 'd':
               {
				      int num = *dataptr++;
				      char buf[10], *ptr = buf;

				      if num < 0)
                  {
					      num = -num;
					      putchar('-');
				      }

				      do
                  {
					      *ptr++ = '0' + num % 10;
                  } while(num /= 10);

				      do
                  {
					      putchar(*--ptr);
                  } while(ptr != buf);
				      break;
			      }

			      case 'x':
               {
				      unsigned int num = *dataptr++, dig;
				      char buf[8], *ptr = buf;

				      do
                  {
					      *ptr++ = ((dig = (num & 0xf)) > 9) ?
							('a' + dig - 10) :
							('0' + dig);
                  } while (num >>= 4);

				      do
                  {
					      putchar(*--ptr);
                  } while(ptr != buf);
				      break;
			      }

			      case 'c':
               	putchar((*dataptr++) & 0xff);
                  break;

			      case 's':
               {
				      char *ptr = (char *)*dataptr++;
				      while((c = *ptr++))
					      putchar(c);
				      break;
			      }
         	}
			}
      }
   }
};
