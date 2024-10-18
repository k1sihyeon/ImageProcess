/* It computes matrix multiplication. Reads two matrix, multiply them, stores the result matrix */
/* Usage: ./hw4 data1.bin data2.bin res.bin */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <semaphore.h>
#include "wrapper.h"
#include "timeft.h"

#define bufSize 10

typedef struct matidx {
  int tidx;
  //int ridx;
  int matsize;
} matidx;

typedef struct Queue
{
    int front, rear;
    int *data;
} Queue;


//global var
double *mat_A, *mat_B, *mat_C;
sem_t empty;
sem_t full;
sem_t mutex;

//bounded buffer
Queue boundedBuffer;



void Enqueue(Queue * q, int data) //put 
{

  sem_wait(&empty);
  sem_wait(&mutex);

  q->rear = (q->rear+1) % bufSize;
  q->data[q->rear] = data;

  sem_post(&mutex);
  sem_post(&full);

  return;
}

 
int Dequeue(Queue * q) //get
{

  q->front = (q->front+1) % bufSize;

  return q->data[q->front];

}



void *thfunc(void *arg) {

  matidx *matidxs = (matidx *) arg;

  int tidx = matidxs->tidx;
  int matsize = matidxs->matsize;

  //printf("thread #%d created\n", tidx);

  

  int idx = 0;
  while(idx != -1) {
    

    sem_wait(&full);
    sem_wait(&mutex);

    idx = Dequeue(&boundedBuffer);

    sem_post(&mutex);
    sem_post(&empty);
    
    //printf("thread #%d : boundedBuff %d dequeued\n", tidx, idx);
    


    if(idx == -1)
      break;

   
    start_timelog(tidx);

    if (matsize == 1) {
      mat_C[0] = mat_A[0] * mat_B[0];
    }
    else {

      for (int row = idx; row <= idx; row++) {
        for (int col = 0; col < matsize; col++) {
          for (int k = 0; k < matsize; k++) {
            mat_C[row * matsize + col] += mat_A[row * matsize + k] * mat_B[k * matsize + col];
          }
        }
      }
    }

    finish_timelog(tidx);

  }

  //printf("thread #%d finished\n", tidx);
  

  return;
}

int main(int argc, char *argv[]) {

  if (argc != 5)   {
    fprintf(stderr, "Usage: ./hw4 infile1 infile2 outfile p\n");
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
  mat_C = (double *) malloc(sizeof(double) * matsize * matsize);

  close(ifd1);
  close(ifd2);

  pthread_t *thids = (pthread_t *)malloc(sizeof(pthread_t) * thnum);
  matidx *matidxs = (matidx *)malloc(sizeof(matidx) * thnum);

  boundedBuffer.front = 0;
  boundedBuffer.rear = 0;
  boundedBuffer.data = malloc(sizeof(int) * bufSize);

  Sem_init(&empty, 0, bufSize);
  Sem_init(&full, 0, 0);
  Sem_init(&mutex, 0, 1);


  init_timelog(thnum);

  //circular queue enqueue
  //dequeue
  //sentinal value




  for (int i = 0; i < thnum; i++) {
    matidxs[i].tidx = i;
    matidxs[i].matsize = matsize;
    Pthread_create(&thids[i], NULL, thfunc, (void *)&matidxs[i]);
    //printf("thread #%d created\n", i);
  }

  for(int i = 0; i < matsize; i++) {
    Enqueue(&boundedBuffer, i);
    //printf("boundedBuff %d enqueued\n", i);
  }

  for(int i = 0; i < bufSize; i++) {
    Enqueue(&boundedBuffer, -1);
  }

  //enqueue 종료 시 worker thread에게 sentinel 값 bounded buffer 통해 전달 : -1

  for(int i = 0; i < thnum; i++) {

    Pthread_join(thids[i], NULL);
    //printf("thread #%d joined\n", i);
  }

  Write(ofd, mat_C, sizeof(double) * matsize * matsize);
  //printf("file write done\n");

  close_timelog();

  close(ofd);

  return (0);
}
