//
// Millennium Operating System Assembler
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
#include "i80x86.h"

//
// Instruction set
//
I80X86::InsMap[] =
{
	{"aaa",		0,	{0, 0, 0},						"\x37", 0, I86},
	{"aad",		0,	{0, 0, 0},						"\xD5\x0A", 0, I86},
	{"aam",		0,	{0, 0, 0},						"\xD4\x0A", 0, I86},
	{"aas",		0,	{0, 0, 0},						"\x3F", 0, I86},

	{"adc",		2,	{AL_REG, IMM|BITS8, 0},	 				"\x14", 0, I86},
	{"adc",		2,	{AX_REG, IMM|BITS16, 0},  				"\x15", 0, I86},
	{"adc",		2,	{EAX_REG, IMM|BITS32, 0},		  		"\x15", 0, I86},
	{"adc",		2,	{REGMEM|BITS8, IMM|BITS8, 0},	  			"\x80", D2|MODRM, I86},
	{"adc",		2,	{REGMEM|BITS16, IMM|BITS16, 0}, 			"\x81", D2|MODRM, I86},
	{"adc",		2,	{REGMEM|BITS32, IMM|BITS32, 0}, 			"\x81", D2|MODRM, I86},
	{"adc",		2,	{REGMEM|BITS16, IMM|BITS8, 0},				"\x83", D2|MODRM, I86},
	{"adc",		2,	{REGMEM|BITS32, IMM|BITS8, 0},				"\x83", D2|MODRM, I86},
	{"adc",		2,	{REGMEM|BITS8, REG|BITS8, 0},				"\x10", R|MODRM, I86},
	{"adc",		2,	{REGMEM|BITS16, REG|BITS16,0},				"\x11", R|MODRM, I86},
	{"adc",		2,	{REGMEM|BITS32, REG|BITS32,0},				"\x11", R|MODRM, I86},
	{"adc",		2,	{REG|BITS8, REGMEM|BITS8, 0},				"\x12", R|MODRM, I86},
	{"adc",		2,	{REG|BITS16, REGMEM|BITS16, 0},				"\x13", R|MODRM, I86},
	{"adc",		2,	{REG|BITS32, REGMEM|BITS32, 0},				"\x13", R|MODRM, I86},

	{"add",		2,	{AL_REG, IMM|BITS8, 0},					"\x04", 0, I86},
	{"add",		2,	{AX_REG, IMM|BITS16, 0},				"\x05", 0, I86},
	{"add",		2,	{EAX_REG, IMM|BITS32, 0},				"\x05", 0, I86},
	{"add",		2,	{REGMEM|BITS8, IMM|BITS8, 0},				"\x80", D0, I86},
	{"add",		2,	{REGMEM|BITS16, IMM|BITS16, 0},				"\x81", D0, I86},
	{"add",		2,	{REGMEM|BITS32, IMM|BITS32, 0},				"\x81", D0, I86},
	{"add",		2,	{REGMEM|BITS16, IMM|BITS8, 0},				"\x83", D0, I86},
	{"add",		2,	{REGMEM|BITS32, IMM|BITS8, 0},				"\x83", D0, I86},
	{"add",		2,	{REGMEM|BITS8, REG|BITS8, 0},				"\x00", R|MODRM, I86},
	{"add",		2,	{REGMEM|BITS16, REG|BITS16, 0},				"\x01", R|MODRM, I86},
	{"add",		2,	{REGMEM|BITS32, REG|BITS32, 0},				"\x01", R|MODRM, I86},
	{"add",		2,	{REG|BITS8, REGMEM|BITS8, 0},				"\x02", R|MODRM, I86},
	{"add",		2,	{REG|BITS16, REGMEM|BITS16, 0},				"\x03", R|MODRM, I86},
	{"add",		2,	{REG|BITS32, REGMEM|BITS32, 0}, 			"\x03", R|MODRM, I86},

	{"and",		2,	{AL_REG, IMM|BITS8, 0},					"\x24", 0, I86},
	{"and",		2,	{AX_REG, IMM|BITS16, 0},				"\x25", 0, I86},
	{"and",		2,	{EAX_REG, IMM|BITS32, 0},				"\x25", 0, I86},
	{"and",		2,	{REGMEM|BITS8, IMM|BITS8, 0},				"\x80", D4, I86},
	{"and",		2,	{REGMEM|BITS16, IMM|BITS16, 0},				"\x81", D4, I86},
	{"and",		2,	{REGMEM|BITS32, IMM|BITS32, 0},				"\x81", D4, I86},
	{"and",		2,	{REGMEM|BITS16, IMM|BITS8, 0},				"\x83", D4, I86},
	{"and",		2,	{REGMEM|BITS32, IMM|BITS8, 0},				"\x83", D4, I86},
	{"and",		2,	{REGMEM|BITS8, REG|BITS8, 0},				"\x20", R|MODRM, I86},
	{"and",		2,	{REGMEM|BITS16, REG|BITS16, 0},				"\x21", R|MODRM, I86},
	{"and",		2,	{REGMEM|BITS32, REG|BITS32, 0},				"\x21", R|MODRM, I86},
	{"and",		2,	{REG|BITS8, REGMEM|BITS8, 0},				"\x22", R|MODRM, I86},
	{"and",		2,	{REG|BITS16, REGMEM|BITS16, 0},				"\x23", R|MODRM, I86},
	{"and",		2,	{REG|BITS32, REGMEM|BITS32, 0},				"\x23", R|MODRM, I86},

	{"arpl",	2,	{REGMEM|BITS16, REG|BITS16, 0},				"\x63", R|MODRM, I286},

	{"bound",	2,	{REG|BITS16, REGMEM|BITS16, 0},				"\x62", R|MODRM, I186},
	{"bound",	2,	{REG|BITS32, REGMEM|BITS32, 0},				"\x62", R|MODRM, I186},

	{"bsf",		2,	{REG|BITS16, REGMEM|BITS16, 0},				"\x0F\xBC", 0, I386},
	{"bsf",		2,	{REG|BITS32, REGMEM|BITS32, 0},				"\x0F\xBC", 0, I386},

	{"bsr",		2,	{REG|BITS16, REGMEM|BITS16, 0},				"\xOF\xBD", 0, I386},
	{"bsr",		2,	{REG|BITS32, REGMEM|BITS32, 0},				"\xOF\xBD", 0, I386},

	{"bswap",	1,	{REG|BITS32, 0, 0},					"\x0F\xC8", RD, I486},

	{"bt",		2,	{REGMEM|BITS16, REG|BITS16, 0},				"\x0F\xA3", 0, I386},
	{"bt",		2,	{REGMEM|BITS32, REG|BITS32, 0},				"\x0F\xA3", 0, I386},
	{"bt",		2,	{REGMEM|BITS16, IMM|BITS8, 0},				"\x0F\xBA", D4, I386},
	{"bt",		2,	{REGMEM|BITS32, IMM|BITS8, 0},				"\x0F\xBA", D4, I386},

	{"btc",		2,	{REGMEM|BITS16, REG|BITS16, 0},				"\x0F\xBB", 0, I386},
	{"btc",		2,	{REGMEM|BITS32, REG|BITS32, 0}, 			"\x0F\xBB", 0, I386},
	{"btc",		2,	{REGMEM|BITS16, IMM|BITS8, 0},				"\x0F\xBA", D7, I386},
	{"btc",		2,	{REGMEM|BITS32, IMM|BITS8, 0},				"\x0F\xBA", D7, I386},

	{"btr",		2,	{REGMEM|BITS16, REG|BITS16, 0},				"\x0F\xB3", 0, I386},
	{"btr",		2,	{REGMEM|BITS32, REG|BITS32, 0},				"\x0F\xB3", 0, I386},
	{"btr",		2,	{REGMEM|BITS16, IMM|BITS8, 0},				"\x0F\xBA", D6, I386},
	{"btr",		2,	{REGMEM|BITS32, IMM|BITS8, 0},				"\x0F\xBA", D6, I386},

	{"bts",		2,	{REGMEM|BITS16, REG|BITS16, 0},				"\x0F\xAB", 0, I386},
	{"bts",		2,	{REGMEM|BITS32, REG|BITS32, 0},				"\x0F\xBA", 0, I386},
	{"bts",		2,	{REGMEM|BITS16, IMM|BITS8, 0},				"\x0F\xBA", D5, I386},
	{"bts",		2,	{REGMEM|BITS32, IMM|BITS8, 0},				"\x0F\xBA", D5, I386},

//	{"call",	1,	{REL|BITS16, 0, 0},					"\xE8", 0, I586},
	{"call",	1,	{REGMEM|BITS16, 0, 0},					"\xFF", 0, I86},
	//
	// More Calls
	//

	{"cbw",		0,	{0, 0, 0},						"\x98", 0, I86},
	{"cwde",	0,	{0, 0, 0},						"\x98", 0, I386},

	{"cdq",		0,	{0, 0, 0},						"\x99", 0, I386},
	{"clc",		0,	{0, 0, 0},						"\xF8", 0, I86},
	{"cld",		0,	{0, 0, 0},						"\xFC", 0, I86},
	{"cli",		0,	{0, 0, 0},						"\xFA", 0, I86},
	{"clts",	0,	{0, 0, 0},						"\x0F\x06", 0, I286},
	{"cmc",		0,	{0, 0, 0},						"\xF5", 0, I86},

	{"cmp",		2,	{AL_REG, IMM|BITS8, 0},					"\x3C", 0, I86},
	{"cmp",		2,	{AX_REG, IMM|BITS16, 0},				"\x3D", 0, I86},
	{"cmp",		2,	{EAX_REG, IMM|BITS32, 0},				"\x3D", 0, I86},
	{"cmp",		2,	{REGMEM|BITS8, IMM|BITS8, 0},				"\x80", D7, I86},
	{"cmp",		2,	{REGMEM|BITS16, IMM|BITS16, 0},				"\x81", D7, I86},
	{"cmp",		2,	{REGMEM|BITS32, IMM|BITS32, 0},				"\x81", D7, I86},
	{"cmp",		2,	{REGMEM|BITS16, IMM|BITS8, 0},				"\x83", D7, I86},
	{"cmp",		2,	{REGMEM|BITS32, IMM|BITS8, 0},				"\x83", D7, I86},
	{"cmp",		2,	{REGMEM|BITS8, REG|BITS8, 0},				"\x38", R|MODRM, I86},
	{"cmp",		2,	{REGMEM|BITS16, REG|BITS16, 0},				"\x39", R|MODRM, I86},
	{"cmp",		2,	{REGMEM|BITS32, REG|BITS32, 0},				"\x39", R|MODRM, I86},
	{"cmp",		2,	{REG|BITS8, REGMEM|BITS8, 0},				"\x3A", R|MODRM, I86},
	{"cmp",		2,	{REG|BITS16, REGMEM|BITS16, 0},				"\x3B", R|MODRM, I86},
	{"cmp",		2,	{REG|BITS32, REGMEM|BITS32, 0},				"\x3B", R|MODRM, I86},

//	{"cmps",	2,	{MEM|BITS8, MEM|BITS8, 0},				"\xA6", 0, I586},
//	{"cmps",	2,	{MEM|BITS16, MEM|BITS16, 0},				"\xA7", 0, I586},
//	{"cmps",	2,	{MEM|BITS32, MEM|BITS32, 0},				"\xA7", 0, I586},
	{"cmpsb",	0,	{0, 0, 0},						"\xA6", 0, I86},
	{"cmpsw",	0,	{0, 0, 0},						"\xA7", 0, I86},
	{"cmpsd",	0,	{0, 0, 0},						"\xA7", 0, I386},
																		
//	{"cmpxchg",	2,	{REGMEM|BITS8, REG|BITS8, 0},				"\x0F\xB0", R|MODRM, I586},
//	{"cmpxchg",	2,	{REGMEM|BITS16, REG|BITS16, 0},				"\x0F\xB1", R|MODRM, I586},
//	{"cmpxchg",	2,	{REGMEM|BITS32, REG|BITS32, 0},				"\xOF\xB1", R|MODRM, I586},
																
//	{"cmpxchg8b",	1,	{REGMEM|BITS64, 0, 0},					"\x0F\xC7", D1|REGMEM|M64, I586},

	{"cpuid",	0,	{0, 0, 0},						"\x0F\xA2", 0, I586},
			
	{"cwd",		0,	{0, 0, 0},						"\x99", 0, I86},
							
	{"daa",		0,	{0, 0, 0},						"\x27", 0, I86},
																				
	{"das",		0,	{0, 0, 0},						"\x2F", 0, I86},
																								
	{"dec",		1,	{REGMEM|BITS8, 0, 0},					"\xFE", D1, I86},
	{"dec",		1,	{REGMEM|BITS16, 0, 0},					"\xFF", D1, I86},
	{"dec",		1,	{REGMEM|BITS32, 0, 0},					"\xFF", D1, I86},
	{"dec",		1,	{REG|BITS16, 0, 0},					"\x48", RW, I86},
	{"dec",		1,	{REG|BITS32, 0, 0},					"\x48", RD, I86},
																							
	{"div",		2,	{AL_REG, REGMEM|BITS8, 0},				"\xF6", D6, I86},
	{"div",		2,	{AX_REG, REGMEM|BITS16, 0},				"\xF7", D6, I86},
	{"div",		2,	{EAX_REG, 0, 0},					"\xF7", D6, I86},
																											
	//
	// More Enters
	//
	{"enter",	2,	{IMM|BITS16, IMM|BITS8, 0},			"\xC8", 0, I186},
																												
//	{"f2xm1"INS_F2XM1,	0,	{0, 0, 0},						"\xD9\xF0", 0, I586},

//	{"fabs"INS_FABS,	0,	{0, 0, 0},						"\xD9\xE1", 0, I586},
																													
	//
	// More Adds
	//
//	{"fadd"INS_FADD,	2,
///	{"faddp"INS_FADDP,
//	{"fiadd"INS_FIADD,	1,	{MEM|BITS32, 0, 0},					"\xDA", D0|REGMEM, I586},
//	{"fiadd"INS_FIADD,	1,	{MEM|BITS16, 0, 0},					"\xDE", D0|REGMEM, I586},
																					
//	{"fbld"INS_FBLD,	1,	{MEM|BITS80, 0, 0},					"\xDF", D4|REGMEM, I586},
																								
//	{"fbstp"INS_FBSTP,	1,	{MEM|BITS80, 0, 0},					"\xDF", D6|REGMEM, I586},
																													
//	{"fchs"INS_FCHS,	0,	{0, 0, 0},						"\xD9\xE0", 0, I586},

//	{"fclex"INS_FCLEX,	0,	{0, 0, 0},						"\x98\xDB\xE2", 0, I586},
//	{"fnclex"INS_FNCLEX,	0,	{0, 0, 0},						"\xDB\xE2", 0, I586},
															
//	{"fcom"INS_FCOM,	1,	{MEM|BITS32, 0, 0},					"\xD8", D2, I586},
//	{"fcom"INS_FCOM,	1,	{MEM|BITS64, 0, 0},					"\xDC", D2, I586},
//	{"fcom"INS_FCOM,	1,	{FPUREG, 0, 0},	 					"\xD8\xD0", I, I586},
//	{"fcom"INS_FCOM,	0,	{0, 0, 0},			 			"\xD8\xD1", 0, I586},
//	{"fcomp"INS_FCOMP,	1,	{MEM|BITS32, 0, 0},					"\xD8", D3, I586},
//	{"fcomp"INS_FCOMP,	1,	{MEM|BITS64, 0, 0},					"\xDC", D3, I586},
//	{"fcomp"INS_FCOMP,	1,	{FPUREG, 0, 0},	 					"\xD8\xD8", I, I586},
//	{"fcomp"INS_FCOMP,	0,	{0, 0, 0},						"\xD8\xD9", 0, I586},
//	{"fcompp"INS_FCOMPP,	0,	{0, 0, 0},						"\xDE\xD9", 0, I586},
																					
//	{"fcos"INS_FCOS,	0,	{0, 0, 0},						"\xD9\xFF", 0, I586},
																												
//	{"fdecstp"INS_FDECSTP,	0,	{0, 0, 0},						"\xD9\xF6", 0, I586},

//	{"fdiv"INS_FDIV,	1,	{MEM|BITS32, 0, 0},					"\xD8", D6, I586},
//	{"fdiv"INS_FDIV,	1,	{MEM|BITS64, 0, 0},					"\xDC", D6, I586},
//	{"fdiv"INS_FDIV,	1,	{
//	{"fdiv"INS_FDIV,	1,	{
//	{"fdivp"INS_FDIVP,	0,	{0, 0, 0},						"\xDE\xF9", 0, I586},
//	{"fidiv"INS_FIDIV,	1,	{MEM|BITS32, 0, 0},					"\xDA", D6, I586},
//	{"fidiv"INS_FIDIV,	1,	{MEM|BITS64, 0, 0},					"\xDE", D6, I586},

//	{"fdivr"INS_FDIVR,	1,	{MEM|BITS32, 0, 0},					"\xD8", D7, I586},
//	{"fdivr"INS_FDIVR,	1,	{MEM|BITS64, 0, 0},					"\xDC", D7, I586},
//	{"fdivr"INS_FDIVR,	2,
//	{"fdivr"INS_FDIVR,	2,
//	{"fdivrp"INS_FDIVRP,	2,
//	{"fdivrp"INS_FDIVRP,	0,	{0, 0, 0},						"\xDE\xF1", 0, I586},
//	{"fidivr"INS_FIDIVR,	1,	{MEM|BITS32, 0, 0},					"\xDA", D7, I586},
//	{"fidivr"INS_FIDIVR,	1,	{MEM|BITS64, 0, 0},					"\xDE", D7, I586},
//	{"fist"INS_FIST,	1,	{MEM|BITS16, 0, 0},					"\xDF", D2, I586},
//	{"fist"INS_FIST,	1,	{MEM|BITS32, 0, 0},					"\xDB", D2, I586},
//	{"fistp"INS_FISTP,	1,	{MEM|BITS16, 0, 0},					"\xDF", D3, I586},
//	{"fistp"INS_FISTP,	1,	{MEM|BITS32, 0, 0},					"\xDB", D3, I586},
//	{"fistp"INS_FISTP,	1,	{MEM|BITS64, 0, 0},					"\xDF", D7, I586},
																					
//	{"fld"INS_FLD,	1,	{MEM|BITS32, 0, 0},					"\xD9", D0, I586},
//	{"fld"INS_FLD,	1,	{MEM|BITS64, 0, 0},					"\xDD", D0, I586},
//	{"fld"INS_FLD,	1,	{MEM|BITS80, 0, 0},					"\xDB", D5, I586},
//	{"fld"INS_FLD,	1,	{FPUREG, 0, 0},						"\xD9\xC0", I, I586},
//	{"fld1"INS_FLD1,	0,	{0, 0, 0},						"\xD9\xE8", 0, I586},
//	{INS_FLDL2T,	0,	{0, 0, 0},						"\xD9\xE9", 0, I586},
//	{INS_FLDL2E,	0,	{0, 0, 0},						"\xD9\xEA", 0, I586},
//	{INS_FLDPI,	0,	{0, 0, 0},						"\xD9\xEB", 0, I586},
//	{INS_FLDLG2,	0,	{0, 0, 0},						"\xD9\xEC", 0, I586},
//	{INS_FLDLN2,	0,	{0, 0, 0},						"\xD9\xED", 0, I586},
//	{INS_FLDZ,	0,	{0, 0, 0},						"\xD9\xEE", 0, I586},
																
//	{"fldcw"INS_FLDCW,	1,	{MEM, 0, 0},						"\xD9", D5, I586},
																			
//	{INS_FLDENV,	1,	{MEM, 0, 0},						"\xD9", D4, I586},

//	{INS_FMUL,	1,	{MEM|BITS32, 0, 0},					"\xD8", D1, I586},
//	{INS_FMUL,	1,	{MEM|BITS64, 0, 0},					"\xDC", D1, I586},
//	{INS_FMUL,
//	{INS_FMUL,
//	{INS_FMULP,	0,	{0, 0, 0},						"\xDE\xC9", 0, I586},
//	{INS_FIMUL,	1,	{MEM|BITS32, 0, 0},					"\xDA", D1, I586},
//	{INS_FIMUL,	1,	{MEM|BITS64, 0, 0},					"\xDE", D1, I586},

//	{INS_FNOP,	0,	{0, 0, 0},						"\xD9\xD0", 0, I586},

//	{INS_FPATAN,	0,	{0, 0, 0},						"\xD9\xF3", 0, I586},
																			
//	{INS_FPREM,	0,	{0, 0, 0},						"\xD9\xF8", 0, I586},
//	{INS_FPREM1,	0,	{0, 0, 0},						"\xD9\xF5", 0, I586},

//	{INS_FPTAN,	0,	{0, 0, 0},						"\xD9\xF2", 0, I586},

//	{INS_FRNDINT,	0,	{0, 0, 0},						"\xD9\xFC", 0, I586},

//	{INS_FRSTOR,	0,	{0, 0, 0},						"\xDD", D4, I586},

//	{INS_FSAVE,		1,
//	{INS_FNSAVE,	1,

//	{INS_FSCALE,	0,	{0, 0, 0},						"\xD9\xFD", 0, I586},
																			
//	{INS_FSIN,	0,	{0, 0, 0},						"\xD9\xFE", 0, I586},

//	{INS_FSINCOS,	0,	{0, 0, 0},						"\xD9\xFB", 0, I586},

//	{INS_FSQRT,	0,	{0, 0, 0},						"\xD9\xFA", 0, I586},

//	{INS_FST,	1,	{MEM|BITS32, 0, 0},					"\xD9", D2, I586},
//	{INS_FST,	1,	{MEM|BITS64, 0, 0},					"\xDD", D2, I586},
	{"fst",		1,	{FPUREG, 0, 0},						"\xDD\xD0", I, I586},
//	{INS_FSTP,	1,	{MEM|BITS32, 0, 0},					"\xD9", D3, I586},
//	{INS_FSTP,	1,	{MEM|BITS64, 0, 0},					"\xDD", D3, I586},
//	{INS_FSTP,	1,	{MEM|BITS80, 0, 0},					"\xDB", D7, I586},
//	{INS_FSTP,	1,	{FPUREG, 0, 0},						"\xDD\xD8", I, I586},

//	{INS_FSTCW,	1,
//	{INS_FNSTCW,	1,

//	{INS_FSTENV,	1,
//	{INS_FNSTENV,	1,
//	{INS_FSTSW,	1,
//	{INS_FSTSW,	1,	{AX_REG, 0, 0},						"\x9B\xDF", D1, I586},
//	{INS_FNSTSW,	1,
//	{INS_FNSTSW,	1,	{AX_REG, 0, 0},						"\xDF\xE0", 0, I586},
//	{INS_FSUB,	1,	{MEM|BITS32, 0, 0},					"\xD8", D4, I586},
//	{INS_FSUB,	1,	{MEM|BITS64, 0, 0},					"\xDC", D4, I586},
//	{INS_FSUB,	2,	{
//	{INS_FSUB,	2,	{
//	{INS_FSUBP,	2,	{
//	{INS_FSUBP,	0,	{0, 0, 0},						"\xDE\xE9", 0, I586},
//	{INS_FISUB,	1,	{MEM|BITS32, 0, 0},					"\xDA", D4, I586},
//	{INS_FISUB,	1,	{MEM|BITS16, 0, 0},					"\xDE", D4, I586},
//	{INS_FSUBR,	1,	{MEM|BITS32, 0, 0},					"\xD8", D5, I586},
//	{INS_FSUBR,	1,	{MEM|BITS64, 0, 0},					"\xDC", D5, I586},
//	{INS_FSUBR,	2,	{
//	{INS_FSUBR,	2,	{
//	{INS_FSUBRP,	2,	{
//	{INS_FSUBRP,	0, 	{0, 0, 0},						"\xDE\xE1", 0, I586},
//	{INS_FISUBR,	1,	{MEM|BITS32, 0, 0},					"\xDA", D5, I586},
//	{INS_FISUBR,	1,	{MEM|BITS16, 0, 0},					"\xDE", D5, I586},

//	{INS_FTST,	0,	{0, 0, 0},						"\xD9\xE4", 0, I586},
//	{INS_FUCOM,	1,	{FPUREG, 0, 0},						"\xDD\xE0", I, I586},
//	{INS_FUCOMP,	1,	{
//	{INS_FUCOMP,	0,	{0, 0, 0},						"\xDD\xE9", 0, I586},
//	{INS_FUCOMPP,	0,	{0, 0, 0},						"\xDA\xE9", 0, I586},

//	{INS_FWAIT,	0,	{0, 0, 0},						"\x9B", 0, I586},
												
//	{INS_FXAM,	0,	{0, 0, 0},						"\xD9\xE5", 0, I586},

//	{INS_FXCH,	1,	{FPUREG, 0, 0},						"\xD9\xC8", I, I586},
//	{INS_FXCH,	0,	{0, 0, 0},						"\xD9\xC9", 0, I586},

//	{INS_FXTRACT,	0,	{0, 0, 0},						"\xD9\xF4", 0, I586},

//	{INS_FYL2X,	0,	{0, 0, 0},						"\xD9\xF1", 0, I586},
//	{INS_FYL2XP1,	0,	{0, 0, 0},						"\xD9\xF9", 0, I586},

//	{INS_FFREE,	1,	{FPUREG, 0, 0},						"\xDD\xC0", I, I586},
											
//	{INS_FICOM,	1,	{MEM|BITS16, 0, 0},					"\xDE", D2, I586},
//	{INS_FICOM,	1,	{MEM|BITS32, 0, 0},					"\xDA", D2, I586},
//	{INS_FICOMP,	1,	{MEM|BITS16, 0, 0},					"\xDE", D3, I586},
//	{INS_FICOMP,	1,	{MEM|BITS32, 0, 0},					"\xDA", D3, I586},
											
//	{INS_FILD,	1,	{MEM|BITS16, 0, 0},					"\xDF", D0, I586},
//	{INS_FILD,	1,	{MEM|BITS32, 0, 0},					"\xDB", D0, I586},
//	{INS_FILD,	1,	{MEM|BITS64, 0, 0},					"\xDF", D5, I586},
										
//	{INS_FINCSTP,	0,	{0, 0, 0},						"\xD9\xF7", 0, I586},

//	{INS_FINIT,	0,	{0, 0, 0},						"\x9B\xDB\xE3", 0, I586},
//	{INS_FNINIT,	0,	{0, 0, 0},						"\xDB\xE3", 0, I586},	

//	{INS_FIST,	1,	{MEM|BITS16, 0, 0},					"\xDF", D2, I586},
//	{INS_FIST,	1,	{MEM|BITS32, 0, 0},					"\xDB", D2, I586},
//	{INS_FISTP,	1,	{MEM|BITS16, 0, 0},					"\xDF", D3, I586},
//	{INS_FISTP,	1,	{MEM|BITS32, 0, 0},					"\xDB", D3, I586},
//	{INS_FISTP,	1,	{MEM|BITS64, 0, 0},					"\xDF", D7, I586},

//	{INS_FLD,	1,	{MEM|BITS32, 0, 0},					"\xD9", D0, I586},
//	{INS_FLD,	1,	{MEM|BITS64, 0, 0},					"\xDD", D0, I586},
//	{INS_FLD,	1,	{MEM|BITS80, 0, 0},					"\xDB", D5, I586},
//	{INS_FLD,	1,	{FPUREG, 0, 0},						"\xD9\xC0", I, I586},

//	{INS_FLD1,	0,	{0, 0, 0},						"\xD9\xE8", 0, I586},
//	{INS_FLDL2T,	0,	{0, 0, 0},						"\xD9\xE9", 0, I586},
//	{INS_FLDL2E,	0,	{0, 0, 0},						"\xD9\xEA", 0, I586},
//	{INS_FLDPI,	0,	{0, 0, 0},						"\xD9\xEB", 0, I586},
//	{INS_FLDLG2,	0,	{0, 0, 0},						"\xD9\xEC", 0, I586},
//	{INS_FLDLN2,	0,	{0, 0, 0},						"\xD9\xED", 0, I586},
//	{INS_FLDZ,	0,	{0, 0, 0},						"\xD9\xEE", 0, I586},

//	{INS_FLDCW,	1,	{MEM, 0, 0},						"\xD9", D5, I586},

//	{INS_FLDENV,	1,	{MEM, 0, 0},						"\xD9", D4, I586},

//	{INS_FMUL,	1,	{MEM|BITS32, 0, 0},					"\xD8", D1, I586},
//	{INS_FMUL,	1,	{MEM|BITS64, 0, 0},					"\xDC", D1, I586},
//	{INS_FMUL,
//	{INS_FMUL,
//	{INS_FMULP,	0,	{0, 0, 0},						"\xDE\xC9", 0, I586},
//	{INS_FIMUL,	1,	{MEM|BITS32, 0, 0},					"\xDA", D1, I586},
//	{INS_FIMUL,	1,	{MEM|BITS64, 0, 0},					"\xDE", D1, I586},

//	{INS_FNOP,	0,	{0, 0, 0},						"\xD9\xD0", 0, I586},

//	{INS_FPATAN,	0,	{0, 0, 0},						"\xD9\xF3", 0, I586},

//	{INS_FPREM,		0,	{0, 0, 0},					"\xD9\xF8", 0, I586},
//	{INS_FPREM1,	0,	{0, 0, 0},						"\xD9\xF5", 0, I586},

//	{INS_FPTAN,	0,	{0, 0, 0},						"\xD9\xF2", 0, I586},

//	{INS_FRNDINT,	0,	{0, 0, 0},						"\xD9\xFC", 0, I586},

//	{INS_FRSTOR,	0,	{0, 0, 0},						"\xDD", D4, I586},

//	{INS_FSAVE,	1,
//	{INS_FNSAVE,	1,

//	{INS_FSCALE,	0,	{0, 0, 0},						"\xD9\xFD", 0, I586},

//	{INS_FSIN,	0,	{0, 0, 0},						"\xD9\xFE", 0, I586},

//	{INS_FSINCOS,	0,	{0, 0, 0},						"\xD9\xFB", 0, I586},

//	{INS_FSQRT,	0,	{0, 0, 0},						"\xD9\xFA", 0, I586},

//	{INS_FST,	1,	{MEM|BITS32, 0, 0},					"\xD9", D2, I586},
//	{INS_FST,	1,	{MEM|BITS64, 0, 0},					"\xDD", D2, I586},
//	{INS_FST,	1,	{FPUREG, 0, 0},						"\xDD\xD0", I, I586},
//	{INS_FSTP,	1,	{MEM|BITS32, 0, 0},					"\xD9", D3, I586},
//	{INS_FSTP,	1,	{MEM|BITS64, 0, 0},					"\xDD", D3, I586},
//	{INS_FSTP,	1,	{MEM|BITS80, 0, 0},					"\xDB", D7, I586},
//	{INS_FSTP,	1,	{FPUREG, 0, 0},						"\xDD\xD8", I, I586},

//	{INS_FSTCW,	1,
//	{INS_FNSTCW,	1,

//	{INS_FSTENV,	1,
//	{INS_FNSTENV,	1,

//	{INS_FSTSW,	1,
//	{INS_FSTSW,	1,	{AX_REG, 0, 0},						"\x9B\xDF", D1, I586},
//	{INS_FNSTSW,	1,
//	{INS_FNSTSW,	1,	{AX_REG, 0, 0},						"\xDF\xE0", 0, I586},

//	{INS_FSUB,	1,	{MEM|BITS32, 0, 0},					"\xD8", D4, I586},
//	{INS_FSUB,	1,	{MEM|BITS64, 0, 0},					"\xDC", D4, I586},
//	{INS_FSUB,	2,	{
//	{INS_FSUB,	2,	{
//	{INS_FSUBP,	2,	{
//	{INS_FSUBP,	0,	{0, 0, 0},						"\xDE\xE9", 0, I586},
//	{INS_FISUB,	1,	{MEM|BITS32, 0, 0},					"\xDA", D4, I586},
//	{INS_FISUB,	1,	{MEM|BITS16, 0, 0},					"\xDE", D4, I586},

//	{INS_FSUBR,	1,	{MEM|BITS32, 0, 0},					"\xD8", D5, I586},
//	{INS_FSUBR,	1,	{MEM|BITS64, 0, 0},					"\xDC", D5, I586},
//	{INS_FSUBR,	2,	{
//	{INS_FSUBR,	2,	{
//	{INS_FSUBRP,	2,	{
//	{INS_FSUBRP,	0, 	{0, 0, 0},						"\xDE\xE1", 0, I586},
//	{INS_FISUBR,	1,	{MEM|BITS32, 0, 0},					"\xDA", D5, I586},
//	{INS_FISUBR,	1,	{MEM|BITS16, 0, 0},					"\xDE", D5, I586},

//	{INS_FTST,	0,	{0, 0, 0},						"\xD9\xE4", 0, I586},

//	{INS_FUCOM,	1,	{FPUREG, 0, 0},						"\xDD\xE0", I, I586},
//	{INS_FUCOM,	0,	{0, 0, 0},						"\xDD\xE1", 0, I586},
//	{INS_FUCOMP,	1,	{
//	{INS_FUCOMP,	0,	{0, 0, 0},						"\xDD\xE9", 0, I586},
//	{INS_FUCOMPP,	0,	{0, 0, 0},						"\xDA\xE9", 0, I586},

//	{INS_FWAIT,	0,	{0, 0, 0},						"\x9B", 0, I586},

//	{INS_FXAM,	0,	{0, 0, 0},						"\xD9\xE5", 0, I586},

//	{INS_FXCH,	1,	{FPUREG, 0, 0},						"\xD9\xC8", I, I586},
//	{INS_FXCH,	0,	{0, 0, 0},		 				"\xD9\xC9", 0, I586},

//	{INS_FXTRACT,	0,	{0, 0, 0},						"\xD9\xF4", 0, I586},

//	{INS_FYL2X,	0,	{0, 0, 0},						"\xD9\xF1", 0, I586},
//	{INS_FYL2XP1,	0,	{0, 0, 0},						"\xD9\xF9", 0, I586},

	{"hlt",		0,	{0, 0, 0},						"\xF4", 0, I86},
	
	{"idiv",	2,	{AL_REG, REGMEM|BITS8, 0},				"\xF6", D7, I86},
	{"idiv",	2,	{AX_REG, REGMEM|BITS16, 0},				"\xF7", D7, I86},
	{"idiv",	2,	{EAX_REG, REGMEM|BITS32, 0},				"\xF7", D7, I86},

	{"imul",	1,	{REGMEM|BITS8, 0, 0},					"\xF6", D5, I86},
	{"imul",	1,	{REGMEM|BITS16, 0, 0},					"\xF7", D5, I86},
	{"imul",	1,	{REGMEM|BITS32, 0, 0},					"\xF7", D5, I86},
	{"imul",	2,	{REG|BITS16, REGMEM|BITS16, 0},				"\x0F\xAF", R|MODRM, I86},
	{"imul",	2,	{REG|BITS32, REGMEM|BITS32, 0},				"\x0F\xAF", R|MODRM, I86},
	{"imul",	3,	{REG|BITS16, REGMEM|BITS16, IMM|BITS8},			"\x6B", R|MODRM, I86},
	{"imul",	3,	{REG|BITS32, REGMEM|BITS32, IMM|BITS8}, 		"\x6B", R|MODRM, I86},
	{"imul",	2,	{REG|BITS16, IMM|BITS8, 0},				"\x6B", R|MODRM, I86},
	{"imul",	3,	{REG|BITS16, REGMEM|BITS16, IMM|BITS8},			"\x69", R|MODRM, I86},
	{"imul",	3,	{REG|BITS32, REGMEM|BITS32, IMM|BITS8},			"\x69", R|MODRM, I86},
	{"imul",	2,	{REG|BITS16, IMM|BITS16, 0},				"\x69", R|MODRM, I86},
	{"imul",	2,	{REG|BITS32, IMM|BITS32, 0},				"\x69", R|MODRM, I86},

	{"in",		2,	{AL_REG, IMM|BITS8, 0},					"\xE4", 0, I86},
	{"in",		2,	{AX_REG, IMM|BITS8, 0},					"\xE5", 0, I86},
	{"in",		2,	{EAX_REG, IMM|BITS8, 0},				"\xE5", 0, I86},
	{"in",		2,	{AL_REG, DX_REG, 0},					"\xEC", 0, I86},
	{"in",		2,	{AX_REG, DX_REG, 0},					"\xED", 0, I86},
	{"in",		2,	{EAX_REG, DX_REG, 0},					"\xED", 0, I86},

	{"inc",		1,	{REGMEM|BITS8, 0, 0},					"\xFE", D0, I86},
	{"inc",		1,	{REGMEM|BITS16, 0, 0},					"\xFF", D0, I86},
	{"inc",		1,	{REGMEM|BITS32, 0, 0},					"\xFF", D0, I86},
	{"inc",		1,	{REG|BITS16, 0, 0},					"\x40", RW, I86},
	{"inc",		1,	{REG|BITS32, 0, 0},					"\x40", RD, I86},

	{"ins",		2,	{MEM|BITS8, DX_REG, 0},					"\x6C", 0, I186},
	{"ins",		2,	{MEM|BITS16, DX_REG, 0},				"\x6D", 0, I186},
	{"ins",		2,	{MEM|BITS32, DX_REG, 0},				"\x6D", 0, I186},
	{"insb",	0,	{0, 0, 0},						"\x6C", 0, I186},
	{"insw",	0,	{0, 0, 0},						"\x6D", 0, I186},
	{"insd",	0,	{0, 0, 0},						"\x6D", 0, I186},

	{"int",		1,	{IMM|BITS8, 0, 0},					"\xCD", 0, I86},
	{"int0",	0,	{0, 0, 0},						"\xCE", 0, I86},
	//
	// More Ints
	//

	{"invd",	0,	{0, 0, 0},						"\x0F\x08", 0, I486},

	{"invlpg",	1,	{MEM, 0, 0},						"\xOF", D7, I486},

	{"iret",	0,	{0, 0, 0},						"\xCF", 0, I86},
	{"iretd",	0,	{0, 0, 0},						"\xCF", 0, I86},
																			
	{"ja",		1,	{IMM|BITS8, 0, 0},					"\x77", CB, I86},
	{"ja",		1,	{IMM|BITS16, 0, 0},					"\xOF\x87", CW, I86},
	{"ja",		1,	{IMM|BITS32, 0, 0},					"\xOF\x87", CD, I86},
	{"jae",		1,	{IMM|BITS8, 0, 0},					"\x73", CB, I86},
	{"jae",		1,	{IMM|BITS16, 0, 0},					"\xOF\x83", CW, I86},
	{"jae",		1,	{IMM|BITS32, 0, 0},					"\xOF\x83", CD, I86},
	{"jb",		1,	{IMM|BITS8, 0, 0},					"\x72", CB, I86},
	{"jb",		1,	{IMM|BITS16, 0, 0},					"\xOF\x82", CW, I86},
	{"jb",		1,	{IMM|BITS32, 0, 0},					"\xOF\x82", CD, I86},
	{"jbe",		1,	{IMM|BITS8, 0, 0},					"\x76", CB, I86},
	{"jbe",		1,	{IMM|BITS16, 0, 0},					"\xOF\x86", CW, I86},
	{"jbe",		1,	{IMM|BITS32, 0, 0},					"\xOF\x86", CD, I86},
	{"jc",		1,	{IMM|BITS8, 0, 0},					"\x72", CB, I86},
	{"jc",		1,	{IMM|BITS16, 0, 0},					"\xOF\x82", CW, I86},
	{"jc",		1,	{IMM|BITS32, 0, 0},					"\xOF\x82", CD, I86},
	{"jcxz",	1,	{IMM|BITS8, 0, 0},					"\xE3", CB, I86},
	{"jecxz",	1,	{IMM|BITS8, 0, 0},					"\xE3", CB, I86},
	{"je",		1,	{IMM|BITS8, 0, 0},					"\x74", CB, I86},
	{"je",		1,	{IMM|BITS16, 0, 0},					"\xOF\x84", CW, I86},
	{"je",		1,	{IMM|BITS32, 0, 0},					"\xOF\x84", CD, I86},
	{"jg",		1,	{IMM|BITS8, 0, 0},					"\x7F", CB, I86},
	{"jg",		1,	{IMM|BITS16, 0, 0},					"\xOF\x8F", CW, I86},
	{"jg",		1,	{IMM|BITS32, 0, 0},					"\xOF\x8F", CD, I86},
	{"jge",		1,	{IMM|BITS8, 0, 0},					"\x7D", CB, I86},
	{"jge",		1,	{IMM|BITS16, 0, 0},					"\xOF\x8D", CW, I86},
	{"jge",		1,	{IMM|BITS32, 0, 0},					"\xOF\x8D", CD, I86},
	{"jl",		1,	{IMM|BITS8, 0, 0},					"\x7C", CB, I86},
	{"jl",		1,	{IMM|BITS16, 0, 0},					"\xOF\x8C", CW, I86},
	{"jl",		1,	{IMM|BITS32, 0, 0},					"\xOF\x8C", CD, I86},
	{"jle",		1,	{IMM|BITS8, 0, 0},					"\x7E", CB, I86},
	{"jle",		1,	{IMM|BITS16, 0, 0},					"\xOF\x8E", CW, I86},
	{"jle",		1,	{IMM|BITS32, 0, 0},					"\xOF\x8E", CD, I86},
	{"jna",		1,	{IMM|BITS8, 0, 0},					"\x76", CB, I86},
	{"jna",		1,	{IMM|BITS16, 0, 0},					"\xOF\x86", CW, I86},
	{"jna",		1,	{IMM|BITS32, 0, 0},					"\xOF\x86", CD, I86},
	{"jnae",	1,	{IMM|BITS8, 0, 0},					"\x72", CB, I86},
	{"jnae",	1,	{IMM|BITS16, 0, 0},					"\xOF\x82", CW, I86},
	{"jnae", 	1,	{IMM|BITS32, 0, 0},					"\xOF\x82", CD, I86},
	{"jnb",		1,	{IMM|BITS8, 0, 0},					"\x77", CB, I86},
	{"jnb",		1,	{IMM|BITS16, 0, 0},					"\xOF\x83", CW, I86},
	{"jnb",		1,	{IMM|BITS32, 0, 0},					"\xOF\x83", CD, I86},
	{"jnbe",	1,	{IMM|BITS8, 0, 0},					"\x77", CB, I86},
	{"jnbe",	1,	{IMM|BITS16, 0, 0},					"\xOF\x87", CW, I86},
	{"jnbe", 	1,	{IMM|BITS32, 0, 0},					"\xOF\x87", CD, I86},
	{"jnc",		1,	{IMM|BITS8, 0, 0},					"\x73", CB, I86},
	{"jnc",		1,	{IMM|BITS16, 0, 0},					"\xOF\x83", CW, I86},
	{"jnc",		1,	{IMM|BITS32, 0, 0},					"\xOF\x83", CD, I86},
	{"jne",		1,	{IMM|BITS8, 0, 0},					"\x75", CB, I86},
	{"jne",		1,	{IMM|BITS16, 0, 0},					"\xOF\x85", CD, I86},
	{"jne",		1,	{IMM|BITS32, 0, 0},					"\xOF\x85", CW, I86},
	{"jng",		1,	{IMM|BITS8, 0, 0},					"\x7E", CB, I86},
	{"jng",		1,	{IMM|BITS16, 0, 0},					"\xOF\x8E", CD, I86},
	{"jng",		1,	{IMM|BITS32, 0, 0},					"\xOF\x8E", CW, I86},
	{"jnge",	1,	{IMM|BITS8, 0, 0},					"\x7C", CB, I86},
	{"jnge", 	1,	{IMM|BITS16, 0, 0},					"\xOF\x8C", CD, I86},
	{"jnge",	1,	{IMM|BITS32, 0, 0},					"\xOF\x8C", CW, I86},
	{"jnl",		1,	{IMM|BITS8, 0, 0},					"\x7D", CB, I86},
	{"jnl",		1,	{IMM|BITS16, 0, 0},					"\xOF\x8D", CW, I86},
	{"jnl",		1,	{IMM|BITS32, 0, 0},					"\xOF\x8D", CD, I86},
	{"jnle",	1,	{IMM|BITS8, 0, 0},					"\x7F", CB, I86},
	{"jnle",	1,	{IMM|BITS16, 0, 0},					"\xOF\x8F", CW, I86},
	{"jnle", 	1,	{IMM|BITS32, 0, 0},					"\xOF\x8F", CD, I86},
	{"jno",		1,	{IMM|BITS8, 0, 0},					"\x71", CB, I86},
	{"jno",		1,	{IMM|BITS16, 0, 0},					"\xOF\x81", CW, I86},
	{"jno",		1,	{IMM|BITS32, 0, 0},					"\xOF\x81", CD, I86},
	{"jnp",		1,	{IMM|BITS8, 0, 0},					"\x7B", CB, I86},
	{"jnp",		1,	{IMM|BITS16, 0, 0},					"\xOF\x8B", CW, I86},
	{"jnp",		1,	{IMM|BITS32, 0, 0},					"\xOF\x8B", CD, I86},
	{"jns",		1,	{IMM|BITS8, 0, 0},					"\x79", CB, I86},
	{"jnz",		1,	{IMM|BITS8, 0, 0},					"\x75", CB, I86},
	{"jnz",		1,	{IMM|BITS16, 0, 0},					"\xOF\x85", CW, I86},
	{"jnz",		1,	{IMM|BITS32, 0, 0},					"\xOF\x85", CD, I86},
	{"jo",		1,	{IMM|BITS8, 0, 0},					"\x70", CB, I86},
	{"jo",		1,	{IMM|BITS16, 0, 0},					"\xOF\x80", CW, I86},
	{"jo",		1,	{IMM|BITS32, 0, 0},					"\xOF\x80", CD, I86},
	{"jp",		1,	{IMM|BITS8, 0, 0},					"\x7A", CB, I86},
	{"jp",		1,	{IMM|BITS16, 0, 0},					"\xOF\x8A", CW, I86},
	{"jp",		1,	{IMM|BITS32, 0, 0},					"\xOF\x8A", CD, I86},
	{"jpe",		1,	{IMM|BITS8, 0, 0},					"\x7A", CB, I86},
	{"jpe",		1,	{IMM|BITS16, 0, 0},					"\xOF\x8A", CW, I86},
	{"jpe",		1,	{IMM|BITS32, 0, 0},					"\xOF\x8A", CD, I86},
	{"jpo",		1,	{IMM|BITS8, 0, 0},					"\x7B", CB, I86},
	{"jpo",		1,	{IMM|BITS16, 0, 0},					"\xOF\x8B", CD, I86},
	{"jpo",		1,	{IMM|BITS32, 0, 0},					"\xOF\x8B", CW, I86},
	{"js",		1,	{IMM|BITS8, 0, 0},					"\x78", CB, I86},
	{"js",		1,	{IMM|BITS16, 0, 0},					"\xOF\x88", CW, I86},
	{"js",		1,	{IMM|BITS32, 0, 0},					"\xOF\x88", CD, I86},
	{"jz",		1,	{IMM|BITS8, 0, 0},					"\x74", CB, I86},
	{"jz",		1,	{IMM|BITS16, 0, 0},					"\xOF\x84", CW, I86},
	{"jz",		1,	{IMM|BITS32, 0, 0},					"\xOF\x84", CD, I86},
																	
	{"jmp",		1,	{IMM|BITS8, 0, 0},					"\xEB", CB, I86},
	{"jmp",		1,	{IMM|BITS16, 0, 0},					"\xE9", CW, I86},
	{"jmp",		1,	{REGMEM|BITS16, 0, 0},					"\xFF", D4, I86},
	//
	// More Jumps
	//
	{"jmp",		1,	{REGMEM|BITS32, 0, 0},					"\xFF", D4, I86},
	//
	// More Jumps
	//

	{"lahf",	0,	{0, 0, 0},						"\x9F", 0, I86},

	{"lar",		2,	{REG|BITS16, REGMEM|BITS16, 0},				"\x0F\x02", R|MODRM, I286},
	{"lar",		2,	{REG|BITS32, REGMEM|BITS32, 0},				"\x0F\x02", R|MODRM, I286},

//	{"lds",		2,	{REG|BITS16,
//	{"les",
//	{"lfs",
//	{"lgs",
//	{"lss",
	{"lea",		2,	{REG|BITS16, MEM, 0},					"\x8D", R|MODRM, I86},
	{"lea",		2,	{REG|BITS32, MEM, 0},					"\x8D", R|MODRM, I86},

	{"leave",	0,	{0, 0, 0},						"\xC9", 0, I186},

//	{"lgdt",	1,	{MEM|BITS16, 0, 0},					"\x0F\x01", D2, I286},
//	{"lgdt",	1,	{MEM|BITS32, 0, 0},					"\x0F\x01", D2, I286},
//	{"lidt",	1,	{MEM|BITS16, 0, 0},					"\x0F\x01", D3, I286},
//	{"lidt",	1,	{MEM|BITS32, 0, 0},					"\x0F\x01", D3, I286},

	{"lldt",	1,	{REGMEM|BITS16, 0, 0},					"\x0F\x00", D2, I286},

	{"lmsw",	1,	{REGMEM|BITS16, 0, 0},					"\x0F\x01", D6, I286},

	{"lock",	0,	{0, 0, 0},						"\xF0", 0, I86},

//	{"lods",	1,	{MEM|BITS8, 0, 0},					"\xAC", 0, I586},
//	{"lods",	1,	{MEM|BITS16, 0, 0},					"\xAD", 0, I586},
//	{"lods",	1,	{MEM|BITS32, 0, 0},					"\xAD", 0, I586},
	{"lodsb",	0,	{0, 0, 0},						"\xAC", 0, I86},
	{"lodsw",	0,	{0, 0, 0},						"\xAD", 0, I86},
	{"lodsd",	0,	{0, 0, 0},						"\xAD", 0, I386},
									
	{"loop",	1,	{IMM|BITS8, 0, 0},					"\xE2", CB, I86},
	{"loope",	1,	{IMM|BITS8, 0, 0},					"\xE1", CB, I86},
	{"loopz",	1,	{IMM|BITS8, 0, 0},					"\xE1", CB, I86},
	{"loopne",	1,	{IMM|BITS8, 0, 0},					"\xE0", CB, I86},
	{"loopnz",	1,	{IMM|BITS8, 0, 0},					"\xE0", CB, I86},

	{"lsl",		2,	{REG|BITS16, REGMEM|BITS16, 0},				"\x0F\x03", R|MODRM, I286},
	{"lsl",		2,	{REG|BITS32, REGMEM|BITS32, 0},				"\x0F\x03", R|MODRM, I286},

	{"ltr",		1,	{REGMEM|BITS16, 0, 0},					"\x0F\x00", D3, I286},

	{"mov",		2,	{REG|BITS8, REG|BITS8, 0},				"\x88", R|MODRM, I86},
	{"mov",		2,	{REGMEM|BITS16, REG|BITS16, 0},				"\x89", R|MODRM, I86},
	{"mov",		2,	{REGMEM|BITS32, REG|BITS32, 0},				"\x89", R|MODRM, I86},
	{"mov",		2,	{REG|BITS8, REGMEM|BITS8, 0},				"\x8A", R|MODRM, I86},
	{"mov",		2,	{REG|BITS16, REGMEM|BITS16, 0},				"\x8B", R|MODRM, I86},
	{"mov",		2,	{REG|BITS32, REGMEM|BITS32, 0},				"\x8B", R|MODRM, I86},
	{"mov",		2,	{REGMEM|BITS16, SREG, 0},				"\x8C", R|MODRM, I86},
	{"mov",		2,	{SREG, REGMEM|BITS16, 0},				"\x8E", R|MODRM, I86},
//	{"mov",		2,	{REG_AL, MEMOFF|BITS8, 0},				"\xA0", 0, I86},
//	{"mov",		2,	{REG_AX, MEMOFF|BITS16, 0},				"\xA1", 0, I86},
//	{"mov",		2,	{REG_EAX, MEMOFF|BITS32,0},				"\xA1", 0, I86},
//	{"mov",		2,	{MEMOFF|BITS8, REG_AL, 0},				"\xA2", 0, I86},
//	{"mov",		2,	{MEMOFF|BITS16, REG_AX, 0},				"\xA3", 0, I86},
//	{"mov",		2,	{MEMOFF|BITS32, REG_EAX, 0},				"\xA3", 0, I86},
	{"mov",		2,	{REG|BITS8, IMM|BITS8, 0},				"\xB0", RB, I86},
	{"mov",		2,	{REG|BITS16, IMM|BITS16, 0},				"\xB8", RW, I86},
	{"mov",		2,	{REG|BITS32, IMM|BITS32, 0},				"\xB8", RD, I86},
	{"mov",		2,	{REGMEM|BITS8, IMM|BITS8, 0},				"\xC6", D0, I86},
	{"mov",		2,	{REGMEM|BITS16, IMM|BITS16, 0},				"\xC7", D0, I86},
	{"mov",		2,	{REGMEM|BITS32, IMM|BITS32, 0},				"\xC7", D0, I86},

	{"mov",		2,	{REG_CR0, REG|BITS32, 0},				"\x0F\x22", R|MODRM, I86},
	{"mov",		2,	{REG_CR2, REG|BITS32, 0},				"\x0F\x22", R|MODRM, I86},
	{"mov",		2,	{REG_CR3, REG|BITS32, 0},				"\x0F\x22", R|MODRM, I86},
	{"mov",		2,	{REG_CR4, REG|BITS32, 0},				"\x0F\x22", R|MODRM, I86},
										
	{"mov",		2,	{REG|BITS32, REG_DR0, 0},				"\x0F\x21", R|MODRM, I86},
	{"mov",		2,	{REG|BITS32, REG_DR1, 0},				"\x0F\x21", R|MODRM, I86},
	{"mov",		2,	{REG|BITS32, REG_DR2, 0},				"\x0F\x21", R|MODRM, I86},
	{"mov",		2,	{REG|BITS32, REG_DR3, 0},				"\x0F\x21", R|MODRM, I86},
//	{"mov",		2,	{REG|BITS32, REG_DR4, 0},				"\x0F\x21", R|MODRM, I586},
//	{"mov",		2,	{REG|BITS32, REG_DR5, 0},				"\x0F\x21", R|MODRM, I586},
	{"mov",		2,	{REG|BITS32, REG_DR7, 0},     				"\x0F\x21", R|MODRM, I86},

	{"mov",		2,	{REG_DR0, REG|BITS32, 0},				"\x0F\x23", R|MODRM, I86},
	{"mov",		2,	{REG_DR1, REG|BITS32, 0},				"\x0F\x23", R|MODRM, I86},
	{"mov",		2,	{REG_DR2, REG|BITS32, 0},				"\x0F\x23", R|MODRM, I86},
	{"mov",		2,	{REG_DR3, REG|BITS32, 0},				"\x0F\x23", R|MODRM, I86},
//	{"mov",		2,	{REG_DR4, REG|BITS32, 0},				"\x0F\x23", R|MODRM, I586},
//	{"mov",		2,	{REG_DR5, REG|BITS32, 0},				"\x0F\x23", R|MODRM, I586},
	{"mov",		2,	{REG_DR6, REG|BITS32, 0},				"\x0F\x23", R|MODRM, I86},
	{"mov",		2,	{REG_DR7, REG|BITS32, 0},				"\x0F\x23", R|MODRM, I86},

//	{"movs",	2,	{MEM|BITS8, MEM|BITS8, 0},				"\xA4", 0, I586},
//	{"movs",	2,	{MEM|BITS16, MEM|BITS16, 0},				"\xA5", 0, I586},
//	{"movs",	2,	{MEM|BITS32, MEM|BITS32, 0},				"\xA5", 0, I586},
	{"movsb",	0,	{0, 0, 0},						"\xA4", 0, I86},
	{"movsw",	0,	{0, 0, 0},						"\xA5", 0, I86},
	{"movsd",	0,	{0, 0, 0},						"\xA5", 0, I386},
												
	{"movsx",	2,	{REG|BITS16, REGMEM|BITS8, 0},				"\x0F\xBE", R|MODRM, I386},
	{"movsx",	2,	{REG|BITS32, REGMEM|BITS8, 0},				"\x0F\xBE", R|MODRM, I386},
	{"movsx",	2,	{REG|BITS32, REGMEM|BITS16, 0},				"\x0F\xBF", R|MODRM, I386},

	{"movzx",	2,	{REG|BITS16, REGMEM|BITS8, 0},				"\x0F\xB6", R|MODRM, I386},
	{"movzx",	2,	{REG|BITS32, REGMEM|BITS8, 0},				"\x0F\xB6", R|MODRM, I386},
	{"movzx",	2,	{REG|BITS32, REGMEM|BITS16, 0},				"\x0F\xB7", R|MODRM, I386},

	{"mul",		2,	{AL_REG, REGMEM|BITS8, 0},				"\xF6", D4, I86},
	{"mul",		2,	{AX_REG, REGMEM|BITS16, 0},				"\xF7", D4, I86},
	{"mul",		2,	{EAX_REG, REGMEM|BITS32, 0},				"\xF7", D4, I86},

	{"neg",		1,	{REGMEM|BITS8, 0, 0},					"\xF6", D3, I86},
	{"neg",		1,	{REGMEM|BITS16, 0, 0},					"\xF7", D3, I86},
	{"neg",		1,	{REGMEM|BITS32, 0, 0},					"\xF7", D3, I86},

	{"nop",		0,	{0, 0, 0},						"\x90", 0, I86},

	{"not",		1,	{REGMEM|BITS8, 0, 0},					"\xF6", D2, I86},
	{"not",		1,	{REGMEM|BITS16, 0, 0},					"\xF7", D2, I86},
	{"not",		1,	{REGMEM|BITS32, 0, 0},					"\xF7", D2, I86},
																
	{"or",		2,	{AL_REG, IMM|BITS8, 0},					"\x0C", 0, I86},
	{"or",		2,	{AX_REG, IMM|BITS16, 0},				"\x0D", 0, I86},
	{"or",		2,	{EAX_REG, IMM|BITS32, 0},				"\x0D", 0, I86},
	{"or",		2,	{REGMEM|BITS8, IMM|BITS8, 0},				"\x80", D1, I86},
	{"or",		2,	{REGMEM|BITS16, IMM|BITS16, 0},				"\x81", D1, I86},
	{"or",		2,	{REGMEM|BITS32, IMM|BITS32, 0},				"\x81", D1, I86},
	{"or",		2,	{REGMEM|BITS16, IMM|BITS8, 0},				"\x83", D1, I86},
	{"or",		2,	{REGMEM|BITS32, IMM|BITS8, 0},				"\x83", D1, I86},
	{"or",		2,	{REGMEM|BITS8, REG|BITS8, 0},				"\x08", R|MODRM, I86},
	{"or",		2,	{REGMEM|BITS16, REG|BITS16, 0},				"\x09", R|MODRM, I86},
	{"or",		2,	{REGMEM|BITS32, REG|BITS32, 0},				"\x09", R|MODRM, I86},
	{"or",		2,	{REG|BITS8, REGMEM|BITS8, 0},				"\x0A", R|MODRM, I86},
	{"or",		2,	{REG|BITS16, REGMEM|BITS16, 0},				"\x0B", R|MODRM, I86},
	{"or",		2,	{REG|BITS32, REGMEM|BITS32, 0},				"\x0B", R|MODRM, I86},

	{"out",		2,	{IMM|BITS8, AL_REG, 0},					"\xE6", 0, I86},
	{"out",		2,	{IMM|BITS8, AX_REG, 0},					"\xE7", 0, I86},
	{"out",		2,	{IMM|BITS8, EAX_REG, 0},				"\xE7", 0, I86},
	{"out",		2,	{DX_REG, AL_REG, 0},					"\xEE", 0, I86},
	{"out",		2,	{DX_REG, AX_REG, 0},					"\xEF", 0, I86},
	{"out",		2,	{DX_REG, EAX_REG, 0},					"\xEF", 0, I86},

//	{"outs",	2,	{DX_REG, REGMEM|BITS8, 0},				"\x6E", 0, I586},
//	{"outs",	2,	{DX_REG, REGMEM|BITS16, 0},				"\x6F", 0, I586},
//	{"outs",	2,	{DX_REG, REGMEM|BITS32, 0},				"\x6F", 0, I586},
	{"outsb",	0,	{0, 0, 0},						"\x6E", 0, I86},
	{"outsw",	0,	{0, 0, 0},						"\x6F", 0, I86},
	{"outsd",	0,	{0, 0, 0},						"\x6F", 0, I386},

	{"pop",		1,	{MEM|BITS16, 0, 0},					"\x8F", D0, I86},
	{"pop",		1,	{MEM|BITS32, 0, 0},					"\x8F", D0, I86},
	{"pop",		1,	{REG|BITS16, 0, 0},					"\x58", RW, I86},
	{"pop",		1,	{REG|BITS32, 0, 0},					"\x58", RD, I86},
//	{"pop",		1,	{REG_DS|BITS16, 0, 0},					"\x1F", 0, I586},
//	{"pop",		1,	{REG_ES|BITS16, 0, 0},					"\x07", 0, I586},
//	{"pop",		1,	{REG_SS|BITS16, 0, 0},					"\x17", 0, I586},
//	{"pop",		1,	{REG_FS|BITS16, 0, 0},					"\x0F\xA1", 0, I586},
//	{"pop",		1,	{REG_GS|BITS16, 0, 0},					"\x0F\xA9", 0, I586},
//	{"popa",	0,	{0, 0, 0},						"\x61", 0, I586},
	{"popad",	0,	{0, 0, 0},						"\x61", 0, I386},
	{"popf",	0,	{0, 0, 0},						"\x9D", 0, I86},
	{"popfd",	0,	{0, 0, 0},						"\x9D", 0, I386},
										
	{"push",	1,	{REGMEM|BITS16, 0, 0},					"\xFF", D6, I86},
	{"push",	1,	{REGMEM|BITS32, 0, 0},					"\xFF", D6, I86},
	{"push",	1,	{REG|BITS16, 0, 0},					"\x50", RW, I86},
	{"push",	1,	{REG|BITS32, 0, 0},					"\x50", RD, I86},
	{"push",	1,	{IMM|BITS8, 0, 0},					"\x6A", 0, I86},
	{"push",	1,	{IMM|BITS16, 0, 0},					"\x68", 0, I86},
	{"push",	1,	{IMM|BITS32, 0, 0},					"\x68", 0, I86},
//	{"push",	1,	{REG_CS|BITS16, 0, 0},					"\x0E", 0, I586},
//	{"push",	1,	{REG_SS|BITS16, 0, 0},					"\x16", 0, I586},
//	{"push",	1,	{REG_DS|BITS16, 0, 0},					"\x1E", 0, I586},
//	{"push",	1,	{REG_ES|BITS16, 0, 0},					"\x06", 0, I586},
//	{"push",	1,	{REG_FS|BITS16, 0, 0},					"\x0F\xA0", 0, I586},
//	{"push",	1,	{REG_GS|BITS16, 0, 0},					"\x0F\xA8", 0, I586},

//	{"pusha",	0,	{0, 0, 0},						"\x60", 0, I586},
	{"pushad",	0,	{0, 0, 0},						"\x60", 0, I386},
	{"pushf",	0,	{0, 0, 0},	      					"\x9C", 0, I86},
	{"pushfd",	0,	{0, 0, 0},						"\x9C", 0, I386},
										
//	{"rcl",
//	{"rcl",
//	{"rcl",

	{"rdmsr",	0,	{0, 0, 0},						"\x0F\x32", 0, I586},

//	{"rdtsc",	0,	{0, 0, 0},						"\x0F\x31", 0, I586},

//	{"rep",
//	{"rep",
//	{"rep",

	{"ret",		0,	{0, 0, 0},						"\xC3", 0, I86},
	{"ret",		1,	{IMM|BITS16, 0, 0},					"\xC2", 0, I86},
				// CHECK ???

	{"rsw",		0,	{0, 0, 0},						"\x0F\xAA", 0, I586},

	{"sahf",	0,	{0, 0, 0},						"\x9E", 0, I86},
						
//	{"sal",		2,	{REGMEM|BITS8,
//	{"sal",

	{"sbb",		2,	{AL_REG, IMM|BITS8, 0},					"\x1C", 0, I86},
	{"sbb",		2,	{AX_REG, IMM|BITS16, 0},				"\x1D", 0, I86},
	{"sbb",		2,	{EAX_REG, IMM|BITS32, 0},				"\x1D", 0, I86},
	{"sbb",		2,	{REGMEM|BITS8, IMM|BITS8, 0},				"\x80", D3, I86},
	{"sbb",		2,	{REGMEM|BITS16, IMM|BITS16, 0},				"\x81", D3, I86},
	{"sbb",		2,	{REGMEM|BITS32, IMM|BITS32, 0},				"\x81", D3, I86},
	{"sbb",		2,	{REGMEM|BITS16, IMM|BITS8, 0},				"\x83", D3, I86},
	{"sbb",		2,	{REGMEM|BITS32, IMM|BITS8, 0},				"\x83", D3, I86},
	{"sbb",		2,	{REGMEM|BITS8, REG|BITS8, 0},				"\x18", R|MODRM, I86},
	{"sbb",		2,	{REGMEM|BITS16, REG|BITS16, 0},				"\x19", R|MODRM, I86},
	{"sbb",		2,	{REGMEM|BITS32, REG|BITS32, 0},				"\x19", R|MODRM, I86},
	{"sbb",		2,	{REG|BITS8, REGMEM|BITS8, 0},				"\x1A", R|MODRM, I86},
	{"sbb",		2,	{REG|BITS16, REGMEM|BITS16, 0},				"\x1B", R|MODRM, I86},
	{"sbb",		2,	{REG|BITS32, REGMEM|BITS32,0},				"\x1B", R|MODRM, I86},
	
//	{"scas",	1,	{MEM|BITS8, 0, 0},					"\xAE", 0, I586},
//	{"scas",	1,	{MEM|BITS16, 0, 0},					"\xAF", 0, I586},
//	{"scas",	1,	{MEM|BITS32, 0, 0},					"\xAF", 0, I586},
	{"scasb",	0,	{0, 0, 0},						"\xAE", 0, I86},
	{"scasw",	0,	{0, 0, 0},						"\xAF", 0, I86},
	{"scasd",	0,	{0, 0, 0},						"\xAF", 0, I386},

	{"seta",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x97", 0, I386},
	{"setae",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x93", 0, I386},
	{"setb",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x92", 0, I386},
	{"setbe",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x96", 0, I386},
	{"setc",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x92", 0, I386},
	{"setce",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x94", 0, I386},
	{"setg",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x9F", 0, I386},
	{"setge",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x9D", 0, I386},
	{"setl",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x9C", 0, I386},
	{"setle",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x9E", 0, I386},
	{"setna",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x96", 0, I386},
	{"setnae",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x92", 0, I386},
	{"setnb",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x93", 0, I386},
	{"setnbe",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x97", 0, I386},
	{"setnc",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x93", 0, I386},
	{"setne",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x95", 0, I386},
	{"setng",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x9E", 0, I386},
	{"setnge",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x9C", 0, I386},
	{"setnl",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x9D", 0, I386},
	{"setnle",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x9F", 0, I386},
	{"setno",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x91", 0, I386},
	{"setnp",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x9B", 0, I386},
	{"setns",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x99", 0, I386},
	{"setnz",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x95", 0, I386},
	{"seto",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x90", 0, I386},
	{"setp",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x9A", 0, I386},
	{"setpe",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x9A", 0, I386},
	{"setpo",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x9B", 0, I386},
	{"sets",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x98", 0, I386},
	{"setz",	1,	{REGMEM|BITS8, 0, 0},					"\x0F\x94", 0, I386},
																					
	{"sgdt",	1,	{MEM, 0, 0},						"\x0F\x01", D0, I286},
	{"sidt",	1,	{MEM, 0, 0},						"\x0F\x01", D1, I286},

	{"shld",	3,	{REGMEM|BITS16, REG|BITS16, IMM|BITS8},			"\x0F\xA4", 0, I386},
	{"shld",	3,	{REGMEM|BITS32, REG|BITS32, IMM|BITS8},			"\x0F\xA4", 0, I386},
	{"shld",	3,	{REGMEM|BITS16, REG|BITS16, REG_CL|BITS8},		"\x0F\xA5", 0, I386},
	{"shld",	3,	{REGMEM|BITS32, REG|BITS32, REG_CL|BITS8},		"\x0F\xA5", 0, I386},

	{"shrd",	3,	{REGMEM|BITS16, REG|BITS16, IMM|BITS8},			"\x0F\xAC", 0, I386},
	{"shrd",	3,	{REGMEM|BITS32, REG|BITS32, IMM|BITS8},			"\x0F\xAC", 0, I386},
	{"shrd",	3,	{REGMEM|BITS16, REG|BITS16, REG_CL|BITS8},		"\x0F\xAD", 0, I386},
	{"shrd",	3,	{REGMEM|BITS32, REG|BITS32, REG_CL|BITS8},		"\x0F\xAD", 0, I386},

	{"sldt",	1,	{REGMEM|BITS16, 0, 0},					"\x0F\x00", D0, I286},
	{"sldt",	1,	{REGMEM|BITS32, 0, 0},					"\x0F\x00", D0, I286},

	{"smsw",	1,	{REGMEM|BITS16, 0, 0},					"\x0F\x01", D4, I286},

	{"stc",		0,	{0, 0, 0},						"\xF9", 0, I86},

	{"std",		0,	{0, 0, 0},						"\xFD", 0, I86},

	{"sti",		0,	{0, 0, 0},						"\xFB", 0, I86},
																
//	{"stos",	1,	{MEM|BITS8, 0, 0},					"\xAA", 0, I586},
//	{"stos",	1,	{MEM|BITS16, 0, 0},					"\xAB", 0, I586},
//	{"stos",	1,	{MEM|BITS32, 0, 0},					"\xAB", 0, I586},
	{"stosb",	0,	{0, 0, 0},						"\xAA", 0, I86},
	{"stosw",	0,	{0, 0, 0},						"\xAB", 0, I86},
	{"stosd",	0,	{0, 0, 0},						"\xAB", 0, I386},

	{"str",		1,	{REGMEM|BITS16, 0, 0},					"\x0F\x00", D1, I286},

	{"sub",		2,	{AL_REG, IMM|BITS8, 0},					"\x2C", 0, I86},
	{"sub",		2,	{AX_REG, IMM|BITS16, 0},				"\x2D", 0, I86},
	{"sub",		2,	{EAX_REG, IMM|BITS32, 0},				"\x2D", 0, I86},
	{"sub",		2,	{REGMEM|BITS8, IMM|BITS8, 0},				"\x80", D5, I86},
	{"sub",		2,	{REGMEM|BITS16, IMM|BITS16, 0},				"\x81", D5, I86},
	{"sub",		2,	{REGMEM|BITS32, IMM|BITS32, 0},				"\x81", D5, I86},
	{"sub",		2,	{REGMEM|BITS16, IMM|BITS8, 0},				"\x83", D5, I86},
	{"sub",		2,	{REGMEM|BITS32, IMM|BITS8, 0},				"\x83", D5, I86},
	{"sub",		2,	{REGMEM|BITS8, REG|BITS8, 0},				"\x28", R|MODRM, I86},
	{"sub",		2,	{REGMEM|BITS16, REG|BITS16, 0},				"\x29", R|MODRM, I86},
	{"sub",		2,	{REGMEM|BITS32, REG|BITS32, 0},				"\x29", R|MODRM, I86},
	{"sub",		2,	{REG|BITS8, REGMEM|BITS8, 0},				"\x2A", R|MODRM, I86},
	{"sub",		2,	{REG|BITS16, REGMEM|BITS16, 0},				"\x2B", R|MODRM, I86},
																		{INS_SUB,	2,	{REG|BITS32, REGMEM|BITS32, 0},		"\x2B", R|MODRM, I86},
	{"test",	2,	{AL_REG, IMM|BITS8, 0},					"\xA8", 0, I86},
	{"test",	2,	{AX_REG, IMM|BITS16, 0},				"\xA9", 0, I86},
	{"test",	2,	{EAX_REG, IMM|BITS32, 0},				"\xA9", 0, I86},
	{"test",	2,	{REGMEM|BITS8, IMM|BITS8, 0},				"\xF6", D0, I86},
	{"test",	2,	{REGMEM|BITS16, IMM|BITS16, 0},				"\xF7", D0, I86},
	{"test",	2,	{REGMEM|BITS32, IMM|BITS32, 0},				"\xF7", D0, I86},
	{"test",	2,	{REGMEM|BITS8, REG|BITS8, 0},				"\x84", R|MODRM, I86},
	{"test",	2,	{REGMEM|BITS16, REG|BITS16, 0},				"\x85", R|MODRM, I86},
	{"test",	2,	{REGMEM|BITS32, REG|BITS32, 0},				"\x85", R|MODRM, I86},

	{"verr",	1,	{REGMEM|BITS16, 0, 0},					"\x0F\x00", D4, I286},
	{"verw",	1,	{REGMEM|BITS16, 0, 0},					"\x0F\x00", D4, I286},

	{"wait",	0,	{0, 0, 0},						"\x9B", 0, I86},

	{"wbinvd",	0,	{0, 0, 0},						"\x0F\x09", 0, I486},

	{"wrmsr",	0,	{0, 0, 0},						"\x0F\x30", 0, I586},
														
	{"xadd",	2,	{REGMEM|BITS8, REG|BITS8, 0},				"\x0F\xC0", R|MODRM, I486},
	{"xadd",	2,	{REGMEM|BITS16, REG|BITS16, 0},				"\x0F\xC1", R|MODRM, I486},
	{"xadd",	2,	{REGMEM|BITS32, REG|BITS32, 0},				"\x0F\xC1", R|MODRM, I486},

	{"xchg",	2,	{AX_REG, REG|BITS16, 0},				"\x90", RW, I86},
	{"xchg",	2,	{REG|BITS16, AX_REG, 0},				"\x90", RW, I86},
	{"xchg",	2,	{EAX_REG, REG|BITS32, 0},				"\x90", RD, I86},
	{"xchg",	2,	{REG|BITS32, EAX_REG, 0},				"\x90", RD, I86},
	{"xchg",	2,	{REGMEM|BITS8, REG|BITS8, 0},				"\x86", R|MODRM, I86},
	{"xchg",	2,	{REG|BITS8, REGMEM|BITS8, 0},				"\x86", R|MODRM, I86},
	{"xchg",	2,	{REGMEM|BITS16, REG|BITS16, 0},				"\x87", R|MODRM, I86},
	{"xchg",	2,	{REG|BITS16, REGMEM|BITS16, 0},				"\x87", R|MODRM, I86},
	{"xchg",	2,	{REGMEM|BITS32, REG|BITS32, 0},				"\x87", R|MODRM, I86},
	{"xchg",	2,	{REG|BITS32, REGMEM|BITS32, 0},				"\x87", R|MODRM, I86},

	{"xlat",	1,	{MEM|BITS8, 0, 0},					"\xD7", 0, I86},
	{"xlatb",	0,	{0, 0, 0},						"\xD7", 0, I586},

	{"xor",		2,	{AL_REG, IMM|BITS8, 0},					"\x34", 0, I86},
	{"xor",		2,	{AX_REG, IMM|BITS16, 0},				"\x35", 0, I86},
	{"xor",		2,	{EAX_REG, IMM|BITS32, 0},				"\x35", 0, I86},
	{"xor",		2,	{REGMEM|BITS8, IMM|BITS8, 0},				"\x80", D6, I86},
	{"xor",		2,	{REGMEM|BITS16, IMM|BITS16, 0},				"\x81", D6, I86},
	{"xor",		2,	{REGMEM|BITS32, IMM|BITS32, 0},				"\x81", D6, I86},
	{"xor",		2,	{REGMEM|BITS16, IMM|BITS8, 0},				"\x83", D6, I86},
	{"xor",		2,	{REGMEM|BITS32, IMM|BITS16, 0},				"\x83", D6, I86},
	{"xor",		2,	{REGMEM|BITS8, REG|BITS8, 0},				"\x30", R|MODRM, I86},
	{"xor",		2,	{REGMEM|BITS16, REG|BITS16, 0},				"\x31", R|MODRM, I86},
	{"xor",		2,	{REGMEM|BITS32, REG|BITS32, 0},				"\x31", R|MODRM, I86},
	{"xor",		2,	{REG|BITS8, REGMEM|BITS8, 0},				"\x32", R|MODRM, I86},
	{"xor",		2,	{REG|BITS16, REGMEM|BITS16, 0},				"\x33", R|MODRM, I86},
	{"xor",		2,	{REG|BITS32, REGMEM|BITS32, 0},				"\x33", R|MODRM, I86},
	{INS_END,	0,	{0, 0, 0},						"", 0, 0},
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

//
// void I80X86::WriteInstruction()
//
// Description
//
// Parameters
//
// Returns
//
void I80X86::WriteInstruction()
{
};
