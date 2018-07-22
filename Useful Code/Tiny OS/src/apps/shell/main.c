#include <types.h>
#include <stdio.h>
#include <sys/kernel.h>

//#define _DEBUG
#include <trace.h>

#define  BACKSPACE   8

#define MAX_ARGS  10
#define MAX_ARGSZ 50

int   argc = 0;
char  args[MAX_ARGS][MAX_ARGSZ];
char* argv[MAX_ARGS];

struct cmd_tbl {
   char name[20];
   voidfun func;
};

/*
 * functions
 */
extern void reboot();
extern void ls();
extern void cd();
extern void ps();
extern void finito();
extern void kill();
extern void sleep_proc();
extern void wake_proc();
extern void prio();
static void dump_args();
static void helpme();

/*
 * command table
 */
struct cmd_tbl cmd[] = { 
     { "reboot", reboot },
     { "ps", ps },
     { "cd", cd },
     { "dump", dump_args },
     { "quit", finito },
     { "exit", finito },
     { "kill", kill },
     { "sleep", sleep_proc },
     { "wake", wake_proc },
     { "prio", prio },
     { "help", helpme },
     { "ls", ls } };

#define NCMD (sizeof(cmd)/sizeof(struct cmd_tbl))

void 
parse(char* p)
{
   char buf[100];
   char* d = buf;

   argc = 0;
   bzero((void*)args, sizeof(args));
   while(1) {
      while(*p++ == ' ');
      p--;
      while((*p != ' ') && (*p != 0))  
         *d++ = *p++;
      *d = 0;
      TRACE1("str: %s\n", buf);
      TRACE1("int: %d\n", atoi(buf));
      strcpy(args[argc], buf);
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

   if(i == NCMD) {
      uint pid = exec_proc(argv[0], argc, argv);
      if(pid == 0) {
      	printf("unknown command.\n");
        return;
      }
      if(strcmp(argv[argc-1], "&"))
	waitpid(pid);
   }
}

static void 
dump_args()
{
   int c;
   for(c = 0; c < argc; c++)
      printf("argv[%d] = %s\n", c, argv[c]);
}

static void
helpme()
{
   int c;
   printf("available commands:\n");
   for(c = 0; c < NCMD; c++)
      printf("%s ", cmd[c].name);
   printf("\n");
}

static char prompt = '$';

void 
main(int argc, char** _argv)
{
   char  ch;
   char  buf[256];
   int   c;

   for(c = 0; c < MAX_ARGS; c++)
	argv[c] = args[c];

   os_kernstat(KS_ASYNC);

   printf("\nShell, version 1.1\n%c", prompt);
   c = 0;
   while(1) {
      ch = getch();
      if( ch ) {
         if( ch == 0x0D ) {
            putc('\n');
            if( c == 0 ) {
               putc(prompt); 
               continue;
            }
            buf[ c ] = 0;
            parse(buf);
            execute();
            putc(prompt);
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
