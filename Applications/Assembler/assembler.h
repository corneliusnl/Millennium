#ifndef MOSA_ASSEMBLER_H
#define MOSA_ASSEMBLER_H
//
// Millenium Operating System Assembler
// Copyright 1997-1998 (c) Leprechaun Software.  All Rights Reserved.
//
#include <stdio.h>
class OUTFORM;
class PARSER;
class ARCHITECTURE;

//
// Defines
//
#define ASM_LIST	0x0001

//
// Assembler Class
//
class ASSEMBLER
{
private:
public:
	ARCHITECTURE	*Arch;
	OUTFORM		*OutForm;
	PARSER		*Parser;

	// Class construction/destruction
	ASSEMBLER();
	~ASSEMBLER();

	// Methods
	int	Go(long Options);
	int	Open(char *Filename);
	int	SetOutputFormat(int Outform);
};

#endif
