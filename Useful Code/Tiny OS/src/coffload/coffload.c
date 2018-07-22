/*
	COFF Image Loader
	Copyright (c) 1996 Bart Sekura

	This piece of code reads coff executable image,
	puts its data at 0 physical, code at 0x20000 physical,
	turns on A20, enters protected mode and jumps to specified
	entry address. ( for now fixed at 0x20000 )

	This file contains pieces of code from MMURTL operating
	system DOS loader code 
	Copyright (c) 1995 Richard A. Burgess

	
	Limitations: code seg size <= 128KB
		     data seg size <= 128KB


	XXX TODO:
	- make it more parametrized, e.g. target addresses and
	  entry point
	- add support for bigger data and code segments
*/

	
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dos.h>
#include <dir.h>
#include <conio.h>
#include "coff.h"

#include "../../include/boot/bootp.h"

char *ptest;

char osname[20] = "";

FILE* run_fh = NULL;

unsigned char bootdrive;

unsigned long code_ptr = 0;
unsigned long code_sz  = 0;
unsigned long data_ptr = 0;
unsigned long data_sz  = 0;

/*
unsigned long basemem = 0;
unsigned long extmem = 0;
*/

FILE* hcode;
FILE* hdata;

/*
 * command line switches
 */
int dump = 0;
int silent = 0;

#define MAX_CHUNKS	3

void read_scs(void);
void load_code(void);
void load_data(void);
void load_coff(void);
void banner(void);

void
banner()
{
	printf("COFF image loader 1.1 (c) 1996 Bart Sekura\n");
	printf("based on MMRTL loader, (c) 1995 Richard A. Burgess\n");
}

/**** Execute ************************************************
   At this point:
     Interrupts are disabled,
     The code and data are waiting to be moved down to 0 linear
	 So we move it, then we tell the processor the size and
	 location of the IDT and GDT (with the SIDT and SGDT
	 insturctions), then set the protected mode bit and
	 jump to the OS code!
     This code can't access any data segment variables.
     It must all be self contained because we relocate it.
**************************************************************/

void Execute(void)
{

/* We're pulling some REAL cheating here in real mode
to get this done.  We set DS up to where we moved the
variables we need to use. We did this so we wouldn't
overwrite them. Then we move the OS code & data DOWN
and DO IT!  This code ends up at 9000:0
*/

asm {	.386P
		NOP
		CLI

		MOV BX, 9800h
		MOV DS, BX
		XOR BX, BX
		MOV WORD PTR DS:[BX], 07FFh
		MOV BX, 2
		MOV WORD PTR DS:[BX], 0h
		MOV BX, 4
		MOV WORD PTR DS:[BX], 0h

		MOV BX, 6
		MOV WORD PTR DS:[BX], 017FFh
		MOV BX, 8
		MOV WORD PTR DS:[BX], 0800h
		MOV BX, 10
		MOV WORD PTR DS:[BX], 0h


		;MOV BX, 0B800h
		;MOV ES, BX
		;MOV BX, 0
		;MOV WORD PTR ES:[BX], 4731h

		/* Set up our new DS to where we moved the data */
		/* We must do this before each 32K load cause we use DS */


/* DATA */	/* Move first 32K data chunk */

		MOV BX, 06000h
		MOV DS, BX
		XOR SI, SI
		MOV AX,0000h
		MOV ES,AX
		XOR DI,DI
		MOV CX,8000h
		CLD                    	;
		REP MOVSB				;

		/* Move second 32K data chunk */

		MOV BX, 06800h
		MOV DS, BX
		XOR SI, SI
		MOV AX,0800h
		MOV ES,AX
		XOR DI,DI
		MOV CX,8000h
		CLD                    	;
		REP MOVSB				;

		/* Move third 32K data chunk */

		MOV BX, 07000h
		MOV DS, BX
		XOR SI, SI
		MOV AX,1800h
		MOV ES,AX
		XOR DI,DI
		MOV CX,8000h
		CLD                    	;
		REP MOVSB				;

/* CODE */	/* Move first 32K code chunk */

		MOV BX, 07800h
		MOV DS, BX
		XOR SI, SI
		MOV AX,2000h
		MOV ES,AX
		XOR DI,DI
		MOV CX,8000h
		CLD                    	;
		REP MOVSB				;

		/* Move second 32K code chunk */

		MOV BX, 08000h
		MOV DS, BX
		XOR SI, SI
		MOV AX,2800h
		MOV ES,AX
		XOR DI,DI
		MOV CX,8000h
		CLD                    	;
		REP MOVSB				;

		/* Move third 32K code chunk */

		MOV BX, 08800h
		MOV DS, BX
		XOR SI, SI
		MOV AX,3000h
		MOV ES,AX
		XOR DI,DI
		MOV CX,8000h
		CLD                    	;
		REP MOVSB				;


		;MOV BX, 0B800h
		;MOV ES, BX
		;MOV BX, 2
		;MOV WORD PTR ES:[BX], 4732h

		MOV BX, 9800h
		MOV DS, BX
		XOR BX, BX

		LIDT FWORD PTR DS:[BX+0]
		LGDT FWORD PTR DS:[BX+6]

		MOV EAX,CR0
		OR AL,1
		MOV CR0,EAX
		JMP $+4
		NOP
		NOP
		NOP
		NOP

        	MOV BX, 10h
		MOV DS,BX
		MOV ES,BX
		MOV FS,BX
		MOV GS,BX
		MOV SS,BX
		DB 66h
		DB 67h
		DB 0EAh
		DD 20000h
		DW 8h
		RETN
	} /* end of assembler */

}


