/*
 * boot parameters
 * passed to kernel by boot loader (coffload)
 */
#ifndef _BOOTP_H_
#define _BOOTP_H_

struct boot_params {
	unsigned long	magic;
	unsigned long	part_no;
	char		path[200];
};

#endif /* _BOOTP_H_ */
