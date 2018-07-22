/*
 * system calls wrappers
 *
 * $Id$
 *
 */

#include <sys/calls.h>

int 
getch(void)
{
	return(os_getch());
}

int 
allocmsg(ulong msg)
{
	return(os_allocmsg(msg));
}

int 
connectmsg(ulong msg)
{
	return(os_connectmsg(msg));
}

int 
wait(int port, void* buf, size_t count)
{
	if(buf == NULL)
		return(-1);

	return(os_wait(port, buf, count));
}

int 
sendmsg(int port, void* buf, size_t count)
{
	if(buf == NULL)
		return(-1);

	return(os_sendmsg(port, buf, count));
}

int 
waitpid(ulong pid)
{
	return(os_waitpid(pid));
}

void* 
mmap(size_t npg, ulong flags)
{
	return((void*)os_mmap(npg, flags));
}

int 
munmap(void* ptr, size_t npg, ulong flags)
{
	if(!ptr)
		return(-1);

	return(os_munmap(ptr, npg, flags));
}
