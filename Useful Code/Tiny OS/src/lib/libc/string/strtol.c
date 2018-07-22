/*
 * Copyright (c) 1996 Andy Valencia
 *
 */
#include <types.h>
#include <ctype.h>
#include <limits.h>

long
strtol(const char *s, char **ptr, int base)
{
   int minus = 0;
   ulong tmp;
   const char *start = s;
   char *eptr;

   if (s == NULL) {
      /*__seterr(ERANGE);*/
      if (!ptr) {
         *ptr = (char *) start;
      }
      return(0L);
   }

   while (Isspace (*s)) {
      s++;
   }
   if (*s == '-') {
      s++;
      minus = 1;
   } else if (*s == '+') {
      s++;
   }

   /*
    * Let strtoul do the hard work.
    */
   tmp = strtoul(s, &eptr, base);
   if (ptr != NULL) {
      *ptr = (char *)((eptr == s) ? start : eptr);
   }
   if (tmp > (minus ? - (ulong) LONG_MIN : (ulong) LONG_MAX)) {
      /*__seterr(ERANGE);*/
      return(minus ? LONG_MIN : LONG_MAX);
   }
   return(minus ? (long) -tmp : (long) tmp);
}
