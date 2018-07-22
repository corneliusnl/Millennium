/*
 * open() file
 *
 * $Id$
 *
 */

#include <fcntl.h>
#include <stdarg.h>

int
open(const char* file, int flags, ...)
{
	if(!file)
		return (-1);

   	if(flags & O_CREAT) {
		va_list args;
		int mode;
		
		va_start(args, flags);
		mode = va_arg(args, int);
		va_end(args);

		return(os_open(file, flags, mode));
	}

	return(os_open(file, flags, 0));
}
