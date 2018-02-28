#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <omp.h>
#include <semaphore.h>
#include <determinant.h>

#include "common.h"
#ifndef OVERLAP
#define OVERLAP 8
#endif

static volatile bool terminate = false;
static volatile size_t life = 0;
static determinant_d_fn_t compute_fn;
static volatile size_t global_order = 0;
static sem_t write_allow_write_sems[OVERLAP];
static sem_t write_allow_read_sems[OVERLAP];
static sem_t comp_allow_write_sems[OVERLAP];
static sem_t comp_allow_read_sems[OVERLAP];
static volatile int write_allow_write_sems_b[OVERLAP];
static volatile int write_allow_read_sems_b[OVERLAP];
static volatile int comp_allow_write_sems_b[OVERLAP];
static volatile int comp_allow_read_sems_b[OVERLAP];
static long long write_values[OVERLAP][NUM_BULKS];
static int *buffer1[OVERLAP];
static int *buffer2[OVERLAP];

void measurement(size_t tid) {
  printf("[tid:%zu]->Start measurement...\n", tid);
  FILE *fp = fopen("result.output", "w");
  if (!fp) {
    perror("Failed to open the output file...");
    return;
  }

  size_t i;
  int fd;
  char d[64];
  int prev_num = 0;
  for (i = 0; i < LIFE; ++i) {
    sleep(SEC_SLEEP);
    /*Reading number of determinant calculations*/
    printf("[tid:%zu]->Write out buffer\n", tid);
    fd = open("/sys/module/dummy/parameters/no_of_det_cals", O_RDONLY);
    size_t j = 0;
    while (true) {
      read(fd, &d[j], 1);
      if (d[j] == '\n')
        break;
      ++j;
    }
    d[j] = '\0';
    int num = atoi(d);
    printf("[tid:%zu]->Throughput %d\n", tid, num - prev_num);
    fprintf(fp, "%d\n", num - prev_num);
    prev_num = num;
    __sync_add_and_fetch(&life, 1);
  }

  terminate = true;

  close(fd);
  fclose(fp);
  printf("[tid:%zu]->End measurement...\n", tid);
}


/*Write determinant value to the dummy*/
void write_back(int fd, size_t tid) {
  printf("[tid:%zu]->Writing to the device...\n", tid);
  size_t order = 0;
  long long results[NUM_BULKS];
  /*Do not have to wait for terminate in the first interval*/
  while (life + 1 < LIFE) {
    //sem_wait(&write_allow_read_sems[order % OVERLAP]);
    while(__sync_bool_compare_and_swap(&write_allow_read_sems_b[order % OVERLAP], 1, 0) == false);
    size_t i;
    for (i = 0; i < NUM_BULKS; ++i) {
      results[i] = write_values[order % OVERLAP][i];
    }
    __sync_add_and_fetch(&write_allow_write_sems_b[order % OVERLAP], 1);
    //sem_post(&write_allow_write_sems[order % OVERLAP]);
    for (i = 0; i < NUM_BULKS; ++i) {
      int ret = write(fd, &results[i], sizeof(long long)); 
      if (ret < 0) {
        printf("incorrect answer %lld tid %zu\n", results[i], tid);
      } else {
        //printf("correct answer %lld tid %zu\n", results[i], tid);
      }
    }
    order = order + 1;
  }
  while (terminate == false) {
    //sem_wait(&write_allow_read_sems[order % OVERLAP]);
    while(__sync_bool_compare_and_swap(&write_allow_read_sems_b[order % OVERLAP], 1, 0) == false) {
      if (terminate == true) {
        return;
      }
    }
    size_t i;
    for (i = 0; i < NUM_BULKS; ++i) {
      results[i] = write_values[order % OVERLAP][i];
    }
    __sync_add_and_fetch(&write_allow_write_sems_b[order % OVERLAP], 1);
    //sem_post(&write_allow_write_sems[order % OVERLAP]);
    for (i = 0; i < NUM_BULKS; ++i) {
      int ret = write(fd, &results[i], sizeof(long long)); 
      if (ret < 0) {
        printf("incorrect answer %lld tid %zu\n", results[i], tid);
      } else {
        //printf("correct answer %lld tid %zu\n", results[i], tid);
      }
    }
    order = order + 1;
  }
  printf("[tid:%zu]->End writing...\n", tid);
}


