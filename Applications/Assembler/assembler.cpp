//
// Millenium Operating System
// Copyright 1997-1998 (c) Leprechaun Software.  All Rights Reserved.
//
#include "assembler.h"
#include "parser.h"
#include "outform/outform.h"
#include "mosa.h"
#include "symbols.h"
#include "types.h"

//
// ASSEMBLER::ASSEMBLER()
//
// Description
//
// Parameters
//
// Returns
//
ASSEMBLER::ASSEMBLER()
{
	Parser	= NULL;
	OutForm = NULL;	
};

//
// ASSEMBLER::~ASSEMBLER()
//
// Description
//
// Parameters
//
// Returns
//
ASSEMBLER::~ASSEMBLER()
{
	if(Parser)
		delete	Parser;

	if(OutForm)
		delete	OutForm;
};

//
// int ASSEMBLER::Go(long Options)
//
// Description
//
// Parameters
//
// Returns
//
int ASSEMBLER::Go(long Options)
{
	Parser->Parse();
//	Parser->Parse();

#ifdef DEBUG
	Parser->TypeTab->Dump();
	Parser->SymTab->Dump();
#endif
};

//
// int ASSEMBLER::Open(char *Filename)
//
// Description
//
// Parameters
//
// Returns
//
int ASSEMBLER::Open(char *Filename)
{
	Parser = new PARSER(this, Filename);
};

//
// int ASSEMBLER::SetOutputFormat(int Outform)
//
// Description
//
// Parameters
//
// Returns
//
int ASSEMBLER::SetOutputFormat(int Outform)
{
	switch(Outform)
	{
/*		case OF_BINARY:
			OutFormat = new BINARY();
			break;

		case OF_ELF:
			OutFormat = new ELF();
			break;*/
	}
};

