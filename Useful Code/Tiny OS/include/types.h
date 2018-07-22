/*
 * useful types
 * 
 */
#ifndef _TYPES_H_
#define _TYPES_H_

typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;
typedef unsigned long ulong;
typedef long off_t;
typedef ulong size_t;
typedef long time_t;
typedef ulong mode_t;
typedef ulong nlink_t;
typedef ulong dev_t;
typedef ulong ino_t;
struct time {
	long t_sec;
	long t_usec;
};
typedef long pid_t;
typedef void (*voidfun)();
typedef int (*intfun)();
typedef ulong uid_t, gid_t;

typedef uchar  byte;
typedef ushort word;
typedef ulong  dword;

#ifndef NULL
#define NULL (0)
#endif

#endif /* _TYPES_H_ */
