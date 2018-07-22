//
// Millenium Operating System Assembler
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "assembler.h"
#include "outform/outform.h"
#include "mosa.h"

//
// Variables
//
ASSEMBLER	*Assembler;
int		OutType = OUT_BINARY;

//
// Local Function Prototypes
//
void	PrintArguments(void);

// ------------------------------------------------------------
// int Main(int argc, char *argv[])
// ------------------------------------------------------------
// Description
//  Entry point for the OMEGOS assembler.  Will parse command
// line commands and attempt to perform the requested actions.
//
// Parameters
//  argc			- Argument count
//  argv			- Argument array
//
// Returns
//  Nothing
// ------------------------------------------------------------
int main(int argc, char *argv[])
{
	extern char	*optarg;
	extern int	optind;
	char		optch;

	Assembler = new ASSEMBLER();

	//
	// Parse command line options
	//
	for(int i = 1; i < argc; i++)
	{
		//
		// Switch
		//
		if(argv[i][0] == '-')
		{
			//
			// Define
			// -dSYMBOL=VALUE
			//
			if(tolower(argv[i][1]) == 'd')
			{
			}

			//
			// Print Help
			// -H or -?
			//
			else if((tolower(argv[i][1]) == 'h') ||
				(tolower(argv[i][1]) == '?'))
			{
				PrintArguments();
				exit(EXIT_SUCCESS);
			}

			//
			// Version Information
			// -V
			//
			else if(tolower(argv[i][1]) == 'v')
			{
				printf("Millennium Assembler Version %s\n", MOSA_VER);
				exit(EXIT_SUCCESS);
			}

			//
			// Generate source code listng
			// -L
			//
			else if(tolower(argv[i][1]) == 'l')
			{
			}

			//
			// Invalid Command Line Argument
			//
			else
			{
				fprintf(stderr, "Invalid command line argument\n");
				exit(1);
			}
		}

		//
		// Filename
		//
		else
		{
			Assembler->Open(argv[i]);
			Assembler->Go(ASM_LIST);
		}
	}

	delete Assembler;

	exit(EXIT_SUCCESS);
};

// ------------------------------------------------------------
// void PrintArguments(void)
// ------------------------------------------------------------
// Description
//  Prints the command line options recognized by the
// millennium assembler.
//
// Parameters
//  None
//
// Returns
//  Nothing
// ------------------------------------------------------------
void PrintArguments(void)
{
	printf("MOSA %s - Millennium OS Assembler\n", MOSA_VER);
	printf("Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.\n");
	printf("\n");
	printf("Syntax    : MOSA [options] INFILE\n");
	printf("\n");
	printf("-d        : Define SYMBOL to be of VALUE\n");
	printf("-v        : Version information\n");
	printf("-a        : Selects target architecture (see below)\n");
	printf("-o        : Selects output format (see below)\n");
	printf("-h or ?   : Print this screen\n");
	printf("-l        : Generate source code listing\n");
	printf("-INFILE   : Assembly filename\n");
	printf("-o FORMAT : Set output format to one of the following:\n");
	printf("\n");
	printf("Available architectures:\n");
	printf("80x86	- Intel 80x86 compatible architecture\n");
	printf("\n");
	printf("Available output formats:\n");
	printf("binary	- Flat form binary output\n");
	printf("aout	- Aout object file format\n");
};
