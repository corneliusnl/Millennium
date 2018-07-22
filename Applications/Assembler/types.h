#ifndef MOSA_TYPE_H
#define MOSA_TYPE_H
//
// Millennium Operating System Assembler
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//

//
// Types
//
enum TypeCode
{
	TYP_DB,		TYP_DW,		TYP_DD,
	TYP_STRUCT,
};

//
// Type class
//
class TYPE
{
private:
public:
	TYPE		*Next;
	TypeCode	Type;
	char		*Name;

	// Class construction/destruction
	TYPE(const char *Name);
	~TYPE();

	void	SetType(TypeCode Type);
};

//
// Type table class
//
class TYPETAB
{
private:
public:
	TYPE	*Type;
	
	// Class construction/destruction
	TYPETAB();
	~TYPETAB();

	TYPE	*Enter(const char *Name);
	TYPE	*Lookup(const char *Name);

#ifdef DEBUG
	void	Dump();
#endif
};
	
#endif
