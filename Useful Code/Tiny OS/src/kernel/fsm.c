/*
 * filesystem manager
 *
 * $Id: FSM.C 1.4 1996/12/15 08:34:52 bart Exp $
 *
 */

#include <sys/wd.h>
#include <sys/proc.h>
#include <sys/dosfs.h>
#include <sys/fs.h>
#include <assert.h>
#include <fcntl.h>

//#define _DEBUG
#include <trace.h>
#undef _DEBUG

#define PART_OFF  (0x1BE)

/*
 * partition types
 */
#define PT_DOS12	(0x01)
#define PT_DOS16	(0x04)
#define PT_EXTDOS 	(0x05)
#define PT_BIGDOS 	(0x06)

/*
 * fs types
 */
#define FS_DOS    (0x02)
#define FS_TINOS  (0x04)

/*
 * all 4 partitions on disk
 */
#define NPART  (0x04)
static struct part_desc partitions[NPART];

static uchar sect_buf[SECSZ];

/*
 * filesystems
 */
#define MAX_FS (0x04)
static struct fsys_desc fsys[MAX_FS];

/*
 * file control blocks pool
 */
static struct fcb fcb_pool[MAX_FILES];

/*
 * kernel working dir
 */
struct fcb kernwd;

/*
 * allocate new file entry
 * put handle to 'handle'
 */
static struct fcb*
alloc_fcb(int* handle)
{
   struct fcb* f = &fcb_pool[4];
   int c, h;

   c = MAX_FILES;
   h = 4;
   while(c--) {
      if(f->fsys == 0) {
	 f->fsys = 1;
         TRACE1("alloc_fcb(): handle = %d\n", h);
         *handle = h;
    	 f->start_blk = 0; /* dangerous, 1 is root */
         return(f);
      }
      f++;
      h++;
   }

   *handle = -1;
   return(NULL);
}

/*
 * main initialization routine
 */
int
fsm_init()
{
   int c;
   struct part_desc* part;
   uchar* p;
   uint init = 0;

   /*
    * read first (0) sector and register all partitions
    */
   reads((void*)sect_buf, 0, 1);

   p = sect_buf + PART_OFF;
   for(c = 0; c < NPART; c++) {
      bcopy(p, &partitions[c], sizeof(struct part_desc));
      p += sizeof(struct part_desc);
   }

#ifdef _DEBUG
   /*
    * dump partition info
    */
   part = &partitions[0];
   for(c = 0; c < NPART; c++) {
      printf("partition[ %d ]: type: %d\n", c, part->type);
      TRACE5("boot: %d; head: %d; sec: %d; cyl: %d; type: %d\n", 
              part->boot, part->head, part->sec, part->cyl, part->type);
      TRACE5("ehead: %d; esec: %d; ecyl: %d; start: %d; len: %d\n", 
              part->ehead, part->esec, part->ecyl, part->start, part->len);

	part++;
   }
#endif

   return (1);
}

int
fsm_mount(int partno)
{
   struct part_desc* part = &partitions[partno];
   struct fsys_desc* fs = &fsys[0];

   fs->type = 0;
   fs->disk = 0;  /* for now */
   fs->start = (partno == 0) ? part->start : part->start + partitions[0].start;
   fs->len = part->len;
   
   /*
    * for now fill with nulls
    */
   fs->read = NULL;
   fs->write = NULL;

   switch(part->type) {
	case PT_DOS16:
	case PT_BIGDOS:
	case PT_EXTDOS:
		fs->type = FS_DOS;
		printf("mounting DOS partition...\n");
		dosfs_init(fs);	
		break;

	default:
		printf("fsm_mount(): unknown partition type.\n");
		return (-1);
   }
   TRACE1("fs->blksz: %d\n", fs->blksz);

   /*
    * setup kernel working dir
    * it's boot fs (dosfs) root
    */
   bcopy(&fs->root, &kernwd, sizeof(struct fcb));
   return (1);
}

