//
// Millennium Operating System Assembler
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
#include <string.h>
#include "mosa.h"
#include "types.h"

//
// TYPE::TYPE(const char *Name)
//
// Description
//
// Parameters
//
// Returns
//
TYPE::TYPE(const char *Name)
{
	this->Name	= strdup(Name);
	Next		= NULL;
};

//
// TYPE::~TYPE()
//
// Description
//
// Parameters
//
// Returns
//
TYPE::~TYPE()
{
	delete[]	Name;
	if(Next)
		delete	Next;
};

//
// void TYPE::SetType(TypeCode Type)
//
// Description
//
// Parameters
//
// Returns
//
void TYPE::SetType(TypeCode Type)
{
	this->Type = Type;
};

//
// TYPETAB::TYPETAB()
//
// Description
//
// Parameters
//
// Returns
//
TYPETAB::TYPETAB()
{
	Type = NULL;
};

//
// TYPETAB::~TYPETAB()
//
// Description
//
// Parameters
//
// Returns
//
TYPETAB::~TYPETAB()
{
	if(Type)
		delete Type;
};

//
// TYPE *TYPETAB::Enter(const char *Name)
//
// Description
//
// Parameters
//
// Returns
//
TYPE *TYPETAB::Enter(const char *Name)
{
	TYPE	*TmpType;

	TmpType		= new TYPE(Name);
	TmpType->Next	= Type;
	Type		= TmpType;

	return TmpType;
};

//
// TYPE *TYPETAB::Lookup(const char *Name)
//
// Description
//
// Parameters
//
// Returns
//
TYPE *TYPETAB::Lookup(const char *Name)
{
	TYPE	*TmpType;
	
	for(TmpType = Type; TmpType; TmpType = TmpType->Next)
		if(strcmp(Name, TmpType->Name) == 0)
			return TmpType;

	return NULL;
};

#ifdef DEBUG

//
// void TYPETAB::Dump()
//
// Description
//
// Parameters
//
// Returns
//
void TYPETAB::Dump()
{
	TYPE	*TmpType;
	
	printf("\n");
	printf("Types\n");
	printf("\n");
	
	for(TmpType = Type; TmpType; TmpType = TmpType->Next)
		printf("%s\n", TmpType->Name);
};

#endif
