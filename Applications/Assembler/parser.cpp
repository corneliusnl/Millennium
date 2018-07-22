//
// Millennium Operating System Assembler
// Copyright 1997-1998 (c) Leprechaun Software.  All Rights Reserved.
//
#include "parser.h"
#include "assembler.h"
#include "input.h"
#include "tokenizer.h"
#include "symbols.h"
#include "types.h"

//
// PARSER::PARSER(ASSEMBLER *Assembler, char *Filename)
//
// Description
//
// Parameters
//
// Returns
//
PARSER::PARSER(ASSEMBLER *Assembler, char *Filename)
{
	this->Assembler	= Assembler;
	Input	= new FILEINPUT(Filename);
	Token	= new TOKEN(Input, Assembler);
	SymTab	= new SYMTAB();
	TypeTab	= new TYPETAB();
	
	// Enter predefined types
	db	= TypeTab->Enter("db");
	dw	= TypeTab->Enter("dw");
	dd	= TypeTab->Enter("dd"); 
};

//
// PARSER::~PARSER()
//
// Description
//
// Parameters
//
// Returns
//
PARSER::~PARSER()
{
	delete	TypeTab;
	delete	SymTab;
	delete	Input;
	delete	Token;
};

//
// void PARSER::Parse()
//
// Description
//
// Parameters
//
// Returns
//
void PARSER::Parse()
{
	while(Token->Get() != TOK_EOF)
	{
		switch(Token->Code)
		{
			case TOK_IDENTIFIER:
				ParseIdentifier();
				break;

			case TOK_STRUC:
				ParseStructure();
				break;

			case TOK_SEGMENT:
				ParseSegment();
				break;
				
			case TOK_PROC:
				ParseProcedure();
				break;

			case TOK_ORG:
				ParseOrigin();
				break;

			case TOK_INCLUDE:
				ParseInclude();
				break;

			case TOK_ENDS:
			case TOK_ENDP:
				return;

			case TOK_INSTRUCTION:
				ParseInstruction();
				break;
		}
	}
};

//
// void PARSER::ParseInclude()
//
// Description
//
// Parameters
//
// Returns
//
void PARSER::ParseInclude()
{
};

//
// void PARSER::ParseProcedure()
//
// Description
//
// Parameters
//
// Returns
//
void PARSER::ParseProcedure()
{
	SYMBOL	*TmpSym;
	
	Token->Get(TOK_IDENTIFIER, errExpectedIdentifier);
	TmpSym = SymTab->Enter(Token->Value.String);
	
	Token->Get(TOK_EOL, errExpectedEOL);

	Parse();

	Token->Get();
	if(Token->Code == TOK_SYMBOL)
	{
		if(TmpSym != Token->Value.Symbol)
			Error(errIdentifiersUnmatched);
		Token->Get(TOK_EOL, errExpectedEOL);
	}
	else if(Token->Code != TOK_EOL)
		Error(errExpectedEOL);
};

//
// void PARSER::ParseSegment()
//
// Description
//
// Parameters
//
// Returns
//
void PARSER::ParseSegment()
{
	SYMBOL	*TmpSym;
	
	Token->Get(TOK_IDENTIFIER, errExpectedIdentifier);
	TmpSym		= SymTab->Enter(Token->Value.String);
	TmpSym->Type	= SYM_SEGMENT;
	
	switch(Token->Get())
	{
		case TOK_USE16:
			break;

		case TOK_USE32:
			break;

		default:
			Error(errExpectedUsage);
			break;
	}
	Token->Get(TOK_EOL, errExpectedEOL);

	Parse();

	Token->Get();
	if(Token->Code == TOK_SYMBOL)
	{
		if(TmpSym != Token->Value.Symbol)
			Error(errIdentifiersUnmatched);
		Token->Get(TOK_EOL, errExpectedEOL);
	}
	else if(Token->Code != TOK_EOL)
		Error(errExpectedEOL);
};

//
// void PARSER::ParseStructure()
//
// Description
//
// Parameters
//
// Returns
//
void PARSER::ParseStructure()
{
//	SYMBOL	*TmpSym;
	TYPE	*TmpType;
//	SYMTAB	*TmpTable;

//	TmpTable = new SYMTAB();
	
	Token->Get(TOK_IDENTIFIER, errExpectedIdentifier);
	TmpType = TypeTab->Enter(Token->Value.String);

	Token->Get(TOK_EOL, errExpectedEOL);

	while(Token->Code != TOK_EOF)
	{
		Token->Get();
		if(Token->Code == TOK_ENDS)
			break;
		else if(Token->Code != TOK_IDENTIFIER)
			Error(errExpectedIdentifier);
//		TmpSym = new SYMBOL(Token->Code.String);
//		TmpTable->Enter(TmpSym);
			
		Token->Get(TOK_TYPE, errExpectedType);
		Token->Get();
		Token->Get(TOK_EOL, errExpectedEOL);
	}

//	TmpType->Struct.SymTab = TmpTable;
	
	Token->Get();
	if(Token->Code == TOK_TYPE)
	{
		if(TmpType != Token->Value.Type)
			Error(errIdentifiersUnmatched);
		Token->Get(TOK_EOL, errExpectedEOL);
	}
	else if(Token->Code != TOK_EOL)
		Error(errExpectedEOL);
};

//
// void PARSER::ParseInstruction()
//
// Description
//
// Parameters
//
// Returns
//
void PARSER::ParseInstruction()
{
};

//
// void PARSER::ParseIdentifier()
//
// Description
//
// Parameters
//
// Returns
//
void PARSER::ParseIdentifier()
{
	SYMBOL	*TmpSym;
	
	TmpSym = SymTab->Enter(Token->Value.String);
	TmpSym->Type = SYM_LABEL;
	
	switch(Token->Get())
	{
		case TOK_EQU:
			ParseEQU(TmpSym);
			break;

		case TOK_TYPE:
			ParseDeclaration(TmpSym);
			break;

		case TOK_COLON:
			TmpSym->Type = SYM_LABEL;
			return;
	}
};

//
// void PARSER::ParseOrigin()
//
// Description
//
// Parameters
//
// Returns
//
void PARSER::ParseOrigin()
{
	Token->Get(TOK_NUMBER, errExpectedNumber);
	Token->Get(TOK_EOL, errExpectedEOL);
};

//
// void PARSER::ParseEQU(SYMBOL *Symbol)
//
// Description
//
// Parameters
//
// Returns
//
void PARSER::ParseEQU(SYMBOL *Symbol)
{
	Token->Get(TOK_NUMBER, errExpectedNumber);
//	Symbol->Equate.Value	= Token->Value.Number;

	Token->Get(TOK_EOL, errExpectedEOL);
};

//
// void PARSER::ParseDeclaration(SYMBOL *Symbol)
//
// Description
//
// Parameters
//
// Returns
//
void PARSER::ParseDeclaration(SYMBOL *Symbol)
{
	TYPE	*Type;

	Type = TypeTab->Lookup(Token->Value.String);
//	Symbol->Type = Type;
};

//
// void PARSER::Warning(int WarnCode)
//
// Description
//
// Parameters
//
// Returns
//
void PARSER::Warning(int WarnCode)
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
	"Expected right square bracket!",
	"Expected comma!",
};

//
// void PARSER::Error(int ErrCode)
//
// Description
//
// Parameters
//
// Returns
//
void PARSER::Error(int ErrCode)
{
	printf("Error! %s\n", ErrorMsg[ErrCode]);
//	NumErrors++;
};
