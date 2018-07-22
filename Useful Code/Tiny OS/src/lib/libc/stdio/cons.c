#include <types.h>
#include <stdarg.h>
#include <sys/io.h>
#include <sys/cons.h>
#include <sys/mem.h>

#define putchar putc

static short* vidmem = (void*) 0x200000;
static uint curpos;

void clrscr()
{
	int c;

	for(c = 0; c < 2000; c++)
		vidmem[c] = DEF_MODE | ' ';
}

void 
putc(unsigned char ch)
{
	int c;

	switch(ch) {
		case '\n':
			curpos = ( curpos + 80 ) / 80 * 80;
			break;
		case '\r':
			curpos--;
			break;
		default:
			vidmem[curpos++] = DEF_MODE | ch;
			break;
	}
	
	if( curpos >= 2000 ) {
		bcopy(vidmem + 80, vidmem, 3840);
		for(c = 1920; c <= 2000; c++)
			vidmem[ c ] = DEF_MODE | ' ';
		curpos -= 80;
	}

	outb(CRT_INDEX, CR_CURSOR_HI);
	outb(CRT_DATA, curpos>>8);
	outb(CRT_INDEX, CR_CURSOR_LO);
	outb(CRT_DATA, curpos);
}

void
ungetc()
{
	curpos--;
	vidmem[curpos] = DEF_MODE | ' ';

	outb(CRT_INDEX, CR_CURSOR_HI);
	outb(CRT_DATA, curpos>>8);
	outb(CRT_INDEX, CR_CURSOR_LO);
	outb(CRT_DATA, curpos);
}

void
get_curpos()
{
	uint hi = 0, lo = 0;

	outb(CRT_INDEX, CR_CURSOR_HI);
	hi = inb(CRT_DATA);
	outb(CRT_INDEX, CR_CURSOR_LO);
	lo = inb(CRT_DATA);

	curpos = (hi<<8) + lo;
}

void 
print(char* str)
{
	while(*str)
		putc(*str++);
}

static char printf_buf[1024];

void
printf(const char* fmt, ...)
{
	va_list args;
	int i;
	char* p = printf_buf;

	va_start(args, fmt);
	i = vsprintf(printf_buf, fmt, args);
	va_end(args);
	//write(-2, printf_buf, i);	
	
	get_curpos();

	while(*p)
		putc(*p++);
}