/**** TurnOnA20 ****************************************************
    This turns on the address line 20 Gate. This must be done before
    we can physically address above 1 Meg.  If your machine only
    shows one Meg of RAM and you know there's more, this isn't
    working on your machine... (R&D time..)
********************************************************************/

void TurnOnA20(void)
{

asm {
		CLI
		XOR CX,CX
	}

IBEmm0:

asm {
		IN AL,64h
		TEST AL,02h
		LOOPNZ IBEmm0
		MOV AL,0D1h
		OUT 64h,AL
		XOR CX,CX
	}

IBEmm1:

asm {
		IN AL,64h
		TEST AL,02h
		LOOPNZ IBEmm1
		MOV AL,0DFh
		OUT 60h,AL
		XOR CX,CX
	}

IBEmm2:

asm {
		IN AL,64h
		TEST AL,02h
		LOOPNZ IBEmm2
	}
}

/*** Relocate *********************************************
This is a tricky little piece of code. We are going to
manipulate DOS memory we don't even own.
We are going to set up a temporary little environment
up at A0000h linear (A000:0 segmented) for our
data environment. And then at 90000h (9000:0) for
the loader code itself. This will become CS.
**********************************************************/

void 
Relocate(void)
{
char *pdest, *psource, **pnewp;
long *pnewl;
unsigned int *pneww;
int  i;

/* First we move the code up */

	psource = (void*) &Execute;

	printf("SEG: %u OFF: %u\r\n", FP_SEG(psource), FP_OFF(psource));

	pdest = MK_FP(0x9000,0);
	i = 2000;	/* more than enough for the code */
	while(i--)
		*pdest++ = *psource++;

}


void 
CallExecute(void)
{
/* We push the new code address on the stack then RETF to it.
   We PUSH CS then IP */

	asm {
		MOV AX,	09000h
		PUSH AX
		MOV AX, 0
		PUSH AX
		RETF
	}
}

void
error(char* msg)
{
	printf(msg);
	if (run_fh)
		fclose(run_fh);

	exit(-1);
}

void 
read_scs()
{
	SCNHDR scn;

	if( !fread(&scn, sizeof(SCNHDR), 1, run_fh))
		error("read failed for section.\n");
	
	if( !silent)
		printf("section: %s, size: %lu, ptr: %lu, vaddr: %lu\n", 
			scn.s_name, scn.s_size, scn.s_scnptr, scn.s_vaddr);

#ifdef DEBUG
	printf("section name: %s\n", scn.s_name);
	printf("section size: %lu\n", scn.s_size);
	printf("section ptr : %lu\n", scn.s_scnptr);
	printf("section vaddr : %lu\n", scn.s_vaddr);
	printf("section paddr : %lu\n", scn.s_paddr);
#endif

	if( ! strcmp(scn.s_name, ".text")) {
		code_ptr = scn.s_scnptr;
		code_sz  = scn.s_size;
	}
	else
	if( ! strcmp(scn.s_name, ".data")) {
		data_ptr = scn.s_scnptr;
		data_sz = scn.s_size;
	}
}

void
load_section(unsigned long filp, unsigned long sz, unsigned short p)
{
	long nobj, nread;
	char *pin;
	int c;

	fseek(run_fh, filp, SEEK_SET);

	for(c = 0; c < MAX_CHUNKS; c++) {
		nread = (sz >= 32768) ? 32768 : sz;
		sz -= nread;
		pin = MK_FP(p, 0);
		if( !silent) 
			printf("reading 32K chunk at: %x\n", p);
		if( !fread(pin, 1, nread, run_fh))
			error("read failed.\n");
		if(sz <= 0)
			break;

		p += 0x800;
	}
	if( !silent)
		printf("ok, section read successfully...\n");
}

