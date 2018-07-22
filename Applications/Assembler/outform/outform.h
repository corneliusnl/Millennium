#ifndef MOSA_OUTFORM_H
#define MOSA_OUTFORM_H
//
// Millennium Operating System Assembler
// Copyright 1997-1998 (c) Leprechaun Software.  All Rights Reserved.
//

//
// Defines
//
#define OUT_BINARY	0
#define OUT_AOUT	1
#define OUT_ELF		2

//
// Outform Class
//
class OUTFORM
{
private:
public:
	char	ShortName[10];
	char	Description[256];

	// Class construction/destruction
	OUTFORM();
	~OUTFORM();

	// Methods
	virtual void	Output() = 0;
};

#endif
