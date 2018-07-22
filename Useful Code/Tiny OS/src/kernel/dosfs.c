/*
 * DOS filesystem support
 *
 * $Id: DOSFS.C 1.8 1996/12/15 08:34:34 bart Exp $
 *
 */

#include <assert.h>
#include <bcopy.h>
#include <sys/wd.h>
#include <ctype.h>
#include <string.h>
#include <sys/dosfs.h>
#include <sys/fs.h>
#include <sys/proc.h>
#include <sys/buf.h>

//#define _DEBUG
#include <trace.h>
#undef _DEBUG

//#define VERIFY_FAT
#ifdef VERIFY_FAT
static ushort* pfat = 0;
#endif

/*
 * FAT magic numbers
 */
#define FAT_FREE     (0x0)
#define FAT_BAD      (0xFFF7)
#define FAT_EOF      (0xFFFF)

#define F_DELETED    (0xE5)

#define EOF(x)		((x) >= FAT_EOF)
#define NOT_EOF(x)	((x) < FAT_EOF)

/*
 * FAT params that never change
 */
#define  SECPER_ROOT    (32)
#define  ROOT_BLOCK     1
#define  DIRESZ         (32)

/*
 * we use threee block buffer types:
 * for FAT block
 * for root block and
 * for regular block
 */
#define BT_ROOT      (1)
#define BT_REGULAR   (2)
#define BT_FAT    (3)

#define BUF_POOL_MAX	(655360)
/*
 * and here's corresponding read/write functions
 */
int dosfs_read_fat(struct buffer_head*);
int dosfs_read_root(struct buffer_head*);
int dosfs_read_block(struct buffer_head*);
int dosfs_write_fat(struct buffer_head*);
int dosfs_write_root(struct buffer_head*);
int dosfs_write_block(struct buffer_head*);

static struct dos_boot dosb = {0};

/*
 * misc dosfs parameters
 */
static uint nde = 0; /* number of dir entries in cluster */
static uint fatstart = 0;
static uint fat2start = 0;
static uint fatsz = 0;
static uint fatpersec = 0; /* FAT entries per sector */
static uint fatperblk = 0; /* FAT entries per block  */
static uint fatblocks = 0; /* how many blocks FAT takes */
static uint fatblksz = 0;
static uint secperfatblk = 0;
static uint rootent = 0;
static uint rootsz  = 0;
static uint rootsec = 0;
static uint secbase = 0;
static uint dos_blksz = 0;

static uchar sect_buf[SECSZ];

#define INO_READ  (0x02)
#define INO_WRITE (0x04)

/*
 * protos
 */ 
static uint dosfs_balloc();
static int dosfs_inode(uint op, struct fcb* dir, struct dir_entry* dirent);
static int dosfs_chsize(struct fcb* f);
static int dosfs_chstart(struct fcb* f);
static uint dosfs_grow(uint last_blk);
int dosfs_seek(struct fcb* f, int pos);

/*
 * main initialization routine
 */
