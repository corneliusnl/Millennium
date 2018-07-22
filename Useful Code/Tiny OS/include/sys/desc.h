/*
 * 386 gate and system descriptors
 *
 * $Id$
 *
 */
#ifndef _DESC_H_
#define _DESC_H_

#include <types.h>

struct gate_desc {
	ushort	offset_0;
	ushort	selector;
	ushort	type;
	ushort	offset_16;
};

struct sys_desc {
	ushort	limit;
	ushort	base_0_15;
	uchar	base_16_23;
	uchar	dpl_type;
	uchar	gav_lim;
	uchar	base_24_31;
};

#endif /* _DESC_H_ */
