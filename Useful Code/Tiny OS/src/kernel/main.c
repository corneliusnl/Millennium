/*
 * kernel main entry
 *
 * $Id: MAIN.C 1.5 1996/12/15 08:35:25 bart Exp $
 *
 */

#include <types.h>
#include <sys/tss.h>
#include <sys/desc.h>
#include <sys/pic.h>
#include <sys/cons.h>
#include <i386/inlines.h>

extern char kb_getch();

extern void init_call_gates(void);
extern void init_idt(void);

extern void wd_init();

extern void syscon();

extern void setup_physmem();

void init_tss_desc();
void tss_setup(struct tss*, ulong, ulong);
void create_task(ulong);
void init_tasks();
void sys_task();

void init();
void _init();
static void svga();

int
nvread(int port)
{
   outb(0x70, port & 0xFF);
   return(inb(0x71) & 0xFF);
}

void
show_time()
{
   int h = nvread(4);
   int m = nvread(2);
   int s = nvread(0);

   h = (((h >> 4) & 0x0F) * 10) + (h & 0x0F);
   m = (((m >> 4) & 0x0F) * 10) + (m & 0x0F);
   s = (((s >> 4) & 0x0F) * 10) + (s & 0x0F);

   printf("TIME: %d:%d:%d\n", h, m, s);
}

void
show_date()
{
   int y = nvread(9);
   int m = nvread(8);
   int d = nvread(7);
   int dw = nvread(6);

   y = (((y >> 4) & 0x0F) * 10) + (y & 0x0F);
   m = (((m >> 4) & 0x0F) * 10) + (m & 0x0F);
   d = (((d >> 4) & 0x0F) * 10) + (d & 0x0F);
   dw = (((dw >> 4) & 0x0F) * 10) + (dw & 0x0F);

   printf("DATE: %d:%d:%d   weekday: %d\n", y, m, d, dw);
}

#define BASELO 0x15
#define BASEHI 0x16
#define EXTLO  0x17
#define EXTHI  0x18

#include <boot/bootp.h>

static struct boot_params* bootp;
static char def_path[] = "/tinos/bin";
static char* path = def_path;

void
get_bootp()
{
   bootp = (struct boot_params*)0x37000;
   if(bootp->magic != 0xDEADBEEF) {
	printf("oops, boot parameters corrupted\n");
	bootp = NULL;
	return;
   }

   printf("magic:   %x\n", bootp->magic);
   printf("part_no: %d\n", bootp->part_no);
   printf("path:    %s\n", bootp->path);
   path = bootp->path;
}

/* 
 * main entry point
 */

#include "kernel.id"

void 
entry()
{
   int c;
   int base, ext;
   ulong total;

   //clrscr();
   get_bootp();
   printf("TINOS kernel: %s\n", kernel_id);

   base = nvread(BASELO) | (nvread(BASEHI) << 8);
   ext = nvread(EXTLO) | (nvread(EXTHI) << 8);
   total = (base + ext)*1024L;
   printf("%d base mem, %d extended, %d bytes total\n", base, ext, total);

   init_idt();
   init_pic(0x20, 0x28);
   set_res();
   init_kmem();
   init_ktss();
   proc_init();
   //init_call_gates();
   init_syscall();
   init_msg();
   ltr(KTSS_SEL);

   /*
   enable_irq(0);
   enable_irq(1);
   */

   enable_irq(6);
   enable_irq(14);

   sti();

   wd_init();
   qio_init();
   fd_init();
   update_curpos();
   fsm_init();
   fsm_mount(bootp ? bootp->part_no : 0);
   pmm_init(total, 0);

   init();

   enable_irq(1);
   syscon();
   
}

void
init()
{
   ulong pid;

   if(chdir(path) == -1) {
      printf("init(): chdir(%s) failed.\n", path);
      return;
   }

   pid = _create_proc(NULL, "init");
   if(pid == 0) {
      printf("init(): create_process() failed.\n");
      return;
   }
   proc_prio(pid, 0);
   enable_irq(0);
   enable_irq(1);
   _exec_proc(pid, path);

   printf("FATAL SHIT!!!\n");
   disable_irq(0);
}

int
kyesno()
{
   char ch;
   printf("continue?(y/n)\n");
   while(1) {
      ch = kb_getch();
      if(ch == 'y' || ch == 'Y')
         return (1);
      if(ch == 'n' || ch == 'N')
         return (0);
   }
}
