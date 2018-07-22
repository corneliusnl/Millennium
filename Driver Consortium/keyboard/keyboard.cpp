//
// Millennium Keyboard Subsystem
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//

char	KeyBuff[64];
int	KeyCount;

bool Shift;
bool Control;
bool Alt;
bool Caps;
bool Num;

char NormalTable[] =
{
	// Symbols & Numbers
   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
   '`', '-', '=', '[', ']', ';', ',', '.', '/', '\',

   // Characters
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
   'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',

   // 12 Function Keys
   '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
};

char ShiftTable[] =
{
	// Symbols & Numbers
   '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
   '~', '_', '+', '{', '}', ':', '<', '>', '?', '|',

   // Characters
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
   'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',

   // 12 Function Keys
   '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0',
};

//
// Description:
//  Keyboard Interrupt Service Routine (ISR)... upon
//  receiving keyboard interrupt the waiting byte will
//  be read from the keyboard controller and queued.
//
void interrupt KB_Isr()
{
	unsigned char Key;

	outb(0x20, 0x20);
   Key = inb(0x60);

   if(Key == SC_SHIFT)
   {
   	Shift = true;
      break;
   }
};

//
// Description:
//  Fetches and returns a character from the
//  keyboard queue buffer.
//
char KB_GetChar()
{
};

//
// Description:
//  Resets the keyboard buffer.
//
void KB_Flush()
{
	memset(KeyBuff, 0, sizeof(KeyBuff));
};