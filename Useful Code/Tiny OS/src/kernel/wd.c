/*
 * IDE disk driver
 * based on VSTa's wd server
 * Copyright (c) 1993-1996 Andy Valencia
 * Copyright (c) 1996 Bart Sekura
 *
 * $Id: WD.C 1.4 1996/12/15 08:38:10 bart Exp $
 *
 */

#include <types.h>
#include <bcopy.h>
#include <sys/wd.h>
#include <sys/thread.h>
#include <i386/inlines.h>
#include <i386/defs.h>

#include <trace.h>

static uchar buffer[512];

static struct wd_parms     parms;
static struct disk_parms   d_parms;

static uint cur_unit = 0;
static ulong cur_sec = 0;
static uint cur_nsec = 0;
static uint cur_xfer = 0;
static void* cur_buf = NULL;
static uint cur_op = WDC_READ;

/*
 * volatile modifier is essential
 * since this variable can be changed by isr
 * during sequential io we while() on it...
 */
static volatile int busy = 0;

#define DELAY_LOOP(count) for(c = 0; c < count; c++)

extern struct tcb* cur_tcb;

static void wd_start();

/*
 * XXX
 * that shouldn't be here
 */
static void 
swab(uchar* ptr, uint count)
{
   char* p, temp;
   int c;

   p = ptr;
   for(c = 0; c < count; c += 2, p += 2) {
      temp = p[0];
      p[0] = p[1];
      p[1] = temp;
   }
}
   
static int 
wd_check_busy()
{
   uchar status;
   uint c = 1000000;

   while(c--) {
      status = inb(WD_BASEIO+WD_STATUS);
      if(status == 0xFF) {
         print("status : 0xFF\n");
         return (-1);
         /*
         outb(WD_BASEIO+WD_SDH, (0xA0 | ( 0 << 4 )));
         status = inb(WD_BASEIO+WD_STATUS);
         */
      }
      if((status & WDS_BUSY) == 0) {
         return (0);
      }
   }
   
   return (-1);
}

static int 
wd_cmd(uchar cmd)
{
   uint c;
   uchar stat;
   uint timeout;

   timeout = 10000000;
   
   c = timeout;
   while(inb(WD_BASEIO+WD_STATUS) & WDS_BUSY) {
      if(--c == 0) {
         return(-1);
      }
   }

   TRACE("not busy\n");

   outb(WD_BASEIO+WD_CMD, cmd);

   c = timeout;
   while(1) {
      stat = inb(WD_BASEIO+WD_STATUS);
      if((stat & WDS_BUSY) == 0) {
         return (stat);
      }
      if(--c == 0) {
         return(-1);
      }
   }
}

static int 
readp_data()
{
   uint c = 100000;
   uint stat;

   while(c--) {
      stat = inb(WD_BASEIO+WD_STATUS);
      if(stat & WDS_ERROR) {
         print("WDS_ERROR\n");
         return (-1);
      }
      if(stat & WDS_DRQ) {
         return (stat);
      }
   }
   print("TIMEOUT\n");
   return (-1);
}

static void 
wd_readp(int unit)
{
   uint c;

   outb(WD_BASEIO+WD_SDH, WDSDH_EXT|WDSDH_512|(unit<<4));
   if(wd_cmd(WDC_READP) < 0) {
      print("readp failed.\n");
      return;
   }
   if(readp_data() < 0 ) {
      print("readp_data failed\n");
      return;
   }

   repinsw(WD_BASEIO+WD_DATA, buffer, 256);

   bcopy(buffer, &parms, sizeof(parms));

   swab(parms.w_model, MODEL_SZ);
   parms.w_model[MODEL_SZ-1] = 0;
   swab(parms.w_rev, REV_SZ);
   parms.w_rev[REV_SZ-1] = 0;
   swab(parms.w_cnsn, CNSN_SZ);
   parms.w_model[CNSN_SZ-1] = 0;

   TRACE("READP results:\n");
   TRACE1("rev  : %s\n", parms.w_rev);
   TRACE1("model: %s\n", parms.w_model);
   TRACE1("heads   : %d\n", parms.w_heads);
   TRACE1("fixedcyl: %d\n", parms.w_fixedcyl);
   TRACE1("removcyl: %d\n", parms.w_removcyl);
   TRACE1("sectors : %d\n", parms.w_sectors);

   printf("wd_init(): model %s\n", parms.w_model);
   printf("wd_init(): CSH: %d, %d, %d\n", 
      parms.w_fixedcyl + parms.w_removcyl,
      parms.w_sectors, parms.w_heads);

   for(c = 0; c < 100000; c++);
   outb(WD_BASEIO+WD_SDH, 
        WDSDH_EXT|WDSDH_512|(unit<<4)|(parms.w_heads-1));
   outb(WD_BASEIO+WD_SCNT, parms.w_sectors);
   if( wd_cmd(WDC_SPECIFY) < 0 ) {
      print("wdc_specify failed\n");
      return;
   }
   print("wd_init(): specify succeeded.\n");

   d_parms.w_cyls = parms.w_fixedcyl + parms.w_removcyl;
   d_parms.w_tracks = parms.w_heads;
   d_parms.w_secpertrk = parms.w_sectors;
   d_parms.w_secpercyl = parms.w_heads * parms.w_sectors;
   d_parms.w_size = d_parms.w_secpercyl * d_parms.w_cyls;

   TRACE1("cyls     : %d\n", d_parms.w_cyls);
   TRACE1("tracks   : %d\n", d_parms.w_tracks);
   TRACE1("secpertrk: %d\n", d_parms.w_secpertrk);
   TRACE1("secpercyl: %d\n", d_parms.w_secpercyl);
   TRACE1("size     : %d\n", d_parms.w_size);

   printf("wd_init(): cyls %d, tracks %d, secpertrk %d, secpercyl %d\n",
          d_parms.w_cyls, d_parms.w_tracks, 
          d_parms.w_secpertrk, d_parms.w_secpercyl);
   printf("wd_init(): size %d\n", d_parms.w_size);
}

