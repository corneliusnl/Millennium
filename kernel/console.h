#ifndef KERNEL_CONSOLE_H
#define KERNEL_CONSOLE_H
//
// Console Support
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
// Authors:
//  Jeremiah Cornelious McCarthy
//

//
// Console Class
//
class TConsole
{
private:
	long	CurPos;

public:

	TConsole();
   ~TConsole();

   void	SetPosition(int X, int Y);
   void	GetPosition(int X, int Y);

   void	Clear();
   void	PutChar(char Char);
   void	UnGetChar();
   void	Printf();
};

#endif