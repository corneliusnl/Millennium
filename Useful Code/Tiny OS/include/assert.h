/*
 * Copyright (c) DJ Delorie
 * assertion stuff
 */

#undef ASSERT

#if defined(NDEBUG)
#define ASSERT(test)
#else
#define ASSERT(test)((test)?0: __assertion(#test,__FILE__, __LINE__))
#endif 

#ifndef _ASSERT_H
#define _ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

void __assertion(const char*, const char*, int);

#ifdef __cplusplus
}
#endif

#endif /* _ASSERT_H */
