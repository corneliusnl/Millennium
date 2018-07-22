/*
 * system console
 *
 * $Id: SYSCON.C 1.3 1996/12/04 16:10:21 bart Exp $
 *
 */

#include <types.h>
#include <i386/inlines.h>
#include <sys/tss.h>
#include <bcopy.h>
#include <sys/wd.h>
#include <sys/buf.h>
#include <ctype.h>
#include <string.h>

#include <trace.h>
#undef _DEBUG

extern ulong pd[];
extern ulong pt[];

#define ARG1(x) (!strcmp(argv[1], x))

#define  BACKSPACE   8

#define VERSION      5

#define MAX_ARGS  10
#define MAX_ARGSZ 50

int   argc = 0;
char  argv[MAX_ARGS][MAX_ARGSZ];

struct cmd_tbl {
   char name[20];
   voidfun func;
};

void
test()
{
   printf("test()\n");
}

/*
 * protos
 */
static void do_reboot();
static void dosfs();
static void ls();
static void chdir();
static void open_file();
static void read_file();
static void write_file();
static void seek_file();
static void stat_file();
static void chmod_file();
static void trunc_file();
static void exec();
static void creat();
static void mkdir();
static void mount();
static void do_getblk();
static void do_brelse();
static void testfat();
static void copy_file();
static void readall();
static void edlin();

extern void fsm_init();
extern void buf_dump();

/*
 * command table
 */
struct cmd_tbl cmd[] = { 
     { "reboot", do_reboot },
     { "dosfs", dosfs },
     { "ls", ls },
     { "cd", chdir },
     { "fsm_init", fsm_init },
     { "mount", mount },
     { "open", open_file },
     { "read", read_file },
     { "write", write_file },
     { "seek", seek_file },
     { "stat", stat_file },
     { "bufdump", buf_dump },
     { "exec", exec },
     { "creat", creat },
     { "mkdir", mkdir },
     { "chmod", chmod_file },
     { "trunc", trunc_file },
     { "getblk", do_getblk },
     { "brelse", do_brelse },
     { "testfat", testfat },
     { "copy", copy_file },
     { "readall", readall },
     { "edlin", edlin },
     { "test", test } };

#define NCMD (sizeof(cmd)/sizeof(struct cmd_tbl))

static void
mount()
{
   fsm_mount(atoi(argv[1]));
}

static void 
dosfs()
{
/* 
   if(ARG1("-init"))
      dosfs_init();
   else
   if(ARG1("-open"))
      dosfs_open(NULL, argv[2]);
   else
   if(ARG1("-dir"))
      _setpwd(NULL, argv[2]);
   else
      printf("dosfs: unknown switch\n");
*/
}

static void 
ls()
{
   fsm_ls();
}

static void 
chdir()
{
   int r = fsm_setpwd(NULL, argv[1]);
   printf("fsm_setpwd(): %d\n", r);
}

void
reboot()
{
   int c;

   syncbufs();
   printf("any key to reboot...\n");
   while(!kb_getch());

   cli();
   for(c = 0; c < 0xFFFF; c++)
      if(inb(0x64) == 0x02)
         break;
   outb(0x64, 0xFE);
   sti();
   while(1);
}

static void 
do_reboot()
{
   /*
   syncbufs();
   printf("any key to reboot...\n");
   while(!kb_getch());
   */
   reboot();
}

static void
open_file()
{
   int h = fsm_open(NULL, argv[1], 0);
   printf("open(): handle = %d\n", h);
}

static void
read_file_old()
{
   char buf[21];
   int r = fsm_read(atoi(argv[1]), buf, 20);
   printf("read(): r = %d\n", r);
   printf("buf = %s\n", buf);
}

static uchar* ptr = (uchar*)0x180000;

static void
read_file()
{
   int r = fsm_read(atoi(argv[1]), ptr, atoi(argv[2]));
   printf("read(): r = %d\n", r);
   ptr[80] = 0;
   printf("DATA: %s\n", ptr);
}

static void
write_file()
{
   int c, r;
   int cb = atoi(argv[2]);
   
   for(c = 0; c < cb; c++)
      ptr[c] = 'A';

   r = fsm_write(atoi(argv[1]), ptr, cb);
   printf("write(): r = %d\n", r);
}

static void
seek_file()
{
   int r = fsm_seek(atoi(argv[1]), atoi(argv[2]));
   printf("seek(): r = %d\n", r);
}

static void
stat_file()
{
   int r = fsm_stat(atoi(argv[1]));
   printf("stat(): r = %d\n", r);
}

static void
chmod_file()
{
   int r = fsm_chmod(atoi(argv[1]));
   printf("chmod(): r = %d\n", r);
}

static void
trunc_file()
{
   fsm_trunc(NULL, argv[1]);
}

static void
exec()
{
   int pid = _create_proc(NULL, argv[1]);
   printf("pid: %d\n", pid);

   if(pid > 0) {
      enable_irq(0);
      _exec_proc(pid, "/tinos/bin");
   }
}

static void
creat()
{
   fsm_creat(NULL, argv[1]);
}

static void
mkdir()
{
   fsm_mkdir(NULL, argv[1]);
}

