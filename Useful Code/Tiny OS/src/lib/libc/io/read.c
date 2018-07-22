/*
 * read() some
 *
 * $Id$
 *
 */

#include <unistd.h>

static int read_stdio(int h, void* buf, size_t count);

int 
read(int h, void* buf, size_t count)
{
	if(!buf || count <= 0)
		return (-1);

	if(h < 4)
		return (read_stdio(h, buf, count));

	return(os_read(h, buf, count));
}

static int
read_stdio(int h, void* buf, size_t count)
{
	switch(h) {
		case 0: /* STDIN */
			break;
		case 1: /* STDOUT */
			break;
		case 3: /* STDERR */
			break;
	}
}
