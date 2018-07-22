/*
 * assert stuff
 *
 * $Header$
 * $Log$
 *
 */

#include <stdio.h>
#include <assert.h>

void
__assertion(const char *msg, const char *file, int line)
{
	printf("Assertion failed at %s line %d: %s\n", file, line, msg);
}
