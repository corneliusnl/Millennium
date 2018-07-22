#ifndef MOSA_ARCHITECTURE_H
#define MOSA_ARCHITECTURE_H
//
// Millennium Operating System Assembler
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//

//
// Architecture Class
//
class ARCHITECTURE
{
private:
public:

	// Class construction/destruction
	ARCHITECTURE();
	~ARCHITECTURE();

	virtual int	LookupRegister(char *Name) = 0;
	virtual int	LookupSpecial(char *Name) = 0;
	virtual int	LookupInstruction(char *Name) = 0;
	virtual void	ParseInstruction() = 0;
	virtual void	ParseSpecial() = 0;
};

#endif