int
dosfs_init(struct fsys_desc* fs)
{
   uint c;

   /*
    * read MS-DOG superblock
    */ 
   reads((void*)sect_buf, fs->start, 1);
   bcopy(sect_buf, &dosb, sizeof(dosb));

   TRACE1("OEM    : %s\n", dosb.oem);
   TRACE1("bps    : %d\n", dosb.bps1 + (dosb.bps2 << 8));
   TRACE1("secperclust: %d\n", dosb.secperclust);
   TRACE1("nres   : %d\n", dosb.nres);
   TRACE1("nfat   : %d\n", dosb.nfat);
   TRACE1("rootent: %d\n", dosb.rootent1 + (dosb.rootent2 << 8));
   TRACE1("nsec   : %d\n", dosb.nsec1 + (dosb.nsec2 << 8));
   TRACE1("secperfat: %d\n", dosb.secperfat);
   TRACE1("secpertrk: %d\n", dosb.secpertrk);
   TRACE1("sides  : %d\n", dosb.sides);
   TRACE1("nhid   : %d\n", dosb.nhid);
   TRACE1("nsecbig: %d\n", dosb.nsecbig);

   /*
    * some sanity checks
    */
   if((dosb.secperclust == 0) || (dosb.secperclust > 64)) {
      printf("dosfs_init(): weird dosb.secperclust: %d\n", dosb.secperclust);
      return (-1);
   }

   fatsz = (dosb.secperfat * SECSZ) / sizeof(ushort);
   fatstart = fs->start + dosb.nres;
   fatpersec = SECSZ/sizeof(ushort);
   fat2start = fatstart + dosb.secperfat;

   fatblksz = 8192;
   secperfatblk = fatblksz / SECSZ;
   fatperblk = fatblksz / sizeof(ushort);
   fatblocks = fatsz / fatblksz;

   printf("FAT params: fatblks: %d; fatperblk: %d; fatblocks: %d\n",
           fatblksz, fatperblk, fatblocks);
   printf("2nd copy of FAT begins at: %d\n", fat2start);

#ifdef VERIFY_FAT
   /*
    * allocate mem from kernel heap for fat table
    */
   pfat = (ushort*)halloc(dosb.secperfat*SECSZ);
   reads((void*)pfat, fs->start + dosb.nres, dosb.secperfat);
   printf("fat sectors = %d\n", dosb.secperfat);
#endif

   /*
    * setup useful fs parameters
    */
   fs->blksz = dos_blksz = dosb.secperclust * SECSZ;

   nde = (dosb.secperclust * SECSZ) / DIRESZ;
   rootent = (dosb.rootent1 + (dosb.rootent2 << 8));
   rootsec = fs->start + dosb.nres + (dosb.nfat * dosb.secperfat);
   secbase = rootsec + SECPER_ROOT;
   rootsz = rootent * DIRESZ;

   printf("fatstart: %d, fatsz: %d, fatpersec: %d, secperfat: %d, rootsz: %d\n", 
           fatstart, fatsz, fatpersec, dosb.secperfat, rootsz);

   /*
    * setup root directory file
    */
   fs->root.dir = NULL; /* no parent */
   fs->root.start_blk = ROOT_BLOCK;
   fs->root.cur_blk = fs->root.start_blk;
   fs->root.iblk = 0;
   fs->root.cur_pos = 0;
   fs->root.sz = rootsz;

   /*
    * init buffer cache
    * we use 3 types of buffers: FAT, root and regular
    */
   c = BUF_POOL_MAX / dos_blksz;
   printf("dosfs_init(): we got %d buffers, %d bytes each...\n", c, dos_blksz);

   init_bufs(1, BT_ROOT, rootsz);
   init_bufs(c, BT_REGULAR, dos_blksz);
   init_bufs(16, BT_FAT, fatblksz);
   setbufop(BT_ROOT, dosfs_read_root, dosfs_write_root);
   setbufop(BT_REGULAR, dosfs_read_block, dosfs_write_block);
   setbufop(BT_FAT, dosfs_read_fat, dosfs_write_fat);

   return (1);
}


#ifdef VERIFY_FAT
static uint
_fatread(uint c)
{
   return(pfat[c]);
}
#define FAT(x) pfat[x]

static void
verify_fat(uint i, uint val, ushort* p)
{
   if(_fatread(i) != val) {
      uint c;
      printf("OOPS!. _fatread returned: %d, not %d\n", 
         _fatread(i), val);

      for(c = 0; c < 8; c++) {
         printf("%d:%d|", *(p+c), _fatread(i+c));
      }
      cli();
      while(1);
   }
}
#endif /* VERIFY_FAT */

/*
 * new buffered fatread
 */
