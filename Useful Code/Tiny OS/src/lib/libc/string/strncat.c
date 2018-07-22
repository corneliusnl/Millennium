/*
 * Copyright (c) 1996 Andy Valencia
 *
 */

char *
strncat(char *dest, const char *src, int len)
{
   char *p;
   const char *lim;

   lim = src+len;
   for (p = dest; *p; ++p)
      ;
   while (src < lim) {
      if ((*p++ = *src++) == '\0') {
         return(dest);
      }
   }
   *p++ = '\0';
   return(dest);
}
