//
// Millennium Operating System Assembler
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
#include <stdio.h>
#include <string.h>
#include "input.h"

//
// Variables
//
char	eofChar = 0x7F;

// ------------------------------------------------------------
// FILEINPUT::FILEINPUT(char *Filename)
// ------------------------------------------------------------
// Description
//  Input class constructor
//
// Parameters
//  Filename	- Name of file to be opened
//
// Returns
//  If file exists ? 1 : 0
// ------------------------------------------------------------
FILEINPUT::FILEINPUT(char *Filename)
{
	char	Buff[256];

	// Open file
	Input		= fopen(Filename, "rt");
	strcpy(this->Filename, Filename);
	CurrLine	= 0;

	if(!Input)
	{
	}
	else
		GetLine();
};

// ------------------------------------------------------------
// FILEINPUT::~FILEINPUT()
// ------------------------------------------------------------
// Description
//  Input class destructor
//
// Parameters
//  None
//
// Returns
//  Nothing
// ------------------------------------------------------------
FILEINPUT::~FILEINPUT()
{
	if(Input)
		fclose(Input);
};

// ------------------------------------------------------------
// char *FILEINPUT::Print()
// ------------------------------------------------------------
// Description
//  Prints a descriptive text about the input
//
// Parameters
//  None
//
// Returns
//  Pointer to string
// ------------------------------------------------------------
char *FILEINPUT::Print()
{
	char	Buff[256];

	sprintf(Buff, "File:\t%s\nLine:\t%s", Filename, CurrLine);

	return Buff;
};

// ------------------------------------------------------------
// int FILEINPUT::GetLine()
// ------------------------------------------------------------
// Description
//  Get the next consecutive line from input
//
// Parameters
//  None
//
// Returns
//  First character
// ------------------------------------------------------------
char FILEINPUT::GetLine()
{
	if(Input)
	{
		if(fgets(Text, sizeof(Text), Input) != NULL)
		{
			pText = Text;
			CurrLine++;
		}
		else
			pText = &eofChar;

		return *pText;
	}
};

// ------------------------------------------------------------
// char FILEINPUT::GetChar()
// ------------------------------------------------------------
// Description
//  Gets the next consecutive character.
//
// Parameters
//  None
//
// Returns
//  Character read
// ------------------------------------------------------------
char FILEINPUT::GetChar()
{
	if(Input)
	{
		if(*pText == eofChar)
			return eofChar;
		else if(*pText == '\0')
			return GetLine();
		else
			return *++pText;
	}
	else
		return eofChar;
};

// ------------------------------------------------------------
// char FILEINPUT::Char()
// ------------------------------------------------------------
// Description
//  Returns the current character.
//
// Parameters
//  None
//
// Returns
//  Current character
// ------------------------------------------------------------
char FILEINPUT::Char()
{
	if(Input)
		return *pText;
	else
		return eofChar;
};

// ------------------------------------------------------------
// STRINGINPUT::STRINGINPUT(char *String)
// ------------------------------------------------------------
// Description
//  Input class constructor
//
// Parameters
//  String		- Input string
//
// Returns
//  If file exists ? 1 : 0
// ------------------------------------------------------------
STRINGINPUT::STRINGINPUT(char *String)
{
	strcpy(Text, String);
	pText = Text;
};

// ------------------------------------------------------------
// STRINGINPUT::~STRINGINPUT()
// ------------------------------------------------------------
// Description
//  Input class destructor
//
// Parameters
//  None
//
// Returns
//  Nothing
// ------------------------------------------------------------
STRINGINPUT::~STRINGINPUT()
{
};

// ------------------------------------------------------------
// char *STRINGINPUT::Print()
// ------------------------------------------------------------
// Description
//  Prints a descriptive text about the input
//
// Parameters
//  None
//
// Returns
//  Pointer to string
// ------------------------------------------------------------
char *STRINGINPUT::Print()
{
	return "";
};

// ------------------------------------------------------------
// char STRINGINPUT::GetChar()
// ------------------------------------------------------------
// Description
//  Gets the next consecutive character.
//
// Parameters
//  None
//
// Returns
//  Character read
// ------------------------------------------------------------
char STRINGINPUT::GetChar()
{
	if(*pText != '\0')
		return *++pText;
	else
		return eofChar;
};

// ------------------------------------------------------------
// char STRINGINPUT::Char()
// ------------------------------------------------------------
// Description
//  Returns the current character.
//
// Parameters
//  None
//
// Returns
//  Current character
// ------------------------------------------------------------
char STRINGINPUT::Char()
{
	return *pText;
};