static uint
fatread(uint i)
{
   uint val;
   struct buffer_head* bh;
   ushort* p;
   uint fatblk, off;

   if(i >= fatsz)
      panic("fatread() fatal!, trying to read beyond FAT size...\n");

   fatblk = i / fatperblk;
   off = i % fatperblk;

   bh = bread(1, fatblk, BT_FAT);
   p = (ushort*)bh->pbuf;
   val = p[off];

   //printf("fatread[%d]=%d\n", i, val1);
   //printf("bread......=%d\n", val2);

#ifdef VERIFY_FAT
   verify_fat(i, val, p);
#endif

   brelse(bh);
   return (val);
}

int
fatwrite(uint i, ushort val)
{
   struct buffer_head* bh;
   ushort* p;
   uint fatblk, off;
   ushort last;

   if(i >= fatsz)
      panic("fatread() fatal!, trying to read beyond FAT size...\n");

   fatblk = i / fatperblk;
   off = i % fatperblk;

   //printf("fatwrite(): fatblk: %d, off: %d\n", fatblk, off);
   //if(!kyesno())
   //   return (-1);

   bh = bread(1, fatblk, BT_FAT);
   p = (ushort*)bh->pbuf;

#ifdef VERIFY_FAT   
   verify_fat(i, p[off], p);
   pfat[i] = val;
#endif

   p[off] = val;

   buf_dirty(bh);
   brelse(bh);

   return (1);
}


/*
 * temporary test func
 */
uint
dosfs_testfat(uint i)
{
#ifdef VERIFY_FAT
   uint val1 = _fatread(i);
   uint val2;
   struct buffer_head* bh;
   ushort* p;
   uint fatblk, off;

   if(i >= fatsz) {
      printf("fatal!, trying to read beyond FAT size...\n");
      return(0);
   }

   fatblk = i / fatperblk;
   off = i % fatperblk;

   printf("fatblk: %d, off: %d\n", fatblk, off);
   
   bh = bread(1, fatblk, BT_FAT);
   p = (ushort*)bh->pbuf;
   val2 = p[off];

   printf("fatread[%d]=%d\n", i, val1);
   printf("bread......=%d\n", val2);

   brelse(bh);

   return (val2);
#else
   return (0);
#endif /* VERIFY_FAT */
}

/*
 * routines below are used by buffer cache
 * we pass them when initializing buffer cache
 */

int 
dosfs_read_fat(struct buffer_head* bh)
{
   //printf("@");
   ASSERT(bh->id == BT_FAT);
   reads(bh->pbuf, fatstart + (bh->blk * secperfatblk), secperfatblk);
}

int
dosfs_write_fat(struct buffer_head* bh)
{
   //printf("!");
   ASSERT(bh->id == BT_FAT);
   writes(bh->pbuf, fatstart + (bh->blk * secperfatblk), secperfatblk); 
   writes(bh->pbuf, fat2start + (bh->blk * secperfatblk), secperfatblk); 
}

int
dosfs_read_root(struct buffer_head* bh)
{
   //printf("dosfs_read_root()\n");
   ASSERT(bh->id == BT_ROOT);
   reads(bh->pbuf, rootsec, SECPER_ROOT);
}

int
dosfs_write_root(struct buffer_head* bh)
{
   //printf("dosfs_write_root()\n");
   ASSERT(bh->id == BT_ROOT);
   printf("WARNING! ROOT WAS ABOUT TO BE WRITTEN!!!\n");
   if(kyesno())
      writes(bh->pbuf, rootsec, SECPER_ROOT);
}

int
dosfs_read_block(struct buffer_head* bh)
{
   //printf("dosfs_read_block()\n");
   ASSERT(bh->id == BT_REGULAR);
   reads(bh->pbuf, secbase + ((bh->blk - 2) * dosb.secperclust), 
         dosb.secperclust);
}

int
dosfs_write_block(struct buffer_head* bh)
{
   //printf("dosfs_write_block()\n");
   ASSERT(bh->id == BT_REGULAR);
   writes(bh->pbuf, secbase + ((bh->blk - 2) * dosb.secperclust), 
          dosb.secperclust);
}

/*
 * construct a dosfs filename
 */
