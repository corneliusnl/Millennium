/*
 * Copyright (c) 1996 Andy Valencia
 *
 * strchr()
 * Return pointer to first occurence, or 0
 */
char *
strchr(const char *p, int c)
{
   int c2;

   do {
      c2 = *p++;
      if (c2 == c) {
         return((char *)(p-1));
      }
   } while (c2);
   return(0);
}
