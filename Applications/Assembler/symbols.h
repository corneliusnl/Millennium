#ifndef MOSA_SYMBOLS_H
#define MOSA_SYMBOLS_H
//
// Millennium Operating System Assembler
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//  
#include "types.h"

//
// Symbol types
//
enum SymType
{
	SYM_EQUATE,
	SYM_LABEL,
	SYM_SEGMENT,
	SYM_STRUCT,
};

//
// Symbol class
//
class SYMBOL
{
private:
public:
	SYMBOL	*Next;
	SymType	Type;
	char	*Name;

	union
	{
		//
		// Equate
		//
		struct
		{
			long	Value;
		} Equate;
		
		//
		// Label
		// 
		struct
		{
			long	Segment;
			long	Offset;
			TYPE	*Type;		// optional
		} Label;

		//
		// Structure
		// 
		struct
		{
			long	Segment;
			long	Offset;
			TYPE	*Type;
		} Struct;
	};
	
	// Class construction/destruction
	SYMBOL(const char *Name);
	~SYMBOL();

	void	SetType(SymType Type);
};

//
// Symbol table class
//
class SYMTAB
{	
private:
public:
	SYMBOL	*Symbol;

	// Class construction/destruction
	SYMTAB();
	~SYMTAB();

	// Management
	SYMBOL	*Enter(const char *Name);
	SYMBOL	*Lookup(const char *Name);
#ifdef	DEBUG
	void	Dump();
#endif
};

#endif
