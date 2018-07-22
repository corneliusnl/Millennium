/*
 * from VSTa libc
 * Copyright (c) 1996 Andy Valencia
 *
 */
char *
strcat(char *dest, const char *src)
{
   char *p;

   for (p = dest; *p; ++p);
   while (*p++ = *src++);
   return(dest);
}
