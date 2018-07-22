#ifndef _STDIO_H_
#define _STDIO_H_

#include <types.h>

#define putchar putc

#define STDIO_BUFSZ	1024
typedef struct {
	int	h;
	uint	eof;
	char*	pos;
	uint 	cnt;
	char	buf[STDIO_BUFSZ];
} FILE;

#define EOF (-1)

#ifdef __cplusplus
extern "C" {
#endif

void putc(unsigned char);
void ungetc();
void printf(const char*,...);
FILE* fopen(const char*);
void  fclose(FILE*);
char  fgetc(FILE*);
char* fgets(char* buf, int len, FILE*);
int   fread(void* buf, int size, int nelem, FILE*);


#ifdef __cplusplus
}
#endif

#endif /* _STDIO_H_ */