static void
dosfs_name(struct dir_entry* dirent, char* buf)
{
   char* p;
   char ext[4];

   strncpy(buf, dirent->name, 8);
   strncpy(ext, dirent->ext, 3);

   buf[8] = 0;
   ext[3] = 0;

   p = buf;
   while((*p != 0) && (*p != ' '))
      p++;

   if((*ext != 0) && (*ext != ' ')) {
      char* e = ext;
      *p++ = '.';
      while((*e != 0) && (*e != ' ')) {
         *p++ = *e++;
      }
   }
   *p = 0;
}

int dosfs_read(struct fcb*, void*, int);

/*
 * scans current directory of process 'p' looking for file name
 * upon success fills in fcb structure
 */
int
dosfs_lookup(struct fcb* dir, char* name, struct inode* res)
{
   struct dir_entry dirent;

   /*
    * scan dir from the beginning
    */
   dosfs_seek(dir, 0);

   /*
    * fast open current dir
    */
   if(name[0] == '.' && name[1] == 0) {
      bzero(res, sizeof(struct inode));
      res->blk = dir->start_blk;
      res->type = IT_DIR;
      res->sz = dir->sz;

      return(res->blk);
   }
   
   /*
    * look up the entry
    */
   while(dosfs_read(dir, (void*)&dirent, sizeof(struct dir_entry)) > 0) {
      char filename[20];
   
      /*
       * skip empty slots
       */
      if(dirent.name[0] == 0) {
         TRACE("dirent.name[0] = 0\n");
         break;
      }

      /*
       * skip deleted crap ...
       */
      if(dirent.name[0] == F_DELETED) {
         TRACE("F_DELETED\n");
         continue;
      }

      dosfs_name(&dirent, filename);
      TRACE2("FILENAME: %s, %s\n", dirent.name, filename);

      if(!strcmp(strlwr(filename), strlwr(name))) {
         TRACE1("Yeah, filename match for %s\n", name);
         TRACE2("dirent.start:%d, dirent.size:%d\n", 
                dirent.start, dirent.size);

	 /*
	  * ok, found it
	  */
         bzero(res, sizeof(struct inode));
         res->blk = dirent.start;
         if(dirent.start == 0)
         	res->blk = ROOT_BLOCK;

         if(dirent.attr & FA_DIREC) {
            res->type = IT_DIR;
         } else {
            res->type = IT_FILE;
         }

         res->sz = dirent.size;

         /*
          * rewind
          * XXX change that to seek() call
          */
         dir->cur_pos -= sizeof(struct dir_entry);
   
         return (res->blk);
      }
   }

   return(-1);
}

