#ifndef _STRING_H_
#define _STRING_H_

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

char*    strcpy(char*, const char*);
char*    strncpy(char*, const char*, int);
char*    strchr(const char*, int);
int      strlen(const char*);
int      strcmp(const char*, const char*);
int      strncmp(const char*, const char*, int);
ulong    strtoul(const char*, char**, int);
long     strtol(const char*, char**, int);
char*    strlwr(char*);
char*    strupr(char*);
size_t   strcspn(const char*, const char*);
int      strcoll(const char*, const char*);
char*    strpbrk(const char*, const char*);
size_t   strspn(const char*, const char*);
char*    strstr(const char*, const char*);
char*    strtok(char*, const char*);
size_t   strxfrm(char*, const char*, size_t);

void*    memchr(const void* s, int c, size_t n);
int      memcmp(const void* s1, const void* s2, size_t n);
void*    memset(void* dest, int c, size_t count);
void*    memcpy(void* dest, const void* src, size_t count);

#ifdef __cplusplus
}
#endif

#endif /* _STRING_H_ */
