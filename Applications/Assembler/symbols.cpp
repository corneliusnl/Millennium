//
// Millennium Operating System Assembler
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
#include <string.h>
#include "mosa.h"
#include "symbols.h"

//
// SYMBOL::SYMBOL(const char *Name)
//
// Description
//
// Parameters
//
// Returns
//
SYMBOL::SYMBOL(const char *Name)
{
	this->Name	= strdup(Name);
	Next		= NULL;
};

//
// SYMBOL::~SYMBOL()
//
// Description
//
// Parameters
//
// Returns
//
SYMBOL::~SYMBOL()
{
	delete[]	Name;
	if(Next)
		delete Next;
};

//
// void SYMBOL::SetType(SymType Type)
//
// Description
//
// Parameters
//
// Returns
//
void SYMBOL::SetType(SymType Type)
{
	this->Type	= Type;
};

//
// SYMTAB::SYMTAB()
//
// Description
//
// Parameters
//
// Returns
//
SYMTAB::SYMTAB()
{
	Symbol = NULL;
};

//
// SYMTAB::~SYMTAB()
//
// Description
//
// Parameters
//
// Returns
//
SYMTAB::~SYMTAB()
{
	delete Symbol;
};

//
// SYMBOL *SYMTAB::Enter(const char *Name)
//
// Description
//
// Parameters
//
// Returns
//
SYMBOL *SYMTAB::Enter(const char *Name)
{
	SYMBOL	*TmpSym;

	TmpSym		= new SYMBOL(Name);
	TmpSym->Next	= Symbol;
	Symbol		= TmpSym;

	return TmpSym;
};

//
// SYMBOL *SYMTAB::Lookup(const char *Name)
//
// Description
//
// Parameters
//
// Returns
//
SYMBOL *SYMTAB::Lookup(const char *Name)
{
	SYMBOL *TmpSym;

	for(TmpSym = Symbol; TmpSym; TmpSym = TmpSym->Next)
		if(strcmp(Name, TmpSym->Name) == 0)
			return TmpSym;

	return NULL;
};

#ifdef DEBUG

//
// void SYMTAB::Dump()
//
// Description
//
// Parameters
//
// Returns
//
void SYMTAB::Dump()
{
	SYMBOL	*TmpSym;

	printf("\n");
	printf("%-30s %-10s %-20s\n", "Symbol Name", "Type", "Value");
	printf("\n");
	
	for(TmpSym = Symbol; TmpSym; TmpSym = TmpSym->Next)
	{
		switch(TmpSym->Type)
		{
			case SYM_EQUATE:
				printf("%-30s %-10s\n", TmpSym->Name, "equate");
				break;

			case SYM_SEGMENT:
				printf("%-30s %-10s\n", TmpSym->Name, "segment");
				break;

			case SYM_STRUCT:
				printf("%-30s %-10s\n", TmpSym->Name, "struct");
				break;

			case SYM_LABEL:
				printf("%-30s %-10s\n", TmpSym->Name, "label");
				break;
		}
		
	}
};

#endif
