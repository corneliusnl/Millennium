#ifndef _TSS_H_
#define _TSS_H_

/*
 * TSS needed by kernel
 */

#include <types.h>

#define		KTSS_SEL	(0x28)

/* Task State Segment */

struct tss {
	ulong	link;
	ulong	esp0, ss0;
	ulong	esp1, ss1;
	ulong	esp2, ss2;
	ulong	cr3;
	ulong	eip;
	ulong	eflags;
	ulong	eax, ecx, edx, ebx, esp, ebp;
	ulong	esi, edi;
	ulong	es, cs, ss, ds, fs, gs;
	ulong	ldt;
	ushort 	trap;
	ushort	iomap;
};

#endif /* _TSS_H_ */
