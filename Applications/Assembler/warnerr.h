#ifndef MOSA_WARNERR_H
#define MOSA_WARNERR_H
//
// Millennium Operating System Assembler
// Copyright 1997-1998 (c) Leprechuan Software. All Rights Reserved.
//
#include "input.h"

//
// Error code list
//
enum
{
	errExpectedNumber,
	errExpectedEOL,
	errExpectedIdentifier,
	errExpectedUsage,
	errExpectedType,
	errIdentifiersUnmatched,
};

void	Warning(INPUT *Input, int WarnCode);
void	Error(INPUT *Input, int ErrCode);

#endif
