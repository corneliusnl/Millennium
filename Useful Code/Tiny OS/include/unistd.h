#ifndef _UNISTD_H_
#define _UNISTD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>

#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2

#define STDIN_FILENO		0
#define STDOUT_FILENO		1
#define STDERR_FILENO		2

int	access(const char* path, int amode);
int	chdir(const char* path);
int	close(int h);
int	dup(int h);
int	dup2(int h, int h2);
char*	getcwd(char* buf, size_t size);
int     seek(int h, uint pos);
off_t	lseek(int h, off_t offset, int whence);
int	read(int h, void* buf, size_t cb);
int	rmdir(const char *_path);
uint	sleep(uint seconds);
int	unlink(const char* path);
int	write(int h, const void* buf, size_t cb);

#ifdef __cplusplus
}
#endif

#endif /* _UNISTD_H_ */
