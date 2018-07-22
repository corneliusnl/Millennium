/* 
 * Copyright (C) 1994 DJ Delorie, see COPYING.DJ for details 
 * Copyright (c) 1996 Bart Sekura
 */

#ifndef _STDDEF_H_
#define _STDDEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

#ifndef NULL  
#define NULL 0
#endif

#define offsetof(s_type, mbr) ((size_t) &((s_type *)0)->mbr)
typedef int ptrdiff_t;

#ifdef __cplusplus
}
#endif

#endif /* _STDDEF_H_ */
