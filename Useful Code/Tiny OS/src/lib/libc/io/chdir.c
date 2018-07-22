/*
 * chdir()
 *
 * $Id$
 *
 */

#include <unistd.h>

int 
chdir(const char* dir)
{
	if(!dir)
		return(-1);

	return(os_chdir(dir));
}

