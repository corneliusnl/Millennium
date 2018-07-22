/*
 * write() some
 *
 * $Id$
 *
 */

#include <unistd.h>

static int write_stdio(int h, const void* buf, size_t count);

int 
write(int h, const void* buf, size_t count)
{
	if(!buf || count <= 0)
		return (-1);

	if(h < 4)
		return (write_stdio(h, buf, count));

	return(os_write(h, buf, count));
}

static int
write_stdio(int h, const void* buf, size_t count)
{
	switch(h) {
		case 0: /* STDIN */
			break;
		case 1: /* STDOUT */
			printf(buf);
			break;
		case 3: /* STDERR */
			printf(buf);
			break;
	}
}
