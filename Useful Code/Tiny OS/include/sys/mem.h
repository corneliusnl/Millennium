/*
 * misc memory and paging defines
 *
 * $Id$
 *
 */
#ifndef _MEM_H_
#define _MEM_H_

#define PTE_SYSBITS     (0x3FF)
#define PTE(x)          ((x) & (~0x3FF))
#define PTE_PADDR       (0xFFFFF000)
#define PADDR(x)        ((x) & PTE_PADDR)
#define PALIGN(x)       ((x) &= 0xFFFFF000)

#define PAGESZ    (4096)
#define NPTE      (1024)

#define PGSHIFT   (12)
#define ptob(x)   ((ulong)(x) << PGSHIFT)
#define btop(x)   ((ulong)(x) >> PGSHIFT)
#define btorp(x)  (((ulong)(x) + (PAGESZ-1)) >> PGSHIFT)

/*
 * page table entry access modes
 */
#define PA_USER   (0x7)
#define PA_KERN   (0x1)

/*
 * macros for retriving page dir and page table indexes
 * out of virtuall address
 */
#define IPD(va)   (((ulong)va) >> 22)
#define IPT(va)   ((((ulong)va) >> 12) & 0x3FF)

/*
 * some fake addresses
 */
#define INVALID_PTE  (0xFFFFA000)
#define FREE_PTE     (0xFFFFA000)
#define RESERVED_PTE (0x0000AAAA)

#define VA_1M  0x100000
#define VA_2M  0x200000
#define VA_3M  0x300000
#define VA_4M  0x400000
#define VA_8M  0x800000
#define VA_12M 0xC00000
#define VA_16M 0xF00000

#define VA_1G  0x40000000
#define VA_2G  0x80000000
#define VA_3G  0xC0000000
#define VA_4G  0xF0000000

#define VA_TOP 0xFFFFFFFF

#define VAPTE_0M  0
#define VAPTE_1M  256
#define VAPTE_2M  512
#define VAPTE_3M  768
#define VAPTE_4M  1024

#define VAPDE_0M  0
#define VAPDE_4M  1
#define VAPDE_8M  2
#define VAPDE_12M 3
#define VAPDE_16M 4

#define VAPDE_1G  256
#define VAPDE_2G  512
#define VAPDE_3G  768
#define VAPDE_4G  1024

#define PD_CODE      VAPDE_8M
#define PD_DATA      VAPDE_2G
#define PD_STACK     (VAPDE_4G-1)

#define PD_DATA_MAX  (VAPDE_4G-2)

#define VA_CODE      VA_8M
#define VA_DATA      VA_2G
#define VA_STACK     (VA_TOP-VA_4M)

#define EPGNO        (1)
#define VA_PENV      (VA_TOP-(PAGESZ*EPGNO)+1)

#define VA_MAPD      (VA_TOP-(PAGESZ*2)+1)
#define VA_MAPT      (VA_TOP-(PAGESZ*3)+1)

#define PD_VAS       (1)
#define VA_VAS       (VA_4M)

#endif /* _MEM_H_ */


