#include <types.h>
#include <sys/mem.h>
#include <sys/vmm.h>
#include <sys/calls.h>
#include <sys/argframe.h>
#include <coff.h>
#include <fcntl.h>

//#define _DEBUG
#include <trace.h>

#ifdef _DEBUG
static void
testmem(void* ptr, uint cb)
{
   uint c;
   uchar* p = (uchar*)ptr;

   printf("testmem(): p = %x, testing it ...", (ulong)p);
   for(c = 0; c < cb; c++)
      p[c] = 0x00;
   printf("ok\n");
}

static void
dumpit(void* ptr, uint nelem)
{
   uint c;
   uchar* p = (uchar*)ptr;


   for(c = 0; c < nelem; c++)
	printf("%x ", *p++);
   putc('\n');
}
#endif

static void
setup_args(int argc, char** argv, void* pdata, ulong base)
{
   ulong c, len;
   struct arg_frame* af = (struct arg_frame*)pdata;
   void* p = af->args;
   char* ptr = (char*)(base + ((ulong)af->args - (ulong)pdata));

   TRACE1("ptr = %x\n", (ulong)ptr);

   if(argc > MAX_ARGNO) {
	TRACE("setup_args(): argc > MAX_ARGNO, trimming...\n");
	argc = MAX_ARGNO;
   }
   
   len = 0;
   for(c = 0; c < argc; c++) {
      uint l = strlen(argv[c])+1;
      len += l;
      if(len > MAX_ARGSZ) {
	 len -= l;
         TRACE("setup_args(): oops, len > MAX_ARGSZ");
         break;
      }
      bcopy(argv[c], p, l);
      af->argv[c] = ptr;
      TRACE4("argv[%d] = %s, l = %d, ptr = %x\n", 
	      c, argv[c], l, (ulong)af->argv[c]);
      p += l;
      ptr += l;
   }
   af->argc = c;   

   TRACE1("setup_args(): len = %d\n", len);
}

/*
 */
ulong
exec_proc(char* s, int argc, char** argv)
{
   int h;
   void* va;
   void* p;
   uint c;
   ulong vaddr, doff, pid;
   uint npg, sz;
   FILHDR hdr;
   AOUTHDR aout;
   SCNHDR tscn, dscn, bscn;

   va = NULL;

   /*
    * read process image file header
    */
   h = open(s, O_RDONLY);
   if(h == -1) {
      printf("exec_proc(): open failed\n");
      return(0);
   }
   
   /*
    * read sections
    * ugly goto's for cleaner, faster code
    */

   if(read(h, (void*)&hdr, sizeof(hdr)) < 0) goto failure;

   if(I386BADMAG(hdr)) {
      printf("exec_proc(): not an executable format\n");
      return(-1);
   }

   if(read(h, (void*)&aout, sizeof(aout)) < 0) goto failure;
   if(read(h, (void*)&tscn, sizeof(tscn)) < 0) goto failure;
   if(read(h, (void*)&dscn, sizeof(dscn)) < 0) goto failure;
   if(read(h, (void*)&bscn, sizeof(bscn)) < 0) goto failure;

#ifdef _DEBUG
   printf("hdr.magic %x, hdr.nscns %x\n", hdr.f_magic, hdr.f_nscns);
   printf("aout.tsize %x, aout.dsize %x, aout.bsize %x, aout.entry 0x%x\n", 
           aout.tsize, aout.dsize, aout.bsize, aout.entry);
   printf("t.name: %s, t.vaddr: %x, t.size: %x, t.ptr: %x\n", 
           tscn.s_name, tscn.s_vaddr, tscn.s_size, tscn.s_scnptr);
   printf("d.name: %s, d.vaddr: %x, d.size: %x, d.ptr: %x\n", 
           dscn.s_name, dscn.s_vaddr, dscn.s_size, dscn.s_scnptr);
   printf("b.name: %s, b.vaddr: %x, b.size: %x, b.ptr: %x\n", 
           bscn.s_name, bscn.s_vaddr, bscn.s_size, bscn.s_scnptr);

   printf("exec_proc(): coff image read.\n");
#endif

   TRACE1("exec_proc(): last addr = %x\n", bscn.s_vaddr + bscn.s_size);
   sz = (bscn.s_vaddr + bscn.s_size) - VA_CODE;
   TRACE1("exec_proc(): whole size = %x, ", sz);
   npg = btorp(sz);
   TRACE1("which is %d pages...\n", npg);
   
   va = mmap(npg, VM_ALLOC);
   if(va == NULL) {
      printf("exec_proc(): va = NULL\n");
      goto failure;
   }

#ifdef _DEBUG
   testmem(va, npg*PAGESZ);
#endif

   doff = dscn.s_vaddr - VA_CODE;
   vaddr = (ulong)va + doff;
   TRACE2("exec_proc(): doff = %x, vaddr = %x\n", doff, vaddr); 

   if(seek(h, tscn.s_scnptr) == -1) {
      printf("exec_proc(): seek to text failed\n");
      goto failure;
   }

   if(read(h, va, tscn.s_size) == -1) {
      printf("exec_proc(): read text failed\n");
      goto failure;
   }
#ifdef _DEBUG
   dumpit(va, 5);
#endif

   if(seek(h, dscn.s_scnptr) == -1) {
      printf("exec_proc(): seek to data failed\n");
      goto failure;
   }

   p = (void*)vaddr;
   if(read(h, p, dscn.s_size) == -1) {
      printf("exec_proc(): read data failed\n");
      goto failure;
   }
   setup_args(argc, argv, p, dscn.s_vaddr);

   close(h);

   TRACE("exec_proc(): process image has been loaded...\n");

   pid = os_exec2(s, va, npg, aout.entry);
   munmap(va, npg, VM_SETFREE);

   return(pid);

failure:
   if(va)
   	munmap(va, npg, VM_FREE);
   close(h);
   return(0);
}

