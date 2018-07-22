/*
 * system calls
 */
#ifndef _CALLS_H_
#define _CALLS_H_

#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

int getch(void);
void exit(int code);
int exec(const char* filename, char* args);
int allocmsg(ulong msg);
int connectmsg(ulong msg);
int wait(int port, void* buf, size_t count);
int sendmsg(int port, void* buf, size_t count);
int waitpid(ulong pid);
void* mmap(size_t npg, ulong flags);
int munmap(void* ptr, size_t npg, ulong flags);
int pctl(uint pid, uint cmd, ulong val);
int thrctl(uint tid, uint cmd, ulong val);

#ifdef __cplusplus
}
#endif

#endif /* _CALLS_H_ */
