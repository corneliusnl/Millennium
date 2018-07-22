/*
 * hardware console
 * processes should use <stdio.h>
 */
#ifndef _CONS_H_
#define _CONS_H_

#include <sys/video.h>

#define		VID_COLOR_BASE	0xb8000

/*
 * CRT controller ports (color)
 */
#define		CRT_INDEX	(0x03D4)
#define		CRT_DATA	(0x03D5)

/*
 * CRT controller registers
 */
#define 	CR_CURSLINE1	(0x0A)
#define		CR_CURSLINE2	(0x0B)
#define		CR_CURSOR_HI	(0x0E)
#define		CR_CURSOR_LO	(0x0F)

/*
 * display modes
 */
#define		DEF_MODE	(YELLOW|BG_BLACK)

void clrscr();
void putc(unsigned char);
void ungetc();
void print(char*);
void printf(const char*,...);

#endif /* _CONS_H_ */
