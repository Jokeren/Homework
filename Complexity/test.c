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
static data_entry_t *memory_pool[NUM_POOL_SIZE];
static volatile bool memory_in_use[NUM_POOL_SIZE];


void memory_init() {
  size_t i;
  for (i = 0; i < NUM_POOL_SIZE; ++i) {
    if (memory_in_use[i] == false) {
      memory_pool[i] = (data_entry_t *)malloc(sizeof(data_entry_t));
      size_t j;
      for (j = 0; j < NUM_BULKS; ++j) {
        memory_pool[i]->data[j] = (int *)malloc(sizeof(int) * D_ARRAY_SIZE * D_ARRAY_SIZE);
      }
    }
  }
}


void memory_free() {
  size_t i;
  for (i = 0; i < NUM_POOL_SIZE; ++i) {
    if (memory_in_use[i] == false) {
      size_t j;
      for (j = 0; j < NUM_BULKS; ++j) {
        free(memory_pool[i]->data[j]);
      }
      free(memory_pool[i]);
    }
  }
}


// TODO: extend it to multiple memory segments
bool memory_fetch(size_t *pool_index) {
  while (true) {
    if (terminate == true) {
      return false;
    }
    if (*pool_index == NUM_POOL_SIZE) {
      *pool_index = 0;
    }
    if (memory_in_use[*pool_index] == false) {
      memory_in_use[*pool_index] = true;
      return true;
    }
    ++(*pool_index);
  }
}


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
  long long result[NUM_BULKS];
  while (terminate == false) {
    write_back_queue_lock(order);
    if (write_back_queue_try_get_val(order, result)) {
      size_t i;
      for (i = 0; i < NUM_BULKS; ++i) {
        //printf("write back %lld\n", result);
        int ret = write(fd, &result[i], sizeof(long long)); 
        if (ret < 0) {
          printf("incorrect answer %lld\n", result[i]);
        } else {
          //printf("correct answer %lld\n", result);
        }
      }
      write_back_queue_unlock(order);
      order = order + 1;
    } else {
      write_back_queue_unlock(order);
    }
  }
  printf("[tid:%zu]->End writing...\n", tid);
}


int __attribute__ ((noinline)) read_wrapper(int fd, int *buffer) {
  return read(fd, buffer, BUFFER_LENGTH);
}


void reader(int fd, size_t tid) {
  printf("[tid:%zu]->Reading from the device...\n", tid);
  size_t order = 1;
  size_t head = 0;
  size_t pool_index = 0;
  typedef int *buffer_t[NUM_BULKS];
  data_entry_t *data_entries[NUM_READ_ITERS];
  while (terminate == false) {
    size_t i = 0;
    for (i = 0; i < NUM_READ_ITERS; ++i) {
      if (memory_fetch(&pool_index) == false) {
        return; 
      };
      data_entries[i] = memory_pool[pool_index];
      data_entries[i]->mem_index = pool_index;
      size_t j;
      for (j = 0; j < NUM_BULKS; ++j) {
        // Use wrapper when hpcrun is wrappered
        int ret = read_wrapper(fd, data_entries[i]->data[j]);
        //int ret = read(fd, buffer, BUFFER_LENGTH);
        if (ret < 0) {
          perror("Failed to read the message from the device.");
        }
      }
    }
    bool lock = false;
    i = 0;
    while ((lock = compute_queue_try_lock(head)) == false && i < NUM_COMP_THREADS) {
      head = (head + 1) % NUM_COMP_THREADS;
      ++i;
    }
    if (lock == false) {
      compute_queue_lock(head);
    }
    for (i = 0; i < NUM_READ_ITERS; ++i) {
      data_entries[i]->tag = order;
      compute_queue_push(head, data_entries[i]);
      order = order + 1;
    }
    compute_queue_unlock(head);
    head = (head + 1) % NUM_COMP_THREADS;
  }
  printf("[tid:%zu]->End reading...\n", tid);
}


void compute(size_t tid) {
  printf("[tid:%zu]->Start computing...\n", tid);
  long long results[NUM_BULKS];
  data_entry_t *data_entry;
  double compute_buffer[D_ARRAY_SIZE * D_ARRAY_SIZE];
  while (terminate == false) {
    bool update = false;
    compute_queue_lock(tid);
    update = compute_queue_try_pop(tid, &data_entry);
    compute_queue_unlock(tid);

    if (update) {
      size_t i;
      for (i = 0; i < NUM_BULKS; ++i) {
        size_t j;
        for (j = 0; j < D_ARRAY_SIZE * D_ARRAY_SIZE; ++j) {
          compute_buffer[j] = (data_entry->data[i])[j];
        }
        results[i] = compute_fn(D_ARRAY_SIZE, compute_buffer);
      }
      memory_in_use[data_entry->mem_index] = false;
      write_back_queue_lock(data_entry->tag);
      write_back_queue_set_val(data_entry->tag, results); 
      write_back_queue_unlock(data_entry->tag);
    }
  }
  printf("[tid:%zu]->End computing...\n", tid);
}


void init() {
  compute_fn = lookup_determinant_func("double", "simd");
  write_back_queue_init();
  compute_queue_init();
  memory_init();
}


void destory() {
  write_back_queue_destory();
  compute_queue_destory();
  memory_free();
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
