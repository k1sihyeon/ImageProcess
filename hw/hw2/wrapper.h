#include <sys/stat.h>
#include <fcntl.h>

extern int Open(const char *path, int oflag);
extern int Creat(const char *path, mode_t mode);
extern int Read(int filedes, void *buf, size_t nbyte);
extern int Write(int filedes, const void *buf, size_t nbyte);
extern void *Mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
extern int Stat(const char *restrict path, struct stat *restrict buf);
extern int Fstat(int fildes, struct stat *buf);

