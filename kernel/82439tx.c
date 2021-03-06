//
// Intel 82439TX PCISet
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
// Authors:
//  Jeremiah Cornelious McCarthy
//
// References:
//  Intel Platform Components 1998 (order number: 296467-009)
//

//
//
//
int MTXC_Probe()
{
	char	Config[256];
   int	*P;

   P = (int *)&Config;
	for(int i = 0; i < 8; i++)
   {
	   out(0xCF8, );
      *P++ = in(0xCFC);
   }

   printf("%d\n", Config[0]);
   printf("%d\n", Config[1]);

	// Check vendor and device identification for MTXC
   if((Config[0] == 0x86) && (Config[1] == 0x80))
   	if((Config[2] == 0x00) && (Config[3] == 0x71))
			return 1;

   return 0;
};
