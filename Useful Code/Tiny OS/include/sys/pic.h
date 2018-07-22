#ifndef _PIC_H_
#define _PIC_H_

/*
 * programmable interrupt controler
 */

#include <types.h>

#define		ICU0		0x20
#define		ICU1		0xa0

#define		ICU_RESET	0x11

#define		IDT_TRAPS_NO	0x20
#define		IDT_HIRQS_NO	0x08

void pic_init(ushort,ushort);
void enable_irq(ushort);
void disable_irq(ushort);
void set_res();

#endif	/* _PIC_H_ */

