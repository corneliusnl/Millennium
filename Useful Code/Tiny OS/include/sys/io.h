/*
 * IO stuff, routines from io.S
 *
 * $Id$
 *
 */
#ifndef _IO_H_
#define _IO_H_

#include <types.h>
 
extern uchar inb(int);
extern void outb(int, uchar);
extern ushort inw();
extern void outw(int, ushort);
extern void repinsw(int, void*, int);
extern void repoutsw(int, void*, int);

#endif /* _IO_H_ */
