/*
 * lseek()
 *
 * $Id$
 *
 */

#include <unistd.h>

int 
seek(int h, uint pos)
{
	return(os_seek(h, pos));
}

off_t 
lseek(int h, off_t offset, int whence)
{
	return(-1);
}