static char*
setup_path(struct pcb* proc, char* path, struct fcb* dir)
{
	char buf[256];
	char* p, *last = NULL, *file = NULL, *ret;
	void* src;

   	if(proc) {
   		struct proc_env* penv = (struct proc_env*)VA_PENV;
		src = (void*)&penv->pwd;
   	} 
   	else
		src = (void*)&kernwd;
	bcopy(src, dir, sizeof(struct fcb)); 

	strcpy(buf, path);
	p = buf;
	while(*p) {
		if(*p == '/')
			last = p;
		p++;
	}
	if(last) {
		//printf("last slash: %s\n", last);
		file = last + 1;
		*last = 0;
	}
	else {
		//printf("no slashes\n");
		file = buf;
	}
	
	//printf("dir: %s, file: %s\n", buf, file);
	ret = path + (file - buf);
	//printf("ret = %s\n", ret);

	if(last) {
		//printf("ok, now chdir() to %s\n", buf);
      		if(dosfs_setpwd(dir, buf) < 0) {
			//printf("dosfs_setpwd() failed\n");
			return (0);
      		}
		//printf("ok, dir changed\n");
        }
		
	return (ret);
}

int
fsm_open(struct pcb* p, char* path, uint flags, uint mode)
{
   uint fblk;
   struct fcb open_dir;
   struct fcb* dir;
   struct inode res;
   struct fcb* f;
   struct fcb* fdir;
   int h, temp;
   char* file;

   file = setup_path(p, path, &open_dir);
   dir = &open_dir;
   if(!file) {
	printf("fsm_open(): setup_path() failed\n");
	return (-1);
   }

   TRACE2("fsm_open(): dir->start_blk = %d, p = %d\n", dir->start_blk, (ulong)p);

   if(flags & O_CREAT) {
	if(dosfs_mknode(dir, file, IT_FILE, &res) == -1) {
     		printf("fsm_open(): could not create file.\n");
      		return (-1);
	}
   	//printf("fsm_open(): file created\n");
   } 
   else {
   	/*
         * lookup file in dir
         */
   	fblk = dosfs_lookup(dir, file, &res);
   	if(fblk == -1) {
     		printf("fsm_open(): file not found.\n");
      		return (-1);
        }
   	//printf("fsm_open(): file found: blk = %d\n", fblk);
   }


   /*
    * alloc fcb
    */
   f = alloc_fcb(&h);
   if(f == NULL)
      return (-1);

   /*
    * alloc fcb for directory
    */
   fdir = alloc_fcb(&temp);
   if(fdir == NULL)
      return (-1);

   /*
    * setup per process open file table
    */
   if(p) {
   	struct proc_env* penv = (struct proc_env*)VA_PENV;
   	int* pof = &penv->oftab[0];
   	int c = MAX_FILES;
   	while(c--) {
      		if(*pof == 0)
         		break;
      		pof++;
   	}
   	*pof = h;
   }

   //bcopy(&f, newf, sizeof(struct fcb));

   bzero(f, sizeof(struct fcb));
   f->fsys = 1;
   f->start_blk = f->cur_blk = res.blk;
   f->iblk = (res.blk == 0) ? -1 : 0;
   f->sz = res.sz;
   f->type = res.type;

   //printf("fsm_open(): f->start_blk: %d, f->cur_blk: %d\n", 
	//   f->start_blk, f->cur_blk);

   /*
    * make a copy of dir and link file to it
    */
   bcopy(dir, fdir, sizeof(struct fcb));
   fdir->fsys = 1;
   f->dir = fdir;

   return (h);
}

static struct fcb*
getfcb(int h)
{
   struct fcb* f = &fcb_pool[h];

   if((h <= 0) || (h >= MAX_FILES))
      return (NULL);

   return (f);
}

int
fsm_read(int h, void* p, int cb)
{
   //printf("fsm_read()\n");
   return(dosfs_read(getfcb(h), p, cb));
}