void
load_coff()
{
	FILHDR hdr;
	AOUTHDR aout;
	int c;
	char* pin;
	char ch;

	if( !fread(&hdr, sizeof(FILHDR), 1, run_fh))
		error("read failed for file header.\n");
	if( !fread(&aout, sizeof(AOUTHDR), 1, run_fh))
		error("read failed for a.out header.\n");

	if( !silent) {
		printf("header:	magic[%d], no. of sections[%d]\n", 
			hdr.f_magic, hdr.f_nscns);

		printf("aout.tsize %lu\n", aout.tsize);
		printf("aout.dsize %lu\n", aout.dsize);
		printf("aout.bsize %lu\n", aout.bsize);
		printf("aout.entry %lu\n", aout.entry);
		printf("aout.text_start %lu\n", aout.text_start);
		printf("aout.data_start %lu\n", aout.data_start);
	}

	for(c = 0; c < hdr.f_nscns; c++)
		read_scs();

	if( dump ) {
		hcode = fopen("CODE.DMP", "wb");
    		hdata = fopen("DATA.DMP", "wb");
	}

	if( !silent)
		printf("reading code...\n");
	load_section(code_ptr, code_sz, 0x7800);

	if( !silent)
		printf("reading data...\n");
	load_section(data_ptr, data_sz, 0x6000);

	if( dump ) {
		pin = MK_FP(0x7800,0);
		fwrite(pin, 1, code_sz, hcode);
		pin = MK_FP(0x6000,0);
		fwrite(pin, 1, data_sz, hdata);

		fclose(hcode);
    		fclose(hdata);
	}

	fclose(run_fh);

    	printf("any key to launch...('q' quits...)\n");
	ch = getch();
	if( ch == 'q' || ch == 'Q' )
		return;

	TurnOnA20();
	Relocate();			/* Move the loader code & data */
	CallExecute();
}

void
params(char* file)
{
	char* pin, *p;
	char buf[201];
	struct boot_params* bootp;
	FILE* f;

	/*
 	 * setup deafult stuff
	 */
	pin = MK_FP(0x8F00,0);
	bootp = (struct boot_params*)pin;
	bootp->magic = 0xDEADBEEF;
	getcwd(buf, 200);
	strcpy(bootp->path, strchr(buf, ':') + 1);
	p = bootp->path;
	while(*p) {
		if(*p == '\\')
			*p = '/';
		p++;
	}
	printf("bootp->path = %s\n", bootp->path);
	/*
	 * just in case 
	 */
	bootp->part_no = 0;

	/*
	 * now get params from bootp file
	 */
	f = fopen(file ? file : "..\\etc\\bootp", "r");
	if(!f) {
		printf("params(): bootp open failed, using defaults...\n");
		return;
	}
	while(fgets(buf, 100, f)) {
		char* val = strchr(buf, ':');
		val++;
		while(*val == ' ') val++;
		/*printf("val: %s\n", val);*/
		if(!strncmp(buf, "part", 4)) {
			bootp->part_no = atoi(val);
			/*printf("part_no = %d\n", bootp->part_no);*/
		}
	}
	fclose(f);
}

/*
 * main 
 */
void 
main(int argc, char** argv)
{
	char* p;
	char *bootp_file = NULL;
	int c;

	for(c = 1; c < argc; c++) {
		p = argv[c];
		if (*p == '-') {
		  p++;
		  switch(*p) {
			case 'S' :
			case 's' :
				silent = 1;
				break;
			case 'D' :
			case 'd' :
				dump = 1;
				break;
			case 'b' :
			case 'B' :
				bootp_file = p + 1;
				break;
			default:
				error("Invalid option/switch \n");
				break;
		  }
		}
		else {
			strncpy(osname, argv[c], 39);
		}
	}

	banner();
	if( *osname == NULL )
		error("specify file name.\n");
	printf("booting: %s \r\n",  osname);
	run_fh = fopen(osname, "rb");
	if (!run_fh)
		error("Can't open specified filename\r\n");
	if (osname[1] == ':')
	{
		bootdrive = osname[0] - 0x41;
		if (bootdrive >= 0x20)
			bootdrive -= 0x20;	/* may be lower case */
	}
	else
	   bootdrive = getdisk();

	params(bootp_file);

	/*
	ptest = malloc(100);
	printf("First MEM = SEG: %u OFF: %u\r\n", 
		FP_SEG(ptest), FP_OFF(ptest));
	*/

	load_coff();
}
