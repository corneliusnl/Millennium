#include <types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/kernel.h>
#include <sys/proc.h>

#define LS_LONG (0x02)

#define LS_BUFSZ 	(2048)
static char names[LS_BUFSZ][20];

extern int argc;
extern char* argv[];

/*
 * construct a dosfs filename
 */
static void
dosname(char* name, char* _ext, char* buf)
{
   char* p;
   char ext[4];

   strncpy(buf, name, 8);
   strncpy(ext, _ext, 3);

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

static void
do_ls(ulong swtch)
{
   int b = 0, c, l;
   int count = 0;
   uchar buf[32];
   char name[32];
   char ext[4];

   int h = open(".", O_RDONLY);
   if(h == -1) {
      printf("sh: open '.' failed");
      return;
   }

   while(read(h, buf, 32) > 0) {
      if(*buf == 0)
         break;
      if(*buf == 0xE5)
         continue;
      b++;
      bcopy(buf, name, 8);
      name[8] = 0;
      bcopy(buf+8, ext, 3);
      ext[3] = 0;
      dosname(name, ext, names[count]);
      strlwr(names[count]);
      if(++count >= LS_BUFSZ) {
         printf("sh: WHOA! ls buffer overflow\n");
         return;
      }
   }
   close(h);

   if(swtch & LS_LONG) {
      for(c = 0; c < count; c++) {  
         printf("%-16s\n", names[c]);
      }
   } else {
      l = 0;
      for(c = 0; c < count; c++) {  
         printf("%-16s", names[c]);
      }
   }

   printf("\n%d entries processed.\n", count);
}

void
ls()
{
	uint flags = 0;
	if( !strcmp(argv[1], "-l"))
		flags |= LS_LONG;
	do_ls(flags);
}

void
cd()
{
	if(chdir(argv[1]) == -1)
		printf("could not change directory to %s\n", argv[1]);
}

void
reboot()
{ os_kernstat(KS_REBOOT); }

void
ps()
{ os_kernstat(KS_PROC); }

void
finito()
{ exit(0); }

void
kill()
{
	uint pid;

	if(argc < 2) {
		printf("specify pid.\n");
		return;
	}

	pid = atoi(argv[1]);
	if(os_pctl(PC_KILL, pid, 0) == -1)
		printf("command failed.\n");
}

void
sleep_proc()
{
	uint pid;
	if(argc < 2) {
		struct proc_env* penv = (struct proc_env*)VA_PENV;
		printf("pid = %d\n", penv->pid);
		
		if(os_pctl(PC_SLEEP, pid, 0) == -1)
			printf("command failed.\n");
		return;
	}

	pid = atoi(argv[1]);
	if(os_pctl(PC_SLEEP, pid, 0) == -1)
		printf("command failed.\n");
}

void
wake_proc()
{
	uint pid;

	if(argc < 2) {
		printf("specify pid and priority.\n");
		return;
	}
	pid = atoi(argv[1]);
	if(os_pctl(PC_WAKEUP, pid, 0) == -1)
		printf("command failed.\n");
}

void
prio()
{
	uint pid;

	if(argc < 3) {
		printf("specify pid and priority.\n");
		return;
	}

	pid = atoi(argv[1]);
	if(os_pctl(PC_SETPRIO, pid, atoi(argv[2])) == -1)
		printf("command failed.\n");
}
