/*
 * buffer cache stuff
 *
 * $Id$
 *
 */
#ifndef _BUF_H_
#define _BUF_H_

#include <types.h>

struct buffer_head {
   uint                 id;
   uint			sz;
   uint 		cnt;
   uchar*               pbuf;
   uint                 dev;
   uint                 blk;
   uint                 lock;
   uint                 status;
   struct buffer_head*  prev;
   struct buffer_head*  next;
   struct buffer_head*  prev_free;
   struct buffer_head*  next_free;
};

typedef int (*rdblkfun) (struct buffer_head*);
typedef int (*wrblkfun) (struct buffer_head*);

/*
 * protos
 */
struct buffer_head*	getblk(uint dev, uint blk, uint id);
struct buffer_head* 	bread(uint dev, uint blk, uint id);
void 			brelse(struct buffer_head* bh);
void			setbufop(uint id, rdblkfun, wrblkfun);

#endif /* _BUF_H_ */
