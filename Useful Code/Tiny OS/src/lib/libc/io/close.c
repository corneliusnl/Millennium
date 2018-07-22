/*
 * close()
 *
 * $Id$
 *
 */

#include <unistd.h>

int 
close(int h)
{
	if(h < 0)
		return (-1);

	return(os_close(h));
}