int
dosfs_read(struct fcb* f, void* p, int cb)
{
   uint blksz, blkno;
   uint offset, left;
   uint read = 0;
   uint blk;
   uint copy;
   uchar* psrc;
   struct buffer_head* bh;

   if(!f)
      return (-1);

   blksz = (f->cur_blk == ROOT_BLOCK) ? rootsz : dos_blksz;
   blk = f->cur_blk;

   TRACE2("dosfs_read(): blk = %d; f->cur_pos = %d\n", blk, f->cur_pos);

   /*
    * check if we're beyond file size
    */
   if((f->cur_pos >= f->sz) && (f->type == IT_FILE))
      return (-1);

   /*
    * read loop
    */
   while(cb > 0) {
      /*
       * calc offset in blk, block no. and bytes left
       */
      offset = f->cur_pos % blksz;
      blkno = f->cur_pos / blksz;
      left = blksz - offset;

      //printf("dosfs_read(): blkno = %d\n", blkno);

      if(blkno > f->iblk) {
         //printf("dosfs_read(): gotta change block\n");
         blk = fatread(blk);
         if(EOF(blk))
            return (-1);
         f->iblk++;
         f->cur_blk = blk;
         //printf("dosfs_read(): new block is: %d\n", blk);
      }

      bh = bread(0, blk, (blk == ROOT_BLOCK) ? BT_ROOT : BT_REGULAR);

      /*
       * calc how many bytes to copy
       * see if trying to read beyond file size
       */
      copy = (cb < left) ? cb : left;  
      if((f->type == IT_FILE) && (f->cur_pos + copy > f->sz))  
         copy = f->sz - f->cur_pos;

      TRACE1("copy = %d\n", copy);

      /*
       * calc src pointer and copy bytes
       */
      //psrc = pblk + offset;
      psrc = bh->pbuf + offset;
      bcopy(psrc, p, copy);
      p += copy;

      offset = 0;
      read += copy;
      cb -= copy;

      /*
       * if we reached the end of the block
       * advance to next one
       */
      /*
      if(copy >= left) {
         printf("dosfs_read(): copy>=left, needa change blk...\n");
      }
      */          

      brelse(bh);

      /*
       * if curpos beyond file size - outta here
       * but if file size is zero (directory possibly)
       * skip it...
       */
      f->cur_pos += copy;
      if((f->cur_pos >= f->sz) && (f->type == IT_FILE))
         break;
   }
   TRACE2("while() stopped, f->cur_blk: %d; f->cur_pos: %d\n", 
           f->cur_blk, f->cur_pos);
   TRACE1("read = %d\n", read);

   return(read);
}

int
dosfs_write(struct fcb* f, void* p, int cb)
{
   uint blksz;
   int blkno;
   uint offset, left;
   uint written = 0;
   uint blk;
   uint copy;
   uchar* pdest;
   struct buffer_head* bh;

   if(!f || !cb)
      return (-1);

   blksz = (f->cur_blk == ROOT_BLOCK) ? rootsz : dos_blksz;
   blk = f->cur_blk;

   //printf("dosfs_write(): blk = %d; f->cur_pos = %d\n", blk, f->cur_pos);

   /*
    * check if we're beyond file size
    */
   if((f->cur_pos > f->sz) && (f->type == IT_FILE))
      return (-1);

   /*
    * write loop
    */
   while(cb > 0) {
      /*
       * calc offset in blk and bytes left
       */
      offset = f->cur_pos % blksz;
      blkno = f->cur_pos / blksz;
      left = blksz - offset;
      if(blkno > f->iblk) {
         struct buffer_head* b;

         //printf("dosfs_write(): block change...\n"); 

         blk = dosfs_grow(blk);
         if(!blk)
            return (-1);

         b = getblk(0, blk, BT_REGULAR);
         //memset(b->pbuf, '*', b->sz);
         buf_valid(b);
         brelse(b);

         f->iblk++;
         f->cur_blk = blk;

	 /*
	  * special case: file has no blocks yet
	  */
	 if(f->start_blk == 0) {
		f->start_blk = blk;
		if(dosfs_chstart(f) < 0) {
			printf("dosfs_write(): chstart failed\n");
			return (-1);
		}
	 }
      }

      bh = bread(0, blk, (blk == ROOT_BLOCK) ? BT_ROOT : BT_REGULAR);

      /*
       * calc how many bytes to copy
       */
      copy = (cb < left) ? cb : left;

      TRACE1("copy = %d\n", copy);

      /*
       * calc src pointer and copy bytes
       */
      pdest = bh->pbuf + offset;
      bcopy(p, pdest, copy);
      p += copy;

      if(f->type == IT_FILE)
          f->sz += copy;

      //bh->status |= BS_DIRTY;
      buf_dirty(bh);
      brelse(bh);

      /*
       * if we reached the end of the block
       * advance to next one
       */
      /*
      if(copy >= left) {
         printf("dosfs_write(): new block needed\n");
      }
      */

      offset = 0;
      written += copy;
      cb -= copy;

      f->cur_pos += copy;
   }
   TRACE2("while() stopped, f->cur_blk: %d; f->cur_pos: %d\n", 
           f->cur_blk, f->cur_pos);
   TRACE1("written = %d\n", written);

   f->cur_blk = blk;

   /*
    * if it's file, we need to update its dir entry
    * with a new size
    */
   if(f->type == IT_FILE) {
      //printf("dosfs_write(): ok, now we're gonna update direntry...\n");
      if(dosfs_chsize(f) < 0) {
         printf("oops, it failed...\n");
         return (-1);
      }
   }

   return(written);
}

