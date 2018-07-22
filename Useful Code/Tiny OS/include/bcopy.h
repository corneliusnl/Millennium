#ifndef _BCOPY_H
#define _BCOPY_H

/*
 * bzero(), bcopy() - very useful ones
 */

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

void bcopy(const void *src, void *dest, size_t n);
void bzero(void *s, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* _BCOPY_H */
