/*
 * General disk and FAT filesystem structures
 * based on structures from VSTa DOS server
 * Copyright (c) 1996 Andy Valencia
 * Copyright (c) 1996 Bart Sekura
 *
 */
#ifndef _DOSFS_H_
#define _DOSFS_H_

/*
 * DOS boot structure
 */

struct dos_boot {
   uint  jmp1:16;
   uint  jmp2:8;
   uchar oem[8];
   uint  bps1:8;
   uint  bps2:8;
   uint  secperclust:8;
   uint  nres:16;
   uint  nfat:8;
   uint  rootent1:8;
   uint  rootent2:8;
   uint  nsec1:8;
   uint  nsec2:8;
   uint  mdb:8;
   uint  secperfat:16;
   uint  secpertrk:16;
   uint  sides:16;
   uint  nhid:32;
   uint  nsecbig:32;
   uint   pdno:16;
   uint  extbootsig:8;
   uint  vserial:32;
   uchar vlbl[11];
   uchar fsid[8];
};

/*
 * FAT file attribs
 */
#define FA_RDONLY 0x01
#define FA_HIDDEN 0x02
#define FA_SYSTEM 0x04
#define FA_LABEL  0x08
#define FA_DIREC  0x10
#define FA_ARCH      0x20

/*
 * directory entry in FAT cluster
 */

struct dir_entry {
   uchar name[8];
   uchar ext[3];
   uint  attr:8;
   uchar res[10];
   uint  time:16;
   uint  date:16;
   uint  start:16;
   uint  size:32;
};

#endif /* _DOSFS_H_ */
