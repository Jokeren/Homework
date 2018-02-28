#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <omp.h>
#include <sys/time.h>
#include <determinant.h>

#include "common.h"

#define CPU_TIMER_START(elapsed_time, t1) \
  do { \
    elapsed_time = 0.0; \
    gettimeofday(&t1, NULL); \
  } while (0)

#define CPU_TIMER_END(elapsed_time, t1, t2) \
  do { \
    gettimeofday(&t2, NULL); \
    elapsed_time = (t2.tv_sec - t1.tv_sec) * 1000.0; \
    elapsed_time += (t2.tv_usec - t1.tv_usec) / 1000.0; \
    elapsed_time /= 1000.0; \
  } while (0)

#define TIMES 100000

static determinant_d_fn_t compute_fn;

void handle(int *data) {
}

int main() {
  /*Resetting the dummy*/
  int fd;
  char c = '0';
  printf("Resetting the device \n");
  fd = open("/sys/module/dummy/parameters/no_of_reads", O_WRONLY);
  write(fd, &c, 1);
  close(fd);

  double elapsed = 0.0;
  struct timeval begin;
  struct timeval end;
  /*Open the device with read/write access*/
  printf("Starting device test code example...\n");
  fd = open("/dev/dummychar", O_RDWR);             
  if (fd < 0) {
    perror("Failed to open the device...");
    return errno;
  }

  int *data = (int *)malloc(D_ARRAY_SIZE * D_ARRAY_SIZE * sizeof(int));

  CPU_TIMER_START(elapsed, begin);
  size_t i;
  for (i = 0; i < TIMES; ++i) {
    int ret = read(fd, data, sizeof(int) * D_ARRAY_SIZE * D_ARRAY_SIZE);
  }
  CPU_TIMER_END(elapsed, begin, end);
  printf("%f\n", elapsed);

  compute_fn = lookup_determinant_func("double", "simd");
  double *data_f = (double *)malloc(D_ARRAY_SIZE * D_ARRAY_SIZE * sizeof(double));
  CPU_TIMER_START(elapsed, begin);
  for (i = 0; i < TIMES; ++i) {
    size_t j = 0;
    for (j = 0; j < D_ARRAY_SIZE * D_ARRAY_SIZE; ++j) {
      data_f[j] = data[j];
    }
    compute_fn(D_ARRAY_SIZE, data_f);
  }
  CPU_TIMER_END(elapsed, begin, end);
  printf("%f\n", elapsed);

  /*Destory queues and locks*/
  printf("End of the program\n");
  close(fd);
  return 0;
}
