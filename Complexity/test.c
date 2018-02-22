#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <omp.h>

#include "compute_queue.h"
#include "write_back_queue.h"

static volatile bool terminate = false;
static int receive[NUM_BULKS * BUFFER_LENGTH];
static double comp[WRITE_QUEUE_LENGTH * D_ARRAY_SIZE * D_ARRAY_SIZE];

void measurement() {
  FILE *fp = fopen("result.output", "w");
  printf("Open output buffer\n");
  if (!fp) {
    perror("Failed to open the output file...");
    return;
  }

  size_t i;
  int fd;
  char d[64];
  for (i = 0; i < LIFE; ++i) {
    sleep(10);
    /*Reading number of determinant calculations*/
    printf("Write out buffer\n");
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
    fprintf(fp, "%d\n", num);
  }

  terminate = true;

  close(fd);
  fclose(fp);
}


/*Write determinant value to the dummy*/
void write_back(int fd) {
  printf("Writing to the device...\n");
  size_t head = 0;
  while (terminate == false) {
    /*Unavail is only set in the write back thread*/
    if (write_back_queue_avail(head)) {
      write_back_queue_lock(head);
      long long result = write_back_queue_get_val(head);
      int ret = write(fd, &result, sizeof(long long)); 
      if (ret < 0) {
        printf("incorrect answer\n");
      }
      write_back_queue_set_unavail(head);
      write_back_queue_unlock(head);
      head = (head + 1) % WRITE_QUEUE_LENGTH;
    }
  }
}


void reader(int fd) {
  printf("Reading from the device...\n");
  size_t order = 0;
  size_t head = 0;
  while (terminate == false) {
    size_t i;
    for (i = 0; i < NUM_BULKS; ++i) {
      int ret = read(fd, receive + i * D_ARRAY_SIZE * D_ARRAY_SIZE, BUFFER_LENGTH);
      if (ret < 0) {
        perror("Failed to read the message from the device.");
      }
    }
    compute_queue_lock(head);
    while (compute_queue_try_push(head, order,
      NUM_BULKS, D_ARRAY_SIZE * D_ARRAY_SIZE, receive) == false) {
      head = (head + 1) % NUM_COMP_THREADS;
    }
    compute_queue_unlock(head);
    head = (head + 1) % NUM_COMP_THREADS;
    order = (order + NUM_BULKS) % WRITE_QUEUE_LENGTH;
  }
}


void compute(size_t tid) {
  printf("%zu: Start computing...\n", tid);
  long long results[NUM_BULKS];
  size_t tags[NUM_BULKS];
  while (true) {
    bool update = false;
    if (compute_queue_try_lock(tid)) {
      compute_queue_compute(tid, NUM_BULKS, D_ARRAY_SIZE, results, tags);
      update = compute_queue_try_pop(tid, NUM_BULKS);
      compute_queue_unlock(tid);
      if (update) {
        size_t i;
        for (i = 0; i < NUM_BULKS; ++i) {
          write_back_queue_lock(tags[i]);
          write_back_queue_set_val(tags[i], results[i]); 
          write_back_queue_set_avail(tags[i]);
          write_back_queue_unlock(tags[i]);
        }
      }
    }
  }
}


void init() {
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
      measurement();
    }
    /*Create writeback thread*/ 
    else if (tid == NUM_COMP_THREADS + 1) {
      write_back(fd);
    }
    /* Read matrices from the device*/
    else {
      reader(fd);
    }
  }


  /*Destory queues and locks*/
  printf("End of the program\n");
  close(fd);
  destory();

  return 0;
}