int read_wrapper(int fd, int *buffer) {
  return read(fd, buffer, BUFFER_LENGTH);
}


void reader(int fd, size_t tid) {
  printf("[tid:%zu]->Reading from the device...\n", tid);
  size_t order = 0;
  size_t head = 0;
  /*Do not have to wait for terminate in the first interval*/
  while (life + 1 < LIFE) {
    size_t i;
    for (i = 0; i < OVERLAP; ++i) {
      int *buffer = (order & 0x1) == 0 ? buffer1[i] : buffer2[i];
      size_t j;
      for (j = 0; j < NUM_BULKS; ++j) {
        // Use wrapper when hpcrun is wrappered
        int ret = read_wrapper(fd, buffer + j * D_ARRAY_SIZE * D_ARRAY_SIZE);
        //int ret = read(fd, buffer, BUFFER_LENGTH);
        if (ret < 0) {
          perror("Failed to read the message from the device.");
        }
      }
    }
    for (i = 0; i < OVERLAP; ++i) {
      //sem_wait(&comp_allow_write_sems[i]);
      while (__sync_bool_compare_and_swap(&comp_allow_write_sems_b[i], 1, 0) == false);
      __sync_add_and_fetch(&comp_allow_read_sems_b[i], 1);
      //sem_post(&comp_allow_read_sems[i]);
    }
    order = order + 1;
  }
  while (terminate == false) {
    size_t i;
    for (i = 0; i < OVERLAP; ++i) {
      int *buffer = (order & 0x1) == 0 ? buffer1[i] : buffer2[i];
      size_t j;
      for (j = 0; j < NUM_BULKS; ++j) {
        // Use wrapper when hpcrun is wrappered
        int ret = read_wrapper(fd, buffer + j * D_ARRAY_SIZE * D_ARRAY_SIZE);
        //int ret = read(fd, buffer, BUFFER_LENGTH);
        if (ret < 0) {
          perror("Failed to read the message from the device.");
        }
      }
    }
    for (i = 0; i < OVERLAP; ++i) {
      //sem_wait(&comp_allow_write_sems[i]);
      while (__sync_bool_compare_and_swap(&comp_allow_write_sems_b[i], 1, 0) == false) {
        if (terminate == true) {
          return;
        }
      }
      __sync_add_and_fetch(&comp_allow_read_sems_b[i], 1);
      //sem_post(&comp_allow_read_sems[i]);
    }
    order = order + 1;
  }
  printf("[tid:%zu]->End reading...\n", tid);
}


