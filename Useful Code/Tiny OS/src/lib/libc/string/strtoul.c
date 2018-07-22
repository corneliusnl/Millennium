/*
 * Copyright (c) 1996 Andy Valencia
 *
 */
#include <types.h>
#include <ctype.h>
#include <limits.h>

ulong
strtoul(const char *s, char **ptr, int base)
{
   ulong total = 0;
   uint digit;
   int radix;
   const char *start = s;
   int did_conversion = 0;
   int overflow = 0;
   int minus = 0;
   ulong maxdiv, maxrem;

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

   radix = base;
   if (base == 0 || base == 16) {
      /*
       * try to infer radix from the string (assume decimal).
       * accept leading 0x or 0X for base 16.
       */
      if (*s == '0') {
         did_conversion = 1;
         if (base == 0) {
            radix = 8;  /* guess it's octal */
         }
         s++;        /* (but check for hex) */
         if (*s == 'X' || *s == 'x') {
            did_conversion = 0;
            s++;
            radix = 16;
         }
      }
   }
   if (radix == 0) {
      radix = 10;
   }
   
   maxdiv = ULONG_MAX / radix;
   maxrem = ULONG_MAX % radix;

   while ((digit = *s) != 0) {
      if (digit >= '0' && digit <= '9' && digit < ('0' + radix)) {
         digit -= '0';
      } else if (radix > 10) {
         if (digit >= 'a' && digit < ('a' + radix - 10)) {
            digit = digit - 'a' + 10;
         } else if (digit >= 'A' &&
               digit < ('A' + radix - 10)) {
            digit = digit - 'A' + 10;
         } else {
            break;
         }
      } else {
         break;
      }
      did_conversion = 1;
      if (total > maxdiv || (total == maxdiv && digit > maxrem)) {
         overflow = 1;
      }
      total = (total * radix) + digit;
      s++;
   }
   if (overflow) {
      /*__seterr(ERANGE);*/
      if (ptr != NULL) {
         *ptr = (char *) s;
      }
      return(ULONG_MAX);
   }
   if (ptr != NULL) {
      *ptr = (char *) ((did_conversion) ? (char *) s : start);
   }
   return(minus ? - total : total);
}
