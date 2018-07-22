//
// Millennium Operating System Assembler
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
#include <string.h>
#include <ctype.h>
#include "arch/architecture.h"
#include "tokenizer.h"
#include "input.h"
#include "parser.h"
#include "symbols.h"
#include "types.h"
#include "assembler.h"

//
// Tokens
//
char	*ResWord[] =
{
	"p186",			"p286",
	"p386",			"p486",
	"p586",			"p8086",
	"proc",			"endp",
	"struc",		"ends",
	"radix",		"arg",
	"align",		"include",
	"equ",
	"extern",		"global",
	"local",		"public",
	"segment",		"org",
	"use16",		"use32",
};

// ------------------------------------------------------------
// TOKEN::TOKEN(INPUT Input, ASSEMBLER *Assembler)
// ------------------------------------------------------------
// Description
//
// Parameters
//
// Returns
//  Nothing
// ------------------------------------------------------------
TOKEN::TOKEN(INPUT *Input, ASSEMBLER *Assembler)
{
	this->Assembler	= Assembler;
	this->Input	= Input;
};

// ------------------------------------------------------------
// TOKEN::~TOKEN()
// ------------------------------------------------------------
// Description
//
// Parameters
//
// Returns
//
// ------------------------------------------------------------
TOKEN::~TOKEN()
{
};

// ------------------------------------------------------------
// int TOKEN::Get()
// ------------------------------------------------------------
// Description
//  Reads a token.
//
// Parameters
//  None
//
// Returns
//  Token Type
// ------------------------------------------------------------
int TOKEN::Get()
{
	char	Tmp[256];
	char	ch, *T;
	int	i, Sign = 1, Radix;

	ch = Input->Char();

	// Skip Spaces
	while(IsSpace(ch))
		ch = Input->GetChar();

	// Newline characters
	while((ch == '\n') || (ch == '\r'))
		ch = Input->GetChar();

	// Reached end of text, attempt to continue
	// Assuming input will read another chunk of text in
	if(ch == '\0')
	{
		Input->GetChar();
		return(Code = TOK_EOL);
	}

	//
	// End of File
	//
	else if(ch == eofChar)
	{
		return(Code = TOK_EOF);
	}

	//
	// Identifiers, registers, types,...
	//
	else if(IsAlpha(ch))
	{
		T = Tmp;
		while(IsAlpha(ch) || IsDigit(ch) || (ch == '_'))
		{
			*T++	= tolower(ch);
			ch	= Input->GetChar();
		}
		*T = '\0';
		Value.String = strdup(Tmp);

		//
		// Reserved word
		//
		for(i = 0; i < TOK_DIVIDER; i++)
			if(strcmp(ResWord[i], Tmp) == 0)
			{
				return(Code = i);
			}

		//
		// Type
		//
		if(Value.Type = Assembler->Parser->TypeTab->Lookup(Tmp))
			return(Code = TOK_TYPE);

		//
		// Symbol
		//
		if(Value.Symbol = Assembler->Parser->SymTab->Lookup(Tmp))
			return(Code = TOK_SYMBOL);

		//
		// Instruction
		//
		if(Assembler->Arch->LookupInstruction(Tmp))
			return(Code = TOK_INSTRUCTION);
			
		//
		// Register
		//
		if(Assembler->Arch->LookupRegister(Tmp))
			return(Code = TOK_REGISTER);
		
		//
		// Special
		//
		if(Assembler->Arch->LookupSpecial(Tmp))
			return(Code = TOK_SPECIAL);
			
		return(Code = TOK_IDENTIFIER);
	}

	//
	// Quoted string
	//
	else if((ch == '\"') || (ch == '\''))
	{
		T	= Tmp;
		ch	= Input->GetChar();

		while((ch != '\"') && (ch != '\'') && (ch != '\0'))
		{
			*T++	= ch;
			ch	= Input->GetChar();
		}
		Input->GetChar();
		*T = '\0';

		strcpy(Value.String, Tmp);
		return(Code = TOK_STRING);
	}

	//
	// Number
	//
	else if((IsDigit(ch)) || (ch == '-'))
	{
		T	= Tmp;

		//
		// negative case
		//
		while((IsDigit(ch)) || (IsAlpha(ch)))
		{
			*T++	= tolower(ch);
			ch	= Input->GetChar();
		}
		*T	= '\0';
		T	= Tmp;

		//
		// Hexadecimal
		//
		if((Tmp[0] == '0') && ((Tmp[1] == 'x') || (Tmp[1] == 'X')))
		{
			Radix	= 16;
			T	+= 2;
		}

		//
		// Hexidecimal
		//
		else if(Tmp[strlen(Tmp)-1] == 'h')
		{
			Radix		= 16;
			Tmp[strlen(Tmp)-1] = '\0';
		}

		//
		// Binary
		//
		else if(Tmp[strlen(Tmp)-1] == 'b')
		{
			Radix		= 2;
			Tmp[strlen(Tmp)-1] = '\0';
		}

		//
		// Decimal
		//
		else
		{
			Radix		= 10;
		}

		Value.Number = 0;
		while(*T != '\0')
		{
			Value.Number = (Radix * Value.Number) + NumValue(*T);
			T++;
		}

		Value.Number *= Sign;
		return(Code = TOK_NUMBER);
	}

	//
	// Comment
	//
	else if(ch == ';')
	{
		while(Input->GetChar() != '\0');
		Input->GetChar();
		return(Get());
	}

	//
	// Colon
	//
	else if(ch == ':')
	{
		Input->GetChar();
		return(Code = TOK_COLON);
	}

	//
	// Left Parenthesis
	//
	else if(ch == '(')
	{
		Input->GetChar();
		return(Code = TOK_LPAREN);
	}

	//
	// Right Parenthesis
	//
	else if(ch == ')')
	{
		Input->GetChar();
		return(Code = TOK_RPAREN);
	}

	//
	// Left square bracket
	//
	else if(ch == '[')
	{
		Input->GetChar();
		return(Code = TOK_LSB);
	}

	//
	// Right square bracket
	//
	else if(ch == ']')
	{
		Input->GetChar();
		return(Code = TOK_RSB);
	}

	//
	// Left angle bracket
	//
	else if(ch == '<')
	{
		Input->GetChar();
		return(Code = TOK_LAB);
	}

	//
	// Right angle bracket
	//
	else if(ch == '>')
	{
		Input->GetChar();
		return(Code = TOK_RAB);
	}

	//
	// Comma
	//
	else if(ch == ',')
	{
		Input->GetChar();
		return(Code = TOK_COMMA);
	}

	//
	// Dollar sign
	//
	else if(ch == '$')
	{
		Input->GetChar();
		return(Code = TOK_DOLLAR);
	}

	//
	// Invalid Token
	//
	else
	{
		return(Code = TOK_NULL);
	}
};

// ------------------------------------------------------------
// TOKEN::Get(int TokCode)
// ------------------------------------------------------------
// Description
//  Gets the subsequent token, however, it this isn't the
// tokem that was expected it prints a error message.
//
// Parameters
//  TokCode			- Token code that is expected
//
// Returns
//  If successful ? 1 : 0
// ------------------------------------------------------------
int TOKEN::Get(int TokCode, int ErrCode)
{
	if(Get() == TokCode)
		return Code;
	else
		Assembler->Parser->Error(ErrCode);

	return TOK_NULL;
};
