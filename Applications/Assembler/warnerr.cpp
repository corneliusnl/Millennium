//
// Millennium Operating System Assembler
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
#include <stdio.h>
#include "warnerr.h"
#include "input.h"

//
// void Warning(INPUT *Input, int WarnCode)
//
// Description
//
// Parameters
//
// Returns
//
void Warning(INPUT *Input, int WarnCode)
{
};

//
// Error messages
//
char *ErrorMsg[] =
{
	"Expected number value!",
	"Expected end of line!",
	"Expected identifier!",
	"Expected segment usage statement!",
	"Expected type specifier!",
	"Identifers are unmatched!",
};

//
// void Error(INPUT *Input, int ErrCode)
//
// Description
//
// Parameters
//
// Returns
//
void Error(INPUT *Input, int ErrCode)
{
	printf("Error! %s\n", ErrorMsg[ErrCode]);
//	NumErrors++;
};