void compute(int fd, size_t tid) {
  if (tid >= OVERLAP) {
    return;
  }
  printf("[tid:%zu]->Start computing...\n", tid);
  double *compute_buffer = (double *)malloc(sizeof(double) * NUM_BULKS * D_ARRAY_SIZE * D_ARRAY_SIZE);
  long long results[NUM_BULKS];
  size_t order = 0;
  /*Do not have to wait for terminate in the first interval*/
  while (life + 1 < LIFE) {
    //sem_wait(&comp_allow_read_sems[tid]);
    while(__sync_bool_compare_and_swap(&comp_allow_read_sems_b[tid], 1, 0) == false);
    int *buffer = (order & 0x1) == 0 ? buffer1[tid] : buffer2[tid];
    size_t i = 0;
    for (i = 0; i < NUM_BULKS * D_ARRAY_SIZE * D_ARRAY_SIZE; ++i) {
      compute_buffer[i] = buffer[i];
    }
    //sem_post(&comp_allow_write_sems[tid]);
    __sync_add_and_fetch(&comp_allow_write_sems_b[tid], 1);
    for (i = 0; i < NUM_BULKS; ++i) {
      results[i] = compute_fn(D_ARRAY_SIZE, compute_buffer + i * D_ARRAY_SIZE * D_ARRAY_SIZE);
    }

    //sem_wait(&write_allow_write_sems[tid]);
    while(__sync_bool_compare_and_swap(&write_allow_write_sems_b[tid], 1, 0) == false);
    for (i = 0; i < NUM_BULKS; ++i) {
      write_values[tid][i] = results[i];
    }
    __sync_add_and_fetch(&write_allow_read_sems_b[tid], 1);
    //sem_post(&write_allow_read_sems[tid]);

    order = order + 1;
  }
  while (terminate == false) {
    //sem_wait(&comp_allow_read_sems[tid]);
    while(__sync_bool_compare_and_swap(&comp_allow_read_sems_b[tid], 1, 0) == false) {
      if (terminate == true) {
        return;
      }
    }
    int *buffer = (order & 0x1) == 0 ? buffer1[tid] : buffer2[tid];
    size_t i = 0;
    for (i = 0; i < NUM_BULKS * D_ARRAY_SIZE * D_ARRAY_SIZE; ++i) {
      compute_buffer[i] = buffer[i];
    }
    //sem_post(&comp_allow_write_sems[tid]);
    __sync_add_and_fetch(&comp_allow_write_sems_b[tid], 1);
    long long result = compute_fn(D_ARRAY_SIZE, compute_buffer);

    //sem_wait(&write_allow_write_sems[tid]);
    while(__sync_bool_compare_and_swap(&write_allow_write_sems_b[tid], 1, 0) == false) {
      if (terminate == true) {
        return;
      }
    }
    for (i = 0; i < NUM_BULKS; ++i) {
      write_values[tid][i] = results[i];
    }
    __sync_add_and_fetch(&write_allow_read_sems_b[tid], 1);
    //sem_post(&write_allow_read_sems[tid]);

    order = order + 1;
  }
  printf("[tid:%zu]->End computing...\n", tid);
}


void init() {
  compute_fn = lookup_determinant_func("double", "simd");
  size_t i;
  for (i = 0; i < OVERLAP; ++i) {
    sem_init(&write_allow_read_sems[i], 0, 0);
    sem_init(&write_allow_write_sems[i], 0, 1);
    sem_init(&comp_allow_write_sems[i], 0, 1);
    sem_init(&comp_allow_read_sems[i], 0, 0);
    write_allow_read_sems_b[i] = 0;
    write_allow_write_sems_b[i] = 1;
    comp_allow_write_sems_b[i] = 1;
    comp_allow_read_sems_b[i] = 0;
    buffer1[i] = (int *)malloc(sizeof(int) * NUM_BULKS * D_ARRAY_SIZE * D_ARRAY_SIZE);
    buffer2[i] = (int *)malloc(sizeof(int) * NUM_BULKS * D_ARRAY_SIZE * D_ARRAY_SIZE);
  }
}


void destroy() {
  size_t i;
  for (i = 0; i < OVERLAP; ++i) {
    sem_destroy(&write_allow_read_sems[i]);
    sem_destroy(&write_allow_write_sems[i]);
    sem_destroy(&comp_allow_write_sems[i]);
    sem_destroy(&comp_allow_read_sems[i]);
    free(buffer1[i]);
    free(buffer2[i]);
  }
}


int main() {
  /*Init semaphores and buffers*/
  init();

  /*Resetting the dummy*/
  int fd;
  char c = '0';
  printf("Resetting the device \n");
  fd = open("/sys/module/dummy/parameters/no_of_reads", O_WRONLY);
  write(fd, &c, 1);
  close(fd);

  /*Open the device with read/write access*/
  printf("Starting device test code example...\n");
  fd = open("/dev/dummychar", O_RDWR);             
  if (fd < 0) {
    perror("Failed to open the device...");
    return errno;
  }

#pragma omp parallel num_threads(OVERLAP + 3)
  {
    size_t tid = omp_get_thread_num();
    /*Start computing threads*/
    if (tid < OVERLAP) {
      compute(fd, tid);
    }
    /*Create measurement thread*/
    else if (tid == OVERLAP) {
      measurement(tid);
    }
    /*Create writeback thread*/ 
    else if (tid == OVERLAP + 1) {
      write_back(fd, tid);
    }
    /*Create writeback thread*/ 
    else {
      reader(fd, tid);
    }
  }

  /*Destory queues and locks*/
  printf("End of the program\n");
  close(fd);
  /*Destroy semaphores and buffers*/
  destroy();

  return 0;
}
