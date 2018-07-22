// ------------------------------------------------------------
// Standard Argument
//		Definitions for accessing parameters in functions that accept
//	 	a variable number of arguments.
// Copyright 1997 (c) Leprechaun Software. All Rights Reserved.
// ------------------------------------------------------------
#define va_list	unsigned long *
#define va_start	(ap, arg)	ap =& arg	// ap now points to last named arg
#define va_arg		(ap, type)	ap -= 4		// next arg, then assign it
#define va_end		(ap)
