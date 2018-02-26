#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <omp.h>
#include <determinant.h>

#include "common.h"
#include "compute_queue.h"
#include "write_back_queue.h"

static volatile bool terminate = false;
static determinant_d_fn_t compute_fn;
static int int_buffer[D_ARRAY_SIZE * D_ARRAY_SIZE];

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
    printf("[tid:%zu]->Throughput %d\n", tid, num);
    fprintf(fp, "%d\n", num);
  }

  terminate = true;

  close(fd);
  fclose(fp);
  printf("[tid:%zu]->End measurement...\n", tid);
}


/*Write determinant value to the dummy*/
void write_back(int fd, size_t tid) {
  //printf("[tid:%zu]->Writing to the device...\n", tid);
  size_t order = 1;
  /*Unavail is only set in the write back thread*/
  size_t i;
  bool get_val = false;
  for (i = 0; i < NUM_BULKS; ++i) {
    long long result;
    write_back_queue_try_get_val(order, &result);
    int ret = write(fd, &result, sizeof(long long)); 
    if (ret < 0) {
      printf("incorrect answer %lld\n", result);
    }
    order = order + 1;
  }
  //printf("[tid:%zu]->End writing...\n", tid);
}


void reader(int fd, size_t tid) {
  //printf("[tid:%zu]->Reading from the device...\n", tid);
  size_t order = 1;
  size_t head = 0;
  double *buffer[NUM_BULKS];
  size_t i;
  for (i = 0; i < NUM_BULKS; ++i) {
    buffer[i] = (double *)malloc(sizeof(double) * D_ARRAY_SIZE * D_ARRAY_SIZE);
    int ret = read(fd, int_buffer, BUFFER_LENGTH);
    if (ret < 0) {
      perror("Failed to read the message from the device.");
    }
    size_t k;
    for (k = 0; k < D_ARRAY_SIZE * D_ARRAY_SIZE; ++k) {
      buffer[i][k] = int_buffer[k];
    }
  }
  compute_queue_push(head, order, NUM_BULKS, buffer);
  head = (head + 1) % NUM_COMP_THREADS;
  order = order + NUM_BULKS;
  //printf("[tid:%zu]->End reading...\n", tid);
}


void compute(size_t tid) {
  //printf("[tid:%zu]->Start computing...\n", tid);
  long long results[NUM_BULKS];
  size_t tags[NUM_BULKS];
  double *data[NUM_BULKS];
  compute_queue_try_pop(tid, NUM_BULKS, data, tags);
  size_t i = 0;
  for (i = 0; i < NUM_BULKS; ++i) {
    results[i] = compute_fn(D_ARRAY_SIZE, data[i]);
    write_back_queue_set_val(tags[i], results[i]); 
  }
  for (i = 0; i < NUM_BULKS; ++i) {
    free(data[i]);
  }
  //printf("[tid:%zu]->End computing...\n", tid);
}


void init() {
  compute_fn = lookup_determinant_func("double", "simd");
  write_back_queue_init();
  compute_queue_init();
}


void destory() {
  write_back_queue_destory();
  compute_queue_destory();
}


int main() {
  /*Init queues and locks*/
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

  #pragma omp parallel num_threads(2)
  {
    size_t tid = omp_get_thread_num();
    /*Start computing threads*/
    /*Create writeback thread*/ 
    /* Read matrices from the device*/
    if (tid < NUM_COMP_THREADS) {
      while (terminate == false) {
        reader(fd, tid);
        compute(tid);
        write_back(fd, tid);
      }
    }
    /*Create measurement thread*/
    else {
      measurement(tid);
    }
  }


  /*Destory queues and locks*/
  printf("End of the program\n");
  close(fd);
  destory();

  return 0;
}
