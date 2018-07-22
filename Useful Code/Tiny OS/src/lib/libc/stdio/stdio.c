/* 
 * stdio routines
 * being adapted from VSTa libc
 * Copyright (c) 1996 Andy Valencia
 *
 *
 */
#include <types.h>
#include <bcopy.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mem.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

FILE*
fopen(const char* name)
{
   FILE* f = (FILE*)malloc(sizeof(FILE));

   if(f == NULL)
      return (NULL);

   f->h = open(name, O_RDONLY);
   if(f->h == -1)
      return (NULL);

   f->pos = f->buf;
   f->cnt = 0;
   f->eof = 0;
   bzero(f->buf, 512);

   /* read(file.h, file.buf, 512); */

   return (f);
}

void
fclose(FILE* f)
{
   close(f->h);
   free(f);
}

static int
fillbuf(FILE* f)
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

static int
set_read(FILE* f)
{
   if(f->cnt == 0) {
      if(fillbuf(f)) {
         return (1);
      }
   }
   return (0);
}

char
fgetc(FILE* f)
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
fgets(char* buf, int len, FILE* f)
{
   int c = 0;
   char ch;
   char* p = buf;

   while((c++ < (len-1)) && ((ch = fgetc(f)) != EOF)) {
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

/*
 * fread()
 * Read a buffer
 */
int
fread(void *buf, int size, int nelem, FILE *fp)
{
   char *p;
   uint len, x;
   int c;

   p = buf;
   len = size * nelem;
   x = 0;
   while (x < len) {
      c = fgetc(fp);
      if (c == EOF) {
         return(x / size);
      }
      *p++ = c;
      ++x;
   }
   return(nelem);
}
