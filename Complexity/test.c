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
  printf("[tid:%zu]->Writing to the device...\n", tid);
  size_t order = 1;
  while (terminate == false) {
    write_back_queue_lock(order);
    long long result = 0;
    if (write_back_queue_try_get_val(order, &result)) {
      //printf("write back %lld\n", result);
      int ret = write(fd, &result, sizeof(long long)); 
      if (ret < 0) {
        printf("incorrect answer %lld\n", result);
      } else {
        //printf("correct answer %lld\n", result);
      }
      write_back_queue_unlock(order);
      order = order + 1;
    } else {
      write_back_queue_unlock(order);
    }
  }
  printf("[tid:%zu]->End writing...\n", tid);
}


void reader(int fd, size_t tid) {
  printf("[tid:%zu]->Reading from the device...\n", tid);
  size_t order = 1;
  size_t head = 0;
  int int_buffer[D_ARRAY_SIZE * D_ARRAY_SIZE];
  while (terminate == false) {
    size_t i = 0;
    typedef double *buffer_t[NUM_BULKS];
    buffer_t receive[NUM_READ_ITERS];
    for (i = 0; i < NUM_READ_ITERS; ++i) {
      size_t j;
      for (j = 0; j < NUM_BULKS; ++j) {
        int ret = read(fd, int_buffer, BUFFER_LENGTH);
        if (ret < 0) {
          perror("Failed to read the message from the device.");
        }
        receive[i][j] = (double *)malloc(sizeof(double) * D_ARRAY_SIZE * D_ARRAY_SIZE);
        size_t k;
        for (k = 0; k < D_ARRAY_SIZE * D_ARRAY_SIZE; ++k) {
          receive[i][j][k] = int_buffer[k];
        }
      }
    }
    //bool lock = false;
    //i = 0;
    //while ((lock = compute_queue_try_lock(head)) == false && i < NUM_COMP_THREADS) {
    //  head = (head + 1) % NUM_COMP_THREADS;
    //  ++i;
    //}
    //if (lock == false) {
    //  compute_queue_lock(head);
    //}
    compute_queue_lock(head);
    for (i = 0; i < NUM_READ_ITERS; ++i) {
      compute_queue_push(head, order, NUM_BULKS, receive[i]);
      order = order + NUM_BULKS;
    }
    compute_queue_unlock(head);
    head = (head + 1) % NUM_COMP_THREADS;
  }
  printf("[tid:%zu]->End reading...\n", tid);
}


void compute(size_t tid) {
  printf("[tid:%zu]->Start computing...\n", tid);
  long long results[NUM_BULKS];
  size_t tags[NUM_BULKS];
  double *data[NUM_BULKS];
  while (terminate == false) {
    bool update = false;
    compute_queue_lock(tid);
    update = compute_queue_try_pop(tid, NUM_BULKS, data, tags);
    compute_queue_unlock(tid);

    if (update) {
      size_t i;
      for (i = 0; i < NUM_BULKS; ++i) {
        results[i] = compute_fn(D_ARRAY_SIZE, data[i]);
      }
      for (i = 0; i < NUM_BULKS; ++i) {
        write_back_queue_lock(tags[i]);
        //printf("write tag %zu\n", tags[i]);
        //printf("tags %zu\n", tags[i]);
        write_back_queue_set_val(tags[i], results[i]); 
        write_back_queue_unlock(tags[i]);
      }
      for (i = 0; i < NUM_BULKS; ++i) {
        free(data[i]);
      }
    }
  }
  printf("[tid:%zu]->End computing...\n", tid);
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

#pragma omp parallel num_threads(NUM_COMP_THREADS + 3)
  {
    size_t tid = omp_get_thread_num();
    /*Start computing threads*/
    if (tid < NUM_COMP_THREADS) {
      compute(tid);
    }
    /*Create measurement thread*/
    else if (tid == NUM_COMP_THREADS) {
      measurement(tid);
    }
    /*Create writeback thread*/ 
    else if (tid == NUM_COMP_THREADS + 1) {
      write_back(fd, tid);
    }
    /*Read matrices from the device*/
    else {
      reader(fd, tid);
    }
  }


  /*Destory queues and locks*/
  printf("End of the program\n");
  close(fd);
  destory();

  return 0;
}
