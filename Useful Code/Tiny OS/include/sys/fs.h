/*
 * General disk and FAT filesystem structures
 * based on structures from VSTa DOS server
 * Copyright (c) 1996 Andy Valencia
 *
 */
#ifndef _FS_H_
#define _FS_H_

#include <types.h>

/*
 * partition definition
 */

struct part_desc {
   uchar boot;
   uchar head;
   uchar sec;
   uchar cyl;
   uchar type;
   uchar ehead;
   uchar esec;
   uchar ecyl;
   ulong start;
   ulong len;
};

/*
 * file control block
 */
struct fcb {
   uint  		fsys;		/* filesystem handle 		*/
   struct fcb*    	dir;		/* dir this file belongs to 	*/
   uint     		start_blk;	/* starting block 		*/
   uint    		cur_blk;	/* current block		*/
   uint     		cur_pos;	/* current position		*/
   int     		iblk;		/* ordinal block number		*/
   uint    		sz;		/* file size			*/
   uint    		type;		/* type ...			*/
   uint			mode;		/* and mode			*/
};

/*
 * file types
 */
#define IT_FILE      	(0x02)
#define IT_DIR    	(0x04)
#define IT_BAD    	(0xF0)

/*
 * file modes
 */
#define FM_READ		(0x02)
#define FM_WRITE	(0x04)

struct inode {
   ulong blk;
   ulong type;
   ulong perms;
   ulong time;
   ulong sz;
};

/*
 * filesystem descriptor
 */

typedef int (*read_func)  (struct fcb* f, void* p, int cnt);
typedef int (*write_func) (struct fcb* f, void* p, int cnt);
typedef int (*creat_func) ();
typedef int (*unlnk_func) ();
typedef int (*trunc_func) ();

struct fsys_desc {
   uint  type;
   uint  disk;
   uint  start;
   uint  len;
   uint  blksz;

   struct fcb  root;

   read_func   read;
   write_func  write;
   creat_func  creat;
   unlnk_func  unlink;
   trunc_func  trunc;
};

#define MAX_PATH  255
#define MAX_FILES 100

#endif /* _FS_H_ */
