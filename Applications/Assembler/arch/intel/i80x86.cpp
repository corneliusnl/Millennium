//
// Millennium Operating System Assembler
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
#include "i80x86.h"

//
// int I80X86::LookupSpecial(char *Name)
//
// Description
//
// Parameters
//
// Returns
//
int I80X86::LookupSpecial(char *Name)
{
};

//
// int I80X86::LookupRegister(char *Name)
//
// Description
//
// Parameters
//
// Returns
//
int I80X86::LookupRegister(char *Name)
{
	for(int i = 0; i < ; i++)
		if(strcmp(Name, Register[i].Name))
			return i;
};

//
// int I80X86::LookupInstruction(char *Name)
//
// Description
//
// Parameters
//
// Returns
//
int I80X86::LookupInstruction(char *Name)
{
	for(int i = 0; i < ; i++)
		if(strcmp(Name, Instruction[i].Name))
			return i;
};

//
// int I80X86::ParseSpecial()
//
// Description
//
// Parameters
//
// Returns
//
void I80X86::ParseSpecial()
{
};

//
// int I80X86::ParseInstruction()
//
// Description
//
// Parameters
//
// Returns
//
int I80X86::ParseInstruction()
{
	Ins.OpCodeIdx	= Token->Value.Number;

	for(int i = 0; i < 3; i++)
	{
		Token->Get();

		//
		// End of line
		//
		if(Token->Code == TOK_EOL)
			break;

		//
		// Expect operands to be comma separated
		//
		else if((i > 0) && (Token->Code != TOK_COMMA))
			Error(errExpectedComma);

		//
		// Skip over comma
		//
		else if((i > 0) && (Token->Code == TOK_COMMA))
			Token->Get():

		//
		// Memory reference
		//
		else if(Token->Code == TOK_LSB)
		{
			Ins.Operand[i].Type |= MEM;
			Token->Get();
		}

		//
		// Register operand
		//
		else if(Token->Code == TOK_REGISTER)
		{
			Ins.Operand[i].Type |= Register[Token->Value.Number];
		}

		//
		// Immediate operand
		//
		else if(Token->Code == TOK_NUMBER)
		{
			Ins.Operand[i].Type |= IMM;
		}

		//
		// Symbol
		//
		else if(Token->Code == TOK_SYMBOL)
		{
		}

		//
		// Expect terminating ']'
		//
		if(Ins.Operand[i].Type & MEM)
		{
			Token->Get(TOK_RSB, errExpectedRSB);
		}
	}
};