static void
do_getblk()
{
   struct buffer_head* b;

   b = getblk(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
   printf("b = %d\n", (ulong)b);
}

static void
do_brelse()
{
   brelse((struct buffer_head*)atoi(argv[1]));
}

static void
testfat()
{
   dosfs_testfat(atoi(argv[1]));
}


static char buffer[1025];

static void
copy_file()
{
   int in, out;
   char* src = argv[1];
   char* dst = argv[2];
   uint n = atoi(argv[3]);

   printf("copy %d bytes from %s to %s, is that right?(y/n)\n",
      n, src, dst);
   while(1) {
      char ch = kb_getch();
      if(ch == 'n')
         return;
      if(ch == 'y')
         break;         
   }

   //printf("opening source.\n");
   //while(!kb_getch());

   bufzero();

   in = fsm_open(NULL, src, 0);
   if(in == -1) {
      printf("open src failed.\n");
      return;
   }

   //printf("creating dest.\n");
   //while(!kb_getch());

   fsm_creat(NULL, dst);

   //printf("opening dest.\n");
   //while(!kb_getch());

   out = fsm_open(NULL, dst, 0);
   if(out == -1) {
      printf("open dest failed.\n");
      return;
   }

   //printf("now copying (1024 bytes chunks) ->\n");
   //while(!kb_getch());

   while(1) {
      int rb = fsm_read(in, ptr, 64);

      if(rb <= 0) {
         printf("done.\n");
         break;
      }

      if(fsm_write(out, ptr, rb) < 0) {
         printf("write failed\n");
         break;
      }

      //printf(".");
   }
   
   fsm_close(in);
   fsm_close(out);

   bufstat();
}

static void
readall()
{
   int h;
   uint cb = atoi(argv[2]);

   bufzero();
   h = fsm_open(NULL, argv[1], 0);
   if(h == -1) {
      printf("open src failed.\n");
      return;
   }
   while(fsm_read(h, ptr, cb) > 0);
   fsm_close(h);
   bufstat();
}

#define MAX_LINE  100

static void
edlin()
{
   char line[MAX_LINE];
   int h, n;
   char* p;

   //fsm_creat(NULL, argv[1]);   
   h = fsm_open(NULL, argv[1], 1);
   if(h == -1) {
      printf("open failed.\n");
      return;
   }

   p = line;
   n = 0;
   while(1) {
      char ch = kb_getch();
      if(!ch)
         continue;
      if(ch == '.') {
         break;
      }
      if(ch == 0x0D) {
         putc('\n');
         *p = 0x0D;
         n++;
         if(fsm_write(h, line, n) < 0) {
            printf("write failed\n");
            break;
         }
         p = line;
         n = 0;
         continue;
      }
            if(ch == BACKSPACE) {
                  if( n ) {
                        ungetc();
                        n--;
                  }
                  continue;
            }
      *p++ = ch;
      putc(ch);
      if(++n >= MAX_LINE) {
         printf("buffer overflow\n");
         break;
      }
   }
   fsm_close(h);
}

void 
parse(char* p)
{
   char buf[100];
   char* d = buf;

   argc = 0;
   while(1) {
      while(*p++ == ' ');
      p--;
      while((*p != ' ') && (*p != 0))  
         *d++ = *p++;
      *d = 0;
      TRACE1("str: %s\n", buf);
      TRACE1("int: %d\n", atoi(buf));
      strcpy(argv[argc], buf);
      argc++;
      if( argc > MAX_ARGS ) {
         printf("parse: fatal - argc > MAX_ARGS\n");
         argc--;
         return;
      }
      if(*p == 0)
         return;
      p++;
      d = buf;
   }
}

void 
execute()
{
   int c = NCMD;
   int i = 0;

   while(c--) {
      if( !strcmp(cmd[i].name, argv[0]) ) {
         cmd[i].func();
         break;
      }
      i++;
   }

   if(i == NCMD)
      printf("unknown command.\n");
}

void
testfs()
{
/*
   int h, r;
   char buf[21];

   dosfs_init();
   h = _open(NULL, "autoexec.bat");
   printf("_open(): %d\n", h);
   if(h == -1)
      return;

   r = _read(NULL, h, buf, 20);
   printf("_read(): %d\n", r);

   buf[20] = 0;
   printf("%s\n", buf);
*/
}

static void 
dump_args()
{
   int c;
   for(c = 0; c < argc; c++)
      printf("argv[%d] = %s\n", c, argv[c]);
}

void 
syscon()
{
   char  ch;
   char  buf[256];
   int   c;

   print("\nEntering kernel console.\n#");
   c = 0;
   while(1) {
      ch = kb_getch();
      if( ch ) {
         if( ch == 0x0D ) {
            putc('\n');
            if( c == 0 ) {
               putc('#'); 
               continue;
            }
            buf[ c ] = 0;
            parse(buf);
            execute();
            putc('#');
            c = 0;
            continue;
         }
         if( ch == BACKSPACE ) {
            if( c ) {
               ungetc();
               c--;
            }
            continue;
         }
         putc(ch);   
         buf[ c ] = ch;
         c++;
      }
   }
}

