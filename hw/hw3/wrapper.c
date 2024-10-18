#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <string.h>
#include <math.h>
#include "wrapper.h"

int isSquare(int num) {
  int res = (int)round(sqrt(num));

  return (res * res == num);
}

size_t getSize(int fd) {
  struct stat statbuf;
  Fstat(fd, &statbuf);

  return (statbuf.st_size / sizeof(double));
}

int Open(const char *path, int oflag)
{
  int fd = open(path, oflag);

  if (fd < 0){
    fprintf(stderr, "file(%s) open error:%s\n", path, strerror(errno));
    exit(1);
  }
  return(fd);
}

int Creat(const char *path, mode_t mode)
{
  int fd = creat(path, mode);

  if (fd < 0){
    fprintf(stderr, "file(%s) creat error:%s\n", path, strerror(errno));
    exit(1);
  }
  return(fd);
}

int Read(int filedes, void *buf, size_t nbyte)
{
  int res = read(filedes, buf, nbyte);

  if (res < 0){
    if (errno == EINTR)
      fprintf(stderr, "read is stopped by signal.\n");
    else
      fprintf(stderr, "read error: %s.\n", strerror(errno));
    exit(1);
  }
  return(res);
}

int Write(int filedes, const void *buf, size_t nbyte)
{
  int res = write(filedes, buf, nbyte);

  if (res < 0){
    if (errno == EINTR)
      fprintf(stderr, "write is stopped by signal.\n");
    else
      fprintf(stderr, "write error: %s.\n", strerror(errno));
    exit(1);
  }
  return(res);
}

void *Mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
{
  void *res = mmap(addr, length, prot, flags, fd, offset);

  if (res < 0){
    fprintf(stderr, "mmap error: %s.\n", strerror(errno));
    exit(1);
  }
  return(res);
}

int Stat(const char *restrict path, struct stat *restrict buf)
{
  int res = stat(path, buf);

  if (res < 0){
    fprintf(stderr, "stat error on %s: %s.\n", path, strerror(errno));
    exit(1);
  }
  return(res);
}

int Fstat(int fildes, struct stat *buf)
{
  int res = fstat(fildes, buf);

  if (res < 0){
    fprintf(stderr, "fstat error: %s.\n", strerror(errno));
    exit(1);
  }
  return(res);
}

int Pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg)
{
  int res = pthread_create(thread, attr, start_routine, arg);

  if (res < 0){
    fprintf(stderr, "pthread_create error: %s.\n", strerror(errno));
    exit(1);
  }
  return(res);
}

int Pthread_join(pthread_t t, void **arg) {
  int res = pthread_join(t, arg);

  if (res < 0){
    fprintf(stderr, "pthread_join error: %s.\n", strerror(errno));
    exit(1);
  }
  return(res);
}

int Sem_init(sem_t *sem, int pshared, unsigned int value)
{
  int res = sem_init(sem, pshared, value);

  if (res < 0){
    fprintf(stderr, "sem_init error: %s.\n", strerror(errno));
    exit(1);
  }
  return(res);
}
