#ifndef MOSA_PARSER_H
#define MOSA_PARSER_H
//
// Millennium Operating System Assembler
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
class SYMTAB;
class SYMBOL;
class TYPETAB;
class TYPE;
class INPUT;
class TOKEN;
class ASSEMBLER;

//
// Defines
//
#define P_PREPROCESS	0
#define P_ASSEMBLE	1

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
	errExpectedRSB,
	errExpectedComma,
};

//
// Parser class
//
class PARSER
{
private:
	ASSEMBLER	*Assembler;
	TYPE	*db, *dw, *dd;

public:
	SYMTAB	*SymTab;
	TYPETAB	*TypeTab;
	INPUT	*Input;
	TOKEN	*Token;
	int	NumWarnings, NumErrors;

	// Class construction/destruction
	PARSER(ASSEMBLER *Assembler, char *Filename);
	~PARSER();

	// Parsers
	void	Parse();
	void	ParseIdentifier();
	void	ParseInclude();
	void	ParseOrigin();
	void	ParseStructure();
	void	ParseSegment();
	void	ParseProcedure();
	void	ParseEQU(SYMBOL *Symbol);
	void	ParseDeclaration(SYMBOL *Symbol);
	void	ParseInstruction();
	
	// Error reporting
	void	Warning(int WarnCode);
	void	Error(int ErrCode);
};

#endif
