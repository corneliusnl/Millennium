#ifndef _SHIFTOPS_H
#define _SHIFTOPS_H

#define MUL_1K(x)	((x) << 10)	
#define MUL_4K(x)	((x) << 12)
#define MUL_1M(x)	((x) <<	20)
#define MUL_4M(x)	((x) << 22)

#define MUL_NPTE(x) ((x) << 10)
#define DIV_NPTE(x) ((x) >> 10)

#define MUL_PAGESZ(x)	((x) << 12)

#endif /* _SHIFTOPS_H */
