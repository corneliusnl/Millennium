#ifndef KERNEL_IO_H
#define KERNEL_IO_H
//
// Input/Output Port Routines
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
extern void  			outb(int port, unsigned char value);
extern void  			outw(int port, int value);
extern unsigned char		inb(int port);
extern int	      		inw(int port);

#endif