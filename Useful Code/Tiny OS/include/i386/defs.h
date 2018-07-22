#ifndef _DEFS_H_
#define _DEFS_H_

#define REPINSW(port, ptr, count) \
	__asm__ __volatile__ ("rep\n\t" \
			      "insw" : : "d" (port), "D" (ptr), "c" (count))

#define REPOUTSW(port, ptr, count) \
	__asm__ __volatile__ ("rep\n\t" \
			      "outsw" : : "d" (port), "S" (ptr), "c" (count))

#define repinsw 	REPINSW
#define repoutsw 	REPOUTSW

#define FAST_COPY(src, dest, count) \
	__asm__ __volatile__ ("rep\n\t" \
			      "movsl"   \
			      :: "S" (src), "D" (dest), "c" (count))

#define LDCR3(val) \
	__asm__ __volatile__ ("movl %0, %%cr3\n\t", :: "r" (val))

#define CLI __asm__ __volatile__ ("cli")
#define STI __asm__ __volatile__ ("sti")

#endif /* _DEFS_H_ */
