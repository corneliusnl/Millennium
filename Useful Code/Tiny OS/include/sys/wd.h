/*
 * IDE hard disk defines, structures and prototypes
 * based on VSTa wd server
 * Copyright (c) 1996 Andy Valencia
 */
#ifndef _WD_H_
#define _WD_H_


#include <types.h>

#define  SECSZ 512

#define  WD_BASEIO1  0x1F0
#define  WD_BASEIO2  0x170

#define  WD_BASEIO   WD_BASEIO1

#define  WD_CTRL     0x206

#define  WD_DATA     0x00
#define  WD_ERROR    0x01
#define  WD_SCNT     0x02
#define  WD_SNUM     0x03
#define  WD_CYL0     0x04
#define  WD_CYL1     0x05
#define  WD_SDH      0x06
#define  WD_STATUS   0x07
#define  WD_CMD      0x07

#define  WDS_BUSY    0x80
#define  WDS_ERROR   0x01
#define  WDS_DRQ     0x08

#define  WDSDH_512   0x20
#define  WDSDH_EXT   0x80

#define  WDC_READ    0x20
#define  WDC_WRITE   0x30
#define  WDC_DIAG    0x90
#define  WDC_SPECIFY 0x91
#define  WDC_READP   0xEC

#define CTRL_IDIS    0x02
#define  CTRL_RESET  0x04
#define CTRL_4BIT    0x08

#define  CNSN_SZ     20
#define  REV_SZ      8
#define  MODEL_SZ    40

struct wd_parms {
   ushort   w_config;
   ushort   w_fixedcyl;
   ushort   w_removcyl;
   ushort   w_heads;
   ushort   w_unfbytespertrk;
   ushort   w_unfbytes;
   ushort   w_sectors;
   ushort   w_minisg;
   ushort   w_minplo;
   ushort   w_vendstat;
   uchar    w_cnsn[CNSN_SZ];
   ushort   w_cntype;
   ushort   w_cnsbsz;
   ushort   w_necc;
   uchar    w_rev[REV_SZ];
   uchar    w_model[MODEL_SZ];
   ushort   w_nsecperint;
   ushort   w_usedmovsd;
};

struct disk_parms {
   uint  w_cyls;
   uint  w_tracks;
   uint  w_secpertrk;
   uint  w_secpercyl;
   uint  w_size;
};

void wd_io(void*, uint, uint);
void reads(void*, uint, uint);
void wd_init();
uint wd_getsz();
struct disk_parms* wd_getparms();

#endif /* _WD_H_ */
