#ifndef _FCNTL_H_
#define _FCNTL_H_

#include <types.h>

/*
 * open flags
 */
#define O_RDONLY 	0x0001
#define O_WRONLY 	0x0002
#define O_RDWR 		0x0003
#define O_TEXT 		0x0004
#define O_BINARY 	0x0008
#define O_CREAT 	0x0100
#define O_TRUNC 	0x0400
#define O_EXCL 		0x0800
#define O_APPEND 	0x1000

#ifdef __cplusplus
extern "C" {
#endif

int open(const char *, int, ...);
int creat(const char *, int);

#ifdef __cplusplus
}
#endif

#endif /* _FCNTL_H_ */

