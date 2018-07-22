#ifndef KERNEL_MDAL_H
#define KERNEL_MDAL_H
//
// Millennium Disk Abstraction Layer
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//

//
// Disk information structure
//
typedef struct
{
	int	Type;
	void	(*Read)();
	void	(*Write)();

} TDiskInfo;

//
// Disk types
//
#define DSK_CDROM	0
#define DSK_FLOPPY	1
#define DSK_HARD	2

//
// Disk functions
//
void	RegisterDisk(TDiskInfo *Disk);

#endif