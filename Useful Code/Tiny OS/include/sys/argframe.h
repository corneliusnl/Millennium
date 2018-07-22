/*
 * this is what process data actually contains at the beginning
 *
 * $Id$
 *
 */
#ifndef _ARGFRAME_H_
#define _ARGFRAME_H_


#include <types.h>
#include <sys/arg.h>

struct arg_frame {
	ulong	magic;
	ulong	argc;
	char	args[MAX_ARGSZ];	/* packed argv */
	char*	argv[MAX_ARGNO];
};

#endif /* _ARGFRAME_H_ */
