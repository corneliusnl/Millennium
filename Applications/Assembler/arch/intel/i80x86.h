#ifndef MOSA_80X86_H
#define MOSA_80X86_H
//
// Millennium Operating System Assembler
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//
class ARCHITECTURE;

//
// Register structure
//
typedef struct
{
	char	*String;
	int	RegCode;
	long	Type;

} REGISTER;

//
// Instruction structure 
// 
typedef struct
{
	char	*OpCode;
	int	NumOperands;
	long	OpType[3];
	char	*MachCode;
	long	OpInfo;
	long	Chip;
} INSTRUCTION;

//
// Operand type
// 
#define	BITS8	0x00000001
#define BITS16	0x00000002
#define BITS32	0x00000004
#define BITS64	0x00000008
#define BITS80	0x00000010

#define AL_REG	0x00000100 | REG | BITS8
#define AX_REG	0x00000200 | REG | BITS16
#define EAX_REG	0x00000400 | REG | BITS32
#define DX_REG	0x00000800 | REG | BITS16

#define REG	0x00010000
#define MEM	0x00020000
#define IMM	0x00040000
#define SREG	0x00080000
#define REGMEM	REG | MEM

//
// Operand info
// 
// D0...D7	Specifies that the reg field contains that digit
// CB, CW, CD	Specify code offset
// R		Indicates that the operand has both a register and r/m
// 		operands
// RB, RW, RD	Specifies that a register code is to be added to final opcode
// 		byte
// I		Indicates that the number I is to be added to the final opcode
// 		byte
//
#define D0	0x00000001
#define D1	0x00000002
#define D2	0x00000004
#define D3	0x00000008
#define D4	0x00000010
#define D5	0x00000020
#define D6	0x00000040
#define D7	0x00000080
#define R	0x00000100
#define M64	0x00000200
#define RB	0x00000400
#define RW	0x00000800
#define RD	0x00001000
#define CB	0x00002000
#define CW	0x00004000
#define CD	0x00008000
#define I	0x00010000
#define MODRM	0x10000000

//
// Machine code
//
#define I86	0x00000001
#define I186	0x00000002 | I86
#define I286	0x00000004 | I186
#define I386	0x00000008 | I286
#define I486	0x00000010 | I386
#define I586	0x00000020 | I486
#define I686	0x00000040 | I586
#define MMX	0x00000080

//
// Intel 80x86 architecture class
//
class I80X86 : public ARCHITECTURE
{
private:
	INSTRUCTION	*Instruction;
	REGISTER	*Register;
	char		*Special[];
	
public:

	int		LookupSpecial(char *Name);
	int		LookupInstruction(char *Name);
	int		LookupRegister(char *Name);
	void		ParseInstruction();
	void		ParseSpecial();
};

#endif
