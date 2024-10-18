/* It computes matrix multiplication. Reads two matrix, multiply them, stores the result matrix */
/* Usage: ./hw2 data1.bin data2.bin res.bin N */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "wrapper.h"
#include "timeft.h"

int isSquare(int num) {
  int res = (int)round(sqrt(num));

  return (res * res == num);
}

size_t getSize(int fd) {
  struct stat statbuf;
  Fstat(fd, &statbuf);

  return (statbuf.st_size / sizeof(double));
}

int main(int argc, char *argv[]) {

  if (argc != 5)   {
    fprintf(stderr, "Usage: ./hw2 infile1 infile2 outfile p\n");
    return (1);
  }

  int ifd1, ifd2, ofd;
  size_t size1, size2;

  ifd1 = Open(argv[1], O_RDONLY);
  ifd2 = Open(argv[2], O_RDONLY);

  size1 = getSize(ifd1);
  size2 = getSize(ifd2);

  int n = (int)sqrt(size1);
  int cpnum = atoi(argv[4]);

  if (cpnum > n) {
    cpnum = n;
  }

  if (!isSquare(size1)) {
    fprintf(stderr, "Error: The 1st input file size is not n*n\n");
    return (1);
  }

  if (size1 != size2) {
    fprintf(stderr, "Error: The 2nd input file size is not the same as the 1st one\n");
    return (1);
  }

  if (cpnum <= 0) {
    fprintf(stderr, "Error: The 4th parameter should be an integer greater than zero\n");
    return (1);
  }

  ofd = Creat(argv[3], 0644);

  double *mat_A, *mat_B;
  mat_A = (double *) Mmap(NULL, sizeof(double) * n * n, PROT_READ, MAP_SHARED, ifd1, 0);
  mat_B = (double *) Mmap(NULL, sizeof(double) * n * n, PROT_READ, MAP_SHARED, ifd2, 0);

  close(ifd1);
  close(ifd2);

  pid_t *pids = (pid_t *)malloc(sizeof(pid_t) * cpnum);

  init_timelog(cpnum);

  for (int i = 0; i < cpnum; i++) {
    int tidx = i;
    pids[tidx] = fork();

    if (pids[tidx] == -1) {
      fprintf(stderr, "fork err");
    }
    else if (pids[tidx] == 0) {

      int start = tidx * n / cpnum;
      int end = (tidx + 1) * n / cpnum - 1;
      int matSize = end - start + 1;

      double *mat_C = malloc(sizeof(double) * n * matSize);

      if(n == 1) {
        mat_C[0] = mat_A[0] * mat_B[0];
      }
      else {

        start_timelog(tidx);

        for (int row = start; row <= end; row++) {
          for (int col = 0; col < n; col++) {
            for (int k = 0; k < n; k++) {
              mat_C[(row - start) * n + col] += mat_A[row * n + k] * mat_B[k * n + col];
            }
          }
        }

        finish_timelog(tidx);
      }

      lseek(ofd, start * n * sizeof(double), SEEK_SET);
      Write(ofd, mat_C, sizeof(double) * n * matSize);

      exit(0);
    }
  }

  for(int i = 0; i < cpnum; i++) {
    wait(&pids[i]);
  }

  close_timelog();

  close(ofd);

  return (0);
}
