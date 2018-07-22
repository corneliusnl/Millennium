#include <types.h>
#include <bcopy.h>
#include <ctype.h>
#include <string.h>
#include <sys/mem.h>
#include <stdio.h>
#include <fcntl.h>

//#define _DEBUG
#include <trace.h>

#define INIT_MSG	9999

#define MAX_ARGS	10
#define MAX_ARGSZ	50

int	argc = 0;
char	argv[MAX_ARGS][MAX_ARGSZ];

struct _FILE {
	int 	h;
	uint	eof;
	char* 	pos;
	uint 	cnt;
	char	buf[512];
};

#define EOF (-1)

struct _FILE file;

struct _FILE*
_fopen(char* name)
{
	file.h = open(name, O_RDONLY);
	if(file.h == -1)
		return (NULL);

	file.pos = file.buf;
	file.cnt = 0;
	file.eof = 0;
	bzero(file.buf, 512);

	/* read(file.h, file.buf, 512); */

	return (&file);
}

void
_fclose(struct _FILE* f)
{
	close(f->h);
}

int
fillbuf(struct _FILE* f)
{
	int rb;

	f->pos = f->buf;
	f->cnt = 0;

	if(f->eof)
		return (1);

	rb = read(f->h, f->buf, 512);

	if(rb <= 0) {
		f->eof = 1;
		return (1);
	}
		
	f->cnt = rb;
	return(0);
}

int
set_read(struct _FILE* f)
{
	if(f->cnt == 0) {
		if(fillbuf(f)) {
			return (1);
		}
	}
	return (0);
}

char
_fgetc(struct _FILE* f)
{
	uchar c;

	if(set_read(f)) {
		return(EOF);
	}

	c = *(f->pos);
	f->pos++;
	f->cnt--;

	return(c);
}

char*
_fgets(char* buf, int len, struct _FILE* f)
{
	int c = 0;
	char ch;
	char* p = buf;

	while((c++ < (len-1)) && ((ch = _fgetc(f)) != EOF)) {
		if(ch == '\r')
			continue;
		*p++ = ch;
		if(ch == '\n')
			break;
	}

	if(ch == EOF)
		return(0);
	*p++ = 0;
	return (buf);
}

char buf[200];
char* vas[5];

void*
pack_args()
{
	ulong c;
	char* args = buf;
	char* p = args;

	
	*(ulong*)p = argc;
	p += sizeof(ulong);
	
	for(c = 0; c < argc; c++) {
		uint len = strlen(argv[c])+1;
		bcopy(argv[c], p, len);
		vas[c] = (char*)p;
		p += len;
	}

	return((void*)args);
}

void
runproc(char* line)
{
	char proc[100];
	char* p = line;
	char* d = proc;

	while(*p && *p != '\n' &&  *p != ' ' && *p != ':')
		*d++ = *p++;
	*d = 0;

	if(*proc) {
		ulong pid;

		TRACE1("launching: %s ... ", proc);
		argc = 1;
		strcpy(argv[1], proc);
		
		//pid = exec(proc, pack_args());
		pid = execl(proc, NULL);

		if(pid == 0)
			printf("init: cannot exec process: %s\n", proc);
	}
}	

void
main(int argc, char** argv)
{
	char buf[100];
	struct _FILE* f = NULL;

	if(chdir("../etc") <= 0) {
		printf("init: fatal, cannot change to /etc\n");
		exit();
	}

	f = _fopen("inittab");	

	if(f == NULL) {
		printf("init: fatal, cannot find inittab\n");
		exit();
	}

	if(chdir("../bin") <= 0) {
		printf("init: fatal, cannot get back to ..\n");
		exit();
	}
	
	TRACE("\nprocessing inittab...\n");
	while(_fgets(buf, 100, f)) {
		TRACE1("file: %s\n", buf);
		runproc(buf);
	}
	_fclose(f);

	while(1);

	/*
	while((wait(INIT_MSG, NULL, 0) != -1));
	*/
}