static void 
wd_ldsect()
{
   repoutsw(WD_BASEIO+WD_DATA, cur_buf, SECSZ/sizeof(ushort));

   cur_buf += SECSZ;
   cur_sec++;
   cur_nsec--;
   cur_xfer--;
}

void 
wd_isr()
{
   uint stat;

   stat = inb(WD_BASEIO+WD_STATUS);
   if(stat & WDS_ERROR) {  
      print("HARD DISK ERROR!!!\n");
      busy = 0;
      return;
   }

   if(busy == 0) {
      TRACE("WEIRD HARD DISK IRQ...\n");
      return;
   }

   if(cur_op == WDC_READ) {
      repinsw(WD_BASEIO+WD_DATA, cur_buf, SECSZ/sizeof(ushort));

      cur_buf += SECSZ;
      cur_sec++;
      cur_nsec--;
      cur_xfer--;
      if(cur_xfer == 0)
         goto finito;
   } else {
      if(stat & WDS_DRQ) {
         wd_ldsect();
      } else {
         goto finito;
      }
   }
   
   return;

finito:
   if( cur_nsec > 0 ) {
      wd_start();
   } else {
      busy = 0;
      io_done();
   }   
}

static void 
wd_start()
{
   uint cyl, snum, trk, sect_left;
   int c,k;
   uint unit = 0;

   c = 0;
   while(inb(WD_BASEIO+WD_STATUS) & WDS_BUSY) {
      if(++c > 1000) {
         printf("wd_read: unit busy, timeout\n");
         busy = 0;
         return;
      }
   }

   cyl = cur_sec / d_parms.w_secpercyl;
   snum = cur_sec % d_parms.w_secpercyl;
   trk = snum / d_parms.w_secpertrk;
   snum = (snum % d_parms.w_secpertrk) + 1;
   sect_left = d_parms.w_secpertrk + 1 - snum;

   if( cur_nsec > sect_left ) {  
      cur_xfer = sect_left;
   } else {
      cur_xfer = cur_nsec;
   }

   TRACE5("cyl:%d; snum:%d; trk:%d; sect_left:%d; cur_xfer:%d\n", 
           cyl, snum, trk, sect_left, cur_xfer);

   outb(WD_BASEIO+WD_SCNT, cur_xfer);
   outb(WD_BASEIO+WD_SNUM, snum);
   outb(WD_BASEIO+WD_CYL0, cyl & 0xFF);
   outb(WD_BASEIO+WD_CYL1, (cyl >> 8) & 0xFF);
   outb(WD_BASEIO+WD_SDH, WDSDH_EXT | WDSDH_512 | trk | (unit << 4));
   outb(WD_BASEIO+WD_CMD, cur_op);

   if(cur_op == WDC_WRITE) {
      while((inb(WD_BASEIO+WD_STATUS) & WDS_DRQ) == 0);
      wd_ldsect();
   }
}

void 
wd_io(void* buf, uint sec, uint nsec)
{
   if(sec + nsec > d_parms.w_size) {
      printf("wd_io(): wrong transfer parameters specified\n");
      return;
   }

   cur_unit = 0;
   cur_sec = sec;
   cur_nsec = nsec;
   cur_buf = buf;
   cur_op = WDC_READ;

   busy = 1;
   
   wd_start();
}

void 
wd_iow(void* buf, uint sec, uint nsec)
{
   if(sec + nsec > d_parms.w_size) {
      printf("wd_iow(): wrong transfer parameters specified\n");
      return;
   }

   cur_unit = 0;
   cur_sec = sec;
   cur_nsec = nsec;
   cur_buf = buf;
   cur_op = WDC_WRITE;

   busy = 1;

   wd_start();
}

void 
wd_init()
{
   uint c;
   
   /* 
    * reset motherfucker 
    */
   outb(WD_BASEIO+WD_CTRL, CTRL_RESET | CTRL_IDIS);
   DELAY_LOOP(1000);
   outb(WD_BASEIO+WD_CTRL, CTRL_IDIS);
   DELAY_LOOP(1000000);

   print("wd_init(): trying to diag...");

   if( wd_check_busy() == -1 ) {
      print("wd_init(): disk busy...\n");
      return;
   }

   /*
    * diag that sonofabitch
    */
   if( wd_cmd(WDC_DIAG) < 0 ) {
      print("failed self diags\n");
      return;
   }

   inb(WD_BASEIO+WD_ERROR);

   /*
    * enable interrupts
    */
   outb(WD_BASEIO+WD_CTRL, CTRL_4BIT);

   DELAY_LOOP(100000);
   printf("ok\n");
   
   wd_readp(0);
}

uint
wd_getsz()
{
   return (d_parms.w_size);
}

struct disk_parms* 
wd_getparms()
{
   return &d_parms;
}

int
wd_is_busy()
{
   return (busy);
}

void io_start(uint, void*, uint, uint);

static volatile int async = 0;

void
wd_async()
{
	async = !async;
}

/*
 * IO requests
 * XXX lame, needs revision badly
 */
void
reads(void* p, uint sect, uint ns)
{
   if(async) {
	io_start(1, p, sect, ns);
	return;
   }
   wd_io(p, sect, ns);
   while(busy);
}

void
writes(void* p, uint sect, uint ns)
{
	if(async) {
		io_start(2, p, sect, ns);
		return;
	}
	wd_iow(p, sect, ns);
	while(busy);
}
