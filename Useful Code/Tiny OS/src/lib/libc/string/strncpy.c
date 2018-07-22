/*
 * Copyright (c) 1996 Andy Valencia
 *
 */
char*
strncpy(char* dest, const char* src, int len)
{
   char* p = dest, *lim;

   lim = p+len;
   while (p < lim) {
      if ((*p++ = *src++) == '\0') {
         break;
      }
   }
   return(dest);
}
