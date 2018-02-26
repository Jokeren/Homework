#include "compute_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static double *data[NUM_BULKS];

int main() {
  compute_queue_init();
  size_t i;
  size_t num = 1000;
  for (i = 0; i < num / NUM_BULKS; ++i) {
    double *receive[NUM_BULKS];
    size_t j;
    for (j = 0; j < NUM_BULKS; ++j) {
      receive[j] = (double *)malloc(sizeof(double) * D_ARRAY_SIZE * D_ARRAY_SIZE);
    }
    compute_queue_push(0,
                       i * NUM_BULKS,
                       NUM_BULKS,
                       receive);
  }
  for (i = 0; i < num / NUM_BULKS; ++i) {
    size_t tags[NUM_BULKS];
    if (compute_queue_try_pop(0, NUM_BULKS, data, tags) == false) {
      printf("error!\n");
    }
    size_t j;
    for (j = 0; j < NUM_BULKS; ++j) {
      free(data[j]);
    }
  }
  compute_queue_destory();
}

