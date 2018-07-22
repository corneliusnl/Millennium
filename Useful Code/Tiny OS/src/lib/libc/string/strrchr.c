/*
 * Copyright (c) 1996 Andy Valencia
 *
 * strrchr()
 * Like strchr(), but search backwards
 */
char *
strrchr(const char *p, int c)
{
   char *q = 0, c2;

   do {
      c2 = *p++;
      if (c == c2) {
         q = (char *)p;
      }
   } while (c2);
   return(q ? (q-1) : 0);
}
