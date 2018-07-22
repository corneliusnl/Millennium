// ------------------------------------------------------------
// Standard Input/Output
// Copyright 1997 (c) Leprechaun Software. All Rights Reserved.
// ------------------------------------------------------------

#ifndef _HCC_STDIO
#define _HCC_STDIO

#define FILE	unsigned long
#define EOF		-1L
#define NULL	0L

#define stdin		1
#define stdout		2
#define stderr		2

#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2

extern FILE *fopen	(const char *filename, char *mode);
extern long *fclose	(FILE *stream);
extern long remove	(const char *filename);
extern long rename	(const char *oldname, const char *newname);
extern long fgetc		(FILE *stream);
extern char *fgets	(char *s, int n, FILE *stream);
extern long fputc		(int c, FILE *stream);
extern long fputs		(const char *s, FILE *stream);
extern long printf	(char *fmt, ...);
extern long sprintf	(char *buffer, const char *format, ...);
extern long fprintf	(FILE *stream, const char *format, ...);
extern long ftell		(FILE *stream);
extern void rewind	(FILE *stream);
extern void fseek		(FILE *stream, long offset, int whence);
extern long fread		(void *ptr, size_t size, long n, FILE *stream);
extern long fwrite	(char *ptr, size_t size, long n, FILE *stream);

#endif