/*
 * list the contents of current working dir
 */
void
dosfs_ls(struct fcb* dir)
{
   char buf[20];
   struct dir_entry dirent;

   /*
    * scan dir from the beginning
    */
   //dir->cur_blk = dir->start_blk;
   //dir->cur_pos = 0;
   dosfs_seek(dir, 0);
   
   while(dosfs_read(dir, (void*)&dirent, sizeof(struct dir_entry)) > 0) {
      char filename[20];
   
      if(dirent.name[0] == 0) {
         break;
      }

      /*
       * skip deleted crap ...
       */
      if(dirent.name[0] == F_DELETED) {
         continue;
      }

      dosfs_name(&dirent, filename);
      sprintf(buf, "%-16s", filename);
      printf(buf);

   }

   printf("\n");
}

int
dosfs_seek(struct fcb* f, int pos)
{
   uint blk;
   uint iblk;
   uint blksz, blkno;

   if(!f)
      return (-1);

   if(pos == 0) {
   	f->cur_blk = f->start_blk;
   	f->iblk = f->cur_pos = 0;

   	return (1);
   }

   blksz = (f->start_blk == ROOT_BLOCK) ? rootsz : dos_blksz;

   /*
    * check if new position is within file
    */
   if(pos > f->sz) {
      //printf("dosfs_seek(): beyond file size\n");
      //printf("f->cur_pos: %d, pos: %d, f->sz: %d\n", 
         //f->cur_pos, pos, f->sz);
      return(-1);
   }

   /*
    * calc new block and offset in it
    */
   blk = iblk = pos / blksz;

   if(blk) {
      /*
       * go thru all blocks checking if they're valid
       * XXX this slows things down, is it really necessary?
       */
      int cur_blk = f->start_blk;
      while(blk--) {
         cur_blk = fatread(cur_blk);
         if(cur_blk >= FAT_EOF)
            return(-1);
      }
      f->cur_blk = cur_blk;
   }

   //printf("dosfs_seek(): f->iblk = %d, f->cur_blk: %d\n", f->iblk, f->cur_blk);

   f->iblk = iblk;
   f->cur_pos = pos;
   
   return(1);
}

void
dosfs_setroot(struct fcb* dir)
{
	dir->start_blk = dir->cur_blk = ROOT_BLOCK;
	dir->iblk = 0;
}

static char tmp_name[MAX_PATH];

/*
 * set given directory
 */
int
dosfs_setpwd(struct fcb* dir, char* name)
{
   char buf[20];
   uint blk;
   struct inode i;
   char* pname = name;
   char* pbuf = buf;
   struct fcb tmp;

   if(!dir)
   	return (-1);

   bcopy(dir, &tmp, sizeof(struct fcb));
   strcpy(tmp_name, "");

   if(*name == '/') {
      buf[0] = '/';
      buf[1] = 0;

      TRACE1("dosfs_setpwd(): buf = %s\n", buf);

      dosfs_setroot(&tmp);
      strcpy(tmp_name, "/");
      pname++;
   }

   if(*pname) {
      while(1) {
         while((*pname != '/') && (*pname != 0)) {
            *pbuf++ = *pname++;
         }
         *pbuf = 0;

         TRACE1("dosfs_setpwd():buf = %s\n", buf);

         blk = dosfs_lookup(&tmp, buf, &i);
         if(blk < 0) {
            TRACE("not found.\n");
            return (-1);
         }
         
         /*
          * check if it's dir
          */
         if(!(i.type & IT_DIR)) {
            TRACE("not a directory.\n");
            return (-1);
         }

         tmp.start_blk = tmp.cur_blk = blk;
	 tmp.iblk = 0;
         strcat(tmp_name, buf);
         if( *pname == 0 )
            break;
         pbuf = buf;
         pname++;
      }
   }

   bcopy(&tmp, dir, sizeof(struct fcb));

   return(1);
}

