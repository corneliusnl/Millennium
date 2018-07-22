/*
 * segment descriptors, Intel crap ...
 *
 * $Id: DESC.C 1.1 1996/11/26 15:40:40 bart Exp $
 *
 */

#include <sys/desc.h>
#include <sys/mem.h>
#include <sys/kernel.h>

/*
 * interrupt descriptor table
 * filled during kernel initialization
 */

struct gate_desc idt[256] = { 0 };

/*
 *  GDT
 */

struct sys_desc gdt[256] = {
/* 0x00 null 	    */	{ 0, 0, 0, 0, 0, 0 },
/* 0x08 kernel code */	{ 0xFFFF, 0x0000, 0x00, 0x9A, 0xCF, 0x00 },
/* 0x10 kernel data */	{ 0xFFFF, 0x0000, 0x00, 0x92, 0xCF, 0x00 },
/* 0x18 user code   */	{ 0xFFFF, 0x0000, 0x00, 0xFA, 0xCF, 0x00 },
/* 0x20 user data   */	{ 0xFFFF, 0x0000, 0x00, 0xF2, 0xCF, 0x00 }, 
/* 0x28 tss	    */	{0},
			{0} };

/*
 * kernel page dir and table 
 */

ulong pd[ NPTE ] = {0};
ulong pt[ NPTE ] = {0};

