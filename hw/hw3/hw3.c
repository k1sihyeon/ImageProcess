/* It computes matrix multiplication. Reads two matrix, multiply them, stores the result matrix */
/* Usage: ./hw3 data1.bin data2.bin res.bin */
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

typedef struct matidx {
  int tidx;
  int thnum;
  int matsize;
} matidx;

//global var
double *mat_A, *mat_B;

void *thfunc(void *arg) {

  matidx *matidxs = (matidx *) arg;

  int tidx = matidxs->tidx;
  int matsize = matidxs->matsize;
  int thnum = matidxs->thnum;

  int start = tidx * matsize / thnum;
  int end = (tidx + 1) * matsize / thnum - 1;
  int partsize = end - start + 1;

  double *mat_C = malloc(sizeof(double) * matsize * partsize);

  //printf("thread #%d created\n", tidx);

  start_timelog(tidx);

  if (matsize == 1) {
    mat_C[0] = mat_A[0] * mat_B[0];
  }
  else {

    for (int row = start; row <= end; row++) {
      for (int col = 0; col < matsize; col++) {
        for (int k = 0; k < matsize; k++) {
          mat_C[(row - start) * matsize + col] += mat_A[row * matsize + k] * mat_B[k * matsize + col];
        }
      }
    }

  }

  finish_timelog(tidx);

  //printf("thread #%d finished\n", tidx);

  return (void *) mat_C;
}

int main(int argc, char *argv[]) {

  if (argc != 5)   {
    fprintf(stderr, "Usage: ./hw3 infile1 infile2 outfile p\n");
    return (1);
  }

  int ifd1, ifd2, ofd;
  size_t size1, size2;

  ifd1 = Open(argv[1], O_RDONLY);
  ifd2 = Open(argv[2], O_RDONLY);

  size1 = getSize(ifd1);
  size2 = getSize(ifd2);

  int matsize = (int)sqrt(size1);
  int thnum = atoi(argv[4]);

  if (thnum > matsize) {
    thnum = matsize;
  }

  if (!isSquare(size1)) {
    fprintf(stderr, "Error: The 1st input file size is not n*n\n");
    return (1);
  }

  if (size1 != size2) {
    fprintf(stderr, "Error: The 2nd input file size is not the same as the 1st one\n");
    return (1);
  }

  if (thnum <= 0) {
    fprintf(stderr, "Error: The 4th parameter should be an integer greater than zero\n");
    return (1);
  }

  ofd = Creat(argv[3], 0644);

  mat_A = (double *) Mmap(NULL, sizeof(double) * matsize * matsize, PROT_READ, MAP_SHARED, ifd1, 0);
  mat_B = (double *) Mmap(NULL, sizeof(double) * matsize * matsize, PROT_READ, MAP_SHARED, ifd2, 0);

  close(ifd1);
  close(ifd2);

  pthread_t *thids = (pthread_t *)malloc(sizeof(pthread_t) * thnum);
  matidx *matidxs = (matidx *)malloc(sizeof(matidx) * thnum);

  init_timelog(thnum);

  for (int i = 0; i < thnum; i++) {
    matidxs[i].tidx = i;
    matidxs[i].thnum = thnum;
    matidxs[i].matsize = matsize;
    Pthread_create(&thids[i], NULL, thfunc, (void *)&matidxs[i]);
  }

  int tidx, start, end, partsize;
  double *mat_C;
  for(int i = 0; i < thnum; i++) {
    
    tidx = i;
    start = tidx * matsize / thnum;
    end = (tidx + 1) * matsize / thnum - 1;
    partsize = end - start + 1;

    Pthread_join(thids[i], (void **)&mat_C);
    //printf("thread #%d joined\n", tidx);

    lseek(ofd, start * matsize * sizeof(double), SEEK_SET);
    Write(ofd, mat_C, sizeof(double) * matsize * partsize);
    //printf("thread #%d file write done\n", tidx);
  }

  close_timelog();

  close(ofd);

  return (0);
}