static uint last_fat = 2;

static uint
dosfs_balloc()
{
   uint c, i;

   i = last_fat;
   c = fatsz - i;
   while(c--) {
      if(fatread(i) == 0)
         break;
      i++;
   }
   if(c == 0) {
      printf("dosfs_balloc(): no free blocks\n");
      return (0);
   }
   last_fat = i + 1;

   //printf("dosfs_ffat(): found empty FAT slot at: %d\n", i);
   fatwrite(i, FAT_EOF);

   return (i);
}

static uint
dosfs_grow(uint last_blk)
{
   uint i = dosfs_balloc();
   if(!i)
      return (0);

   if(last_blk != 0)
   	fatwrite(last_blk, i);
   return(i);
}

static uint
dosfs_write_node(struct fcb* dir, char* name, struct inode* i)
{
   struct dir_entry dirent;
   uint nblk;
   uint last_blk;
   uint dirsz;
   char nombre[20];
   char* dot = 0;
   int recycle = 0;

   /*
    * scan dir from the beginning
    */
   dosfs_seek(dir, 0);

   //last_blk = dir->cur_blk;
   //nblk = 0;

   /*
    * dir is of at least this size
    * watch out for root dir here
    */
   dirsz = (dir->start_blk == ROOT_BLOCK) ? rootsz : dos_blksz;

   //printf("dir->cur_blk = %d\n", dir->cur_blk);
   
   while(1) {
      int r;
      
      r = dosfs_read(dir, (void*)&dirent, sizeof(struct dir_entry));
      if(r <= 0) {
         /*
          * check here if read ended beacause of end of block
          * if so, extend dir to the next block
          */
	 /*
         if(dir->cur_pos == dirsz) {
            printf("we reached the end of dir, needa extend it...\n");
         }
	 */

         break;
      }

      /*
       * if dir block changes, record count and dir size
       */
      /*
      if(dir->cur_blk != last_blk) {
         printf("dosfs_write_node(): dir block changed.\n");
         nblk++;
         dirsz += dos_blksz;
         last_blk = dir->cur_blk;
      }
      */

      if(dirent.name[0] == 0 || dirent.name[0] == F_DELETED) {
   	recycle = 1;
   	break;
      }
    }


    /*
     * prepare filename
     * ugh, I hate it
     */
    memset(&dirent.name, ' ', 8 + 3);
    strcpy(nombre, name);
    strupr(nombre);
    dot = strchr(nombre, '.');
    if(dot) {
      strncpy(dirent.name, nombre, dot++ - nombre);
      strncpy(dirent.ext, dot, strlen(dot));
    } else
      strncpy(dirent.name, nombre, strlen(nombre));

    dirent.start = i->blk;
    dirent.size = 0;

    if(i->type == IT_DIR)
            dirent.attr = FA_DIREC;
    else
            dirent.attr = FA_ARCH;

    if(recycle) {
      //printf("dosfs_write_node(): found free dirent, let's fill it...\n");
      /*
       * rewind a little
       */
      dir->cur_pos -= sizeof(struct dir_entry);
      //dosfs_write(dir, (void*)&dirent, sizeof(struct dir_entry));

      //return (1);
   }
#ifdef _DEBUG   
   else
   	printf("dosfs_write_node(): no free dirents found...\n");
#endif

   if(dosfs_write(dir, (void*)&dirent, sizeof(struct dir_entry)) <= 0) {
      printf("dosfs_write_node(): write failed\n");
      return (0);
   }

   /*
    * rewind a little
    * dir always has to point to file entry
    */
   dir->cur_pos -= sizeof(struct dir_entry);

   return (1);
}