int
fsm_write(int h, void* p, int cb)
{
   //printf("fsm_write()\n");
   return(dosfs_write(getfcb(h), p, cb));
}

int
fsm_seek(int h, int pos)
{
   //printf("fsm_seek(): f->start_blk = %d\n", f->start_blk);
   return(dosfs_seek(getfcb(h), pos));
}

int
fsm_stat(int h)
{
   return(dosfs_stat(getfcb(h)));
}

int
fsm_chmod(int h)
{
   return(dosfs_chmod(getfcb(h)));
}

int
fsm_setpwd(struct pcb* p, char* name)
{
   struct fcb tmp;
   struct fcb* dir;

   if(p) {
      int r;
      struct proc_env* penv = (struct proc_env*)VA_PENV;

      bcopy(&penv->pwd, &tmp, sizeof(struct fcb));
      dir = &tmp;

      r = dosfs_setpwd(dir, name);
      if(r > 0) {
         bcopy(dir, &penv->pwd, sizeof(struct fcb));
      }

      return (r);

   } else {
      dir = &kernwd;
   }

   //printf("fsm_setpwd(): dir->start_blk = %d\n", dir->start_blk);

   return(dosfs_setpwd(dir, name));
}

void
fsm_close(int h)
{
   struct fcb* f = &fcb_pool[h];

   if(f->dir)
       bzero(f->dir, sizeof(struct fcb));
   bzero(f, sizeof(struct fcb));
}

void
fsm_ls()
{
   dosfs_ls(&kernwd);
}

int
new_open(char* file)
{
   return(fsm_open(NULL, file, 0, 0));
}

int
new_read(int h, void* p, int cb)
{
   return(fsm_read(h, p, cb));
}

int
new_seek(int h, int pos)
{
   return(fsm_seek(h, pos));
}

void
new_close(int h)
{
   fsm_close(h);
}

int
chdir(char* name)
{
   return(fsm_setpwd(NULL, name));
}

void
setkwd(struct pcb* p)
{
      struct proc_env* penv = (struct proc_env*)VA_PENV;
      kernwd.start_blk = penv->pwd.start_blk;
}

int
fsm_creat(struct pcb* p, char* path)
{
   struct inode i;
   struct fcb creat_dir;
   struct fcb* dir;
   char* file;

   dir = &creat_dir;
   file = setup_path(p, path, dir);
   if(!file) {
	printf("fsm_creat(): setup_path() failed\n");
	return (-1);
   }

   return (dosfs_mknode(dir, file, IT_FILE, &i));
}

int
fsm_trunc(struct pcb* p, char* path)
{
   uint fblk;
   struct inode res;
   struct fcb trunc_dir;
   struct fcb* dir;
   struct fcb f;
   char* file;

   dir = &trunc_dir;
   file = setup_path(p, path, dir);
   if(!file) {
	printf("fsm_trunc(): setup_path() failed\n");
	return (-1);
   }

   fblk = dosfs_lookup(dir, file, &res);
   if(fblk == -1) {
   	printf("fsm_trunc(): file not found.\n");
      	return (-1);
   }

   bzero(&f, sizeof(struct fcb));
   f.fsys = 1;
   f.start_blk = f.cur_blk = res.blk;
   f.sz = res.sz;
   f.type = res.type;
   f.dir = dir;

   dosfs_truncate(&f);

   return (1);
}

int
fsm_mkdir(struct pcb* p, char* path)
{
   struct inode i;
   struct fcb mkdir_dir;
   struct fcb* dir;
   char* file;

   /*
    * XXX would be cool to be able to create the whole branch
    * XXX recursive?
    */
   dir = &mkdir_dir;
   file = setup_path(p, path, dir);
   if(!file) {
	printf("fsm_mkdir(): setup_path() failed\n");
	return (-1);
   }
   return (dosfs_mknode(dir, file, IT_DIR, &i));
}

