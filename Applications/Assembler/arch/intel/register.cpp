//
// Millennium Operating System Assembler
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
#include "registers.h"

//
// Registers
//
REGISTER	Register[] =
{
	{"al",		0,	AL_REG},
	{"ax",		0,	AX_REG},
	{"eax",		0,	EAX_REG},
	{"mm0",		0,	REG},
	{"es",		0,	SREG|BITS16},
	{"cr0",		0,	REG},
	{"dr0",		0,	REG},
	{"st0",		0,	REG},

	{"cl",		1,	REG|BITS8},
	{"cx",		1,	REG|BITS16},
	{"ecx",		1,	REG|BITS32},
	{"mm1",		1,	REG},
	{"cs",		1,	SREG|BITS16},
	{"dr1",		1,	REG},
	{"st1",		1,	REG},

	{"dl",		2,	REG|BITS8},
	{"dx",		2,	REG|BITS16},
	{"edx",		2,	REG|BITS32},
	{"mm2",		2,	REG},
	{"ss",		2,	SREG|BITS16},
	{"cr2",		2,	REG},
	{"dr2",		2,	REG},
	{"st2",		2,	REG},

	{"bl",		3,	REG|BITS8},
	{"bx",		3,	REG|BITS16},
	{"ebx",		3,	REG|BITS32},
	{"mm3",		3,	REG},
	{"ds",		3,	SREG|BITS16},
	{"cr3",		3,	REG},
	{"dr3",		3,	REG},
	{"st3",		3,	REG},

	{"ah",		4,	REG|BITS8},
	{"sp",		4,	REG|BITS16},
	{"esp",		4,	REG|BITS32},
	{"mm4",		4,	REG},
	{"fs",		4,	SREG|BITS16},
	{"cr4",		4,	REG},
	{"st4",		4,	REG},
	{"tr4",		4,	REG},

	{"ch",		5,	REG|BITS8},
	{"bp",		5,	REG|BITS16},
	{"ebp",		5,	REG|BITS32},
	{"mm5",		5,	REG},
	{"gs",		5,	SREG|BITS16},
	{"st5",		5,	REG},
	{"tr5",		5,	REG},

	{"dh",		6,	REG|BITS8},
	{"si",		6,	REG|BITS16},
	{"esi",		6,	REG|BITS32},
	{"mm6",		6,	REG},
	{"dr6",		6,	REG},
	{"st6",		6,	REG},
	{"tr6",		6,	REG},

	{"bh",		7,	REG|BITS8},
	{"di",		7,	REG|BITS16},
	{"edi",		7,	REG|BITS32},
	{"mm7",		7,	REG},
	{"dr7",		7,	REG},
	{"st7",		7,	REG},
	{"tr7",		7,	REG},
};
