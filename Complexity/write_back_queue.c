#include "write_back_queue.h"
#include <omp.h>
#include <stdlib.h>
#include <string.h>

struct write_back_entry {
  long long vals[NUM_BULKS];
  size_t tag;
};

static write_back_entry_t *wq[WRITE_QUEUE_LENGTH];
static omp_lock_t wq_locks[WRITE_QUEUE_LENGTH];
static size_t wq_size[WRITE_QUEUE_LENGTH];


void write_back_queue_init() {
  size_t i;
  for (i = 0; i < WRITE_QUEUE_LENGTH; ++i) {
    omp_init_lock(&wq_locks[i]);
    wq[i] = (write_back_entry_t *)calloc(NUM_WRITE_ENTRIES, sizeof(write_back_entry_t));
    wq_size[i] = NUM_WRITE_ENTRIES;
  }
}


void write_back_queue_destory() {
  size_t i;
  for (i = 0; i < WRITE_QUEUE_LENGTH; ++i) {
    omp_destroy_lock(&wq_locks[i]);
    free(wq[i]);
  }
}


void write_back_queue_lock(size_t index) {
  //printf("lock %zu\n", (index - 1) % WRITE_QUEUE_LENGTH);
  omp_set_lock(&wq_locks[(index - 1) % WRITE_QUEUE_LENGTH]);
}


void write_back_queue_unlock(size_t index) {
  //printf("unlock %zu\n", (index - 1) % WRITE_QUEUE_LENGTH);
  omp_unset_lock(&wq_locks[(index - 1) % WRITE_QUEUE_LENGTH]);
}


void write_back_queue_set_val(size_t index, long long vals[NUM_BULKS]) {
  size_t queue_id = (index - 1) % WRITE_QUEUE_LENGTH;
  size_t i;
  for (i = 0; i < wq_size[queue_id]; ++i) {
    if (wq[queue_id][i].tag == 0) {
      size_t j;
      for (j = 0; j < NUM_BULKS; ++j) {
        wq[queue_id][i].vals[j] = vals[j];
      }
      wq[queue_id][i].tag = index;
      return;
    }
  }
  if (i == wq_size[queue_id]) {
    write_back_entry_t *new_queue = (write_back_entry_t *)calloc(
        (wq_size[queue_id] + NUM_WRITE_ENTRIES), sizeof(write_back_entry_t));
    size_t j;
    for (j = 0; j < wq_size[queue_id]; ++j) {
      new_queue[j].tag = wq[queue_id][j].tag;
      size_t k;
      for (k = 0; k < NUM_BULKS; ++k) {
        new_queue[j].vals[k] = wq[queue_id][j].vals[k];
      }
    }
    free(wq[queue_id]);
    wq[queue_id] = new_queue;
    wq[queue_id][wq_size[queue_id]].tag = index;
    for (j = 0; j < NUM_BULKS; ++j) {
      wq[queue_id][wq_size[queue_id]].vals[j] = vals[j];
    }
    wq_size[queue_id] = wq_size[queue_id] + NUM_WRITE_ENTRIES;
  }
}


bool write_back_queue_try_get_val(size_t index, long long vals[NUM_BULKS]) {
  size_t queue_id = (index - 1) % WRITE_QUEUE_LENGTH;
  size_t i;
  for (i = 0; i < wq_size[queue_id]; ++i) {
    if (wq[queue_id][i].tag == index) {
      wq[queue_id][i].tag = 0;
      size_t j;
      for (j = 0; j < NUM_BULKS; ++j) {
        vals[j] = wq[queue_id][i].vals[j];
      }
      return true;
    }
  }
  return false;
}


void write_back_queue_display() {
  size_t queue_id;
  for (queue_id = 0; queue_id < WRITE_QUEUE_LENGTH; ++queue_id) {
    printf("queue %zu\n", queue_id);
    size_t i;
    for (i = 0; i < wq_size[queue_id]; ++i) {
      size_t k;
      for (k = 0; k < NUM_BULKS; ++k) {
        printf("tag %zu val %lld\n", wq[queue_id][i].tag, wq[queue_id][i].vals[k]);
      }
    }
  }
  printf("\n");
}
