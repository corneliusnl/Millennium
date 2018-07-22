#include <types.h>
#include <stdarg.h>
#include <sys/arg.h>
#include <sys/calls.h>

#include <trace.h>

static void
pack_args(int argc, const char** argv, char* args)
{
   ulong c;
   char* p = args;

   
   *(ulong*)p = argc;
   p += sizeof(ulong);
   
   for(c = 0; c < argc; c++) {
      uint len = strlen(argv[c])+1;
      bcopy(argv[c], p, len);
      p += len;
   }
}

/*
 * exec() takes packed args
 */
int
exec(const char* filename, char* args)
{
   if(filename == NULL)
      return (-1);

   return(os_exec(filename, args));
}

int
execl(const char* filename, const char* argv0, ...)
{
   const char* argv[MAX_ARGNO];
   char args[MAX_ARGSZ];
   const char* p;
   uint c, i;
   va_list arglist;
   va_start(arglist, filename);
   
   /*
    * first arg is process name
    */
   argv[0] = filename;

   /*
    * put all arg pointers into argv[]
    */
   c = 1;
   while(1) {
      if(c >= sizeof(argv) / sizeof(argv[0])) {
         printf("execl(): arglist too big.\n");
         va_end(arglist);
         return (-1);
      }
   
      argv[c] = va_arg(arglist, const char*);
      if(argv[c] == NULL) {
         //printf("NULL\n");
         break;
      }
      c++;
   }
   va_end(arglist);

   /*
    * now pack args
    */
   TRACE("now trying to pack args...\n");
   //pack_args(c, argv, args);

   TRACE("now executing...\n");
   //return(exec(filename, args));
   return (exec_proc(filename, c, argv));
}

int
execle(const char* filename, const char* argv0, .../*, const char* const* env*/)
{
}

int
execlp(const char* filename, const char* argv0, ...)
{
}

int
execlpe(const char* filename, const char* argv0, .../*, const char* const* env*/)
{
}

int
execv(const char* filename, const char* const* argv)
{
}

int
execve(const char* filename, const char* const* argv, const char* const* env)
{
}

int
execvp(const char* filename, const char* const* argv)
{
}

int
execvpe(const char* filename, const char* const* argv, const char* const* env)
{
}