int
dosfs_mknode(struct fcb* dir, char* name, uint type, struct inode* res)
{
   struct inode i;

   if(dosfs_lookup(dir, name, &i) >= 0) {
      printf("dosfs_mknode(): file exists.\n");
      return (-1);
   }

   bzero(&i, sizeof(struct inode));
   i.type = type;
   i.blk = 0;
   //i.blk = dosfs_balloc();
   //printf("dosfs_mknode(): newblk = %d\n", i.blk);   

   dosfs_write_node(dir, name, &i); 

   if(type == IT_DIR) {
      /*
       * write dir entries '.' and '..'
       */
      struct buffer_head* bh = bread(1, i.blk, BT_REGULAR);
      bzero(bh->pbuf, bh->sz);
   }

   bcopy(&i, res, sizeof(struct inode));
   return (1);
}

int
dosfs_stat(struct fcb* f)
{
   char name[20];
   struct dir_entry dirent;
   int r;

   if(!f)
   	return (-1);

   r = dosfs_read(f->dir, (void*)&dirent, sizeof(struct dir_entry));
   if(r < 0) {
      printf("dosfs_stat(): read failed.\n");
      return (-1); 
   }

   dosfs_name(&dirent, name); 
   printf("name: %s\n", name);
   printf("time: %d, date: %d, start: %d, size: %d\n",
           dirent.time, dirent.date, dirent.start, dirent.size);

   /*
    * rewind
    */
   f->dir->cur_pos -= sizeof(struct dir_entry);

   return (1);
}

int
dosfs_chmod(struct fcb* f)
{
   return (-1);
}

#define _RETCHECK 1

static int
dosfs_inode(uint op, struct fcb* dir, struct dir_entry* dirent)
{
   int r = (op == INO_READ) ? 
      dosfs_read(dir, dirent, sizeof(struct dir_entry)) :
      dosfs_write(dir, dirent, sizeof(struct dir_entry));

   /*
    * rewind
    */
   dir->cur_pos -= sizeof(struct dir_entry);

#ifdef _RETCHECK
   /*
    * check status
    */
   if(r < 0) {
      if(op == INO_READ)
         printf("dosfs_inode(): read failed.\n");
      else
         printf("dosfs_inode(): write failed.\n");

      return (-1); 
   }
   return (1);
#else
   return (r);
#endif /* _RETCHECK */
}

static int
dosfs_chsize(struct fcb* f)
{
   struct dir_entry dirent;

   if(dosfs_inode(INO_READ, f->dir, &dirent) < 0)
      return (-1);

   if(dirent.size == f->sz)
      return (1);

   dirent.size = f->sz;
   return (dosfs_inode(INO_WRITE, f->dir, &dirent));
}

static int
dosfs_chstart(struct fcb* f)
{
   struct dir_entry dirent;

   if(dosfs_inode(INO_READ, f->dir, &dirent) < 0)
      return (-1);

   if(dirent.start == f->start_blk)
      return (1);

   dirent.start = f->start_blk;
   return (dosfs_inode(INO_WRITE, f->dir, &dirent));
}

int
dosfs_truncate(struct fcb* f)
{
	uint blk, last_blk;
	last_blk = f->start_blk;

	while(1) {
		blk = fatread(last_blk);
		printf("dosfs_truncate(): freeing %d\n", last_blk);				
		fatwrite(last_blk, FAT_FREE);
		if(EOF(blk))
			break;

		last_blk = blk;
	}
	f->iblk = -1;
	f->cur_pos = f->sz = 0;
	f->cur_blk = f->start_blk = 0;

      	if(dosfs_chsize(f) < 0) {
        	printf("dosfs_truncate(): oops dosfs_chsize() failed...\n");
         	return (-1);
      	}
      	if(dosfs_chstart(f) < 0) {
        	printf("dosfs_truncate(): oops dosfs_chstart() failed...\n");
         	return (-1);
      	}

	return (1);
}
