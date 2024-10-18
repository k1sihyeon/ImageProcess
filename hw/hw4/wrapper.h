#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>

extern int isSquare(int num);
extern size_t getSize(int fd);
extern int Open(const char *path, int oflag);
extern int Creat(const char *path, mode_t mode);
extern int Read(int filedes, void *buf, size_t nbyte);
extern int Write(int filedes, const void *buf, size_t nbyte);
extern void *Mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
extern int Stat(const char *restrict path, struct stat *restrict buf);
extern int Fstat(int fildes, struct stat *buf);
extern int Pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg);
extern int Pthread_join(pthread_t t, void **arg);
extern int Sem_init(sem_t *sem, int pshared, unsigned int value);
