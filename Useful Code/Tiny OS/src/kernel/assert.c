/*
 * assert stuff
 *
 * $Id: ASSERT.C 1.1 1996/11/26 15:39:32 bart Exp $
 *
 */

#include <stdio.h>
#include <assert.h>

void
__assertion(const char* msg, const char* file, int line)
{
   printf("Assertion failed at %s line %d: %s\n", file, line, msg);
   cli();
   while(1);
}
