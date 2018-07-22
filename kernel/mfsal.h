#ifndef KERNEL_MFSAL_H
#define KERNEL_MFSAL_H
//
// Millennium Filesytem Abstraction Layer
// Copyright 1997-1998 (c) Leprechaun Software. All Rights Reserved.
//

typedef struct
{
	char	*ShortDescription;
	char	*LongDescription;

	void	(*Open);
	void	(*Close);
	void	(*Create);
	void	(*Delete);
	void	(*Read);
	void	(*Write);
	void	(*Copy);

} TFilesystem;

#endif