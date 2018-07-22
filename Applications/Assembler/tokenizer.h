#ifndef MOSA_TOKENIZER_H
#define MOSA_TOKENIZER_H
//
// Tokenzier
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
class SYMBOL;
class INPUT;
class PARSER;
class TYPE;
class ASSEMBLER;

#define IsDigit(A)	((((A) >= '0') && ((A) <= '9')) ? 1 : 0)
#define IsAlpha(A)	(((((A) >= 'a') && ((A) <= 'z')) || (((A) >= 'A') && ((A) <= 'Z'))) ? 1 : 0)
#define IsSpace(A)	((((A) == ' ') || ((A) == '\t')) ? 1 : 0)
#define NumValue(A)	((A) - '0')

//
// Token types
//
enum 
{
	TOK_P186,		TOK_P286,
	TOK_P386,		TOK_P486,
	TOK_P586,		TOK_8086,
	TOK_PROC,		TOK_ENDP,
	TOK_STRUC,		TOK_ENDS,
	TOK_RADIX,		TOK_ARG,
	TOK_ALIGN,		TOK_INCLUDE,
	TOK_EQU,
	TOK_EXTERN,		TOK_GLOBAL,
	TOK_LOCAL,		TOK_PUBLIC,
	TOK_SEGMENT,		TOK_ORG,
	TOK_USE16,		TOK_USE32,

	TOK_DIVIDER,

	TOK_NULL,		TOK_COLON,
 	TOK_IDENTIFIER,		TOK_REGISTER,
	TOK_INSTRUCTION,	TOK_NUMBER,
	TOK_DOLLAR,		TOK_PERIOD,
	TOK_LSB,		TOK_RSB,
	TOK_PERCENT,		TOK_LPAREN,
	TOK_RPAREN,		TOK_COMMA,
	TOK_EOF,		TOK_TYPE,
	TOK_STRING,		TOK_EOL,
	TOK_LAB,		TOK_RAB,
	TOK_SYMBOL,		TOK_SPECIAL,
};

//
// Value
//
typedef struct
{
	SYMBOL	*Symbol;
	TYPE	*Type;
	char	*String;
	float	Number;
} VALUE;

//
// Tokenizer Class
//
class TOKEN
{
private:
	ASSEMBLER	*Assembler;
	INPUT		*Input;

public:
	VALUE	Value;
	int	Code;

	// Constructor/Destructor
	TOKEN(INPUT *Input, ASSEMBLER *Assembler);
	~TOKEN();

	// Get Token Routines
	int	Get();
	int	Get(int TokenCode, int ErrorCode);
};

#endif
