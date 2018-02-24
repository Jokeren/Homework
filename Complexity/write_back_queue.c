#include "write_back_queue.h"
#include <omp.h>
#include <stdlib.h>
#include <string.h>

struct write_back_entry {
  long long val;
  size_t tag;
};

static write_back_entry_t *wq[WRITE_QUEUE_LENGTH];
static omp_lock_t wq_locks[WRITE_QUEUE_LENGTH];
static size_t wq_size[WRITE_QUEUE_LENGTH];


void write_back_queue_init() {
  size_t i;
  for (i = 0; i < WRITE_QUEUE_LENGTH; ++i) {
    omp_init_lock(&wq_locks[i]);
    wq[i] = (write_back_entry_t *)calloc(WRITE_QUEUE_ENTRIES, sizeof(write_back_entry_t));
    wq_size[i] = WRITE_QUEUE_ENTRIES;
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


void write_back_queue_set_val(size_t index, long long val) {
  size_t queue_id = (index - 1) % WRITE_QUEUE_LENGTH;
  size_t i;
  for (i = 0; i < wq_size[queue_id]; ++i) {
    if (wq[queue_id][i].tag == 0) {
      wq[queue_id][i].val = val;
      wq[queue_id][i].tag = index;
      return;
    }
  }
  if (i == wq_size[queue_id]) {
    write_back_entry_t *new_queue = (write_back_entry_t *)calloc(
      (wq_size[queue_id] + WRITE_QUEUE_ENTRIES), sizeof(write_back_entry_t));
    memcpy(new_queue, wq[queue_id], wq_size[queue_id] * sizeof(write_back_entry_t));
    free(wq[queue_id]);
    wq[queue_id] = new_queue;
    wq[queue_id][wq_size[queue_id]].val = val;
    wq[queue_id][wq_size[queue_id]].tag = index;
    wq_size[queue_id] = wq_size[queue_id] + WRITE_QUEUE_ENTRIES;
  }
}


long long write_back_queue_get_val(size_t index, bool *get_value) {
  size_t queue_id = (index - 1) % WRITE_QUEUE_LENGTH;
  size_t i;
  for (i = 0; i < wq_size[queue_id]; ++i) {
    if (wq[queue_id][i].tag == index) {
      wq[queue_id][i].tag = 0;
      *get_value = true;
      return wq[queue_id][i].val;
    }
  }
  *get_value = false;
  return 0;
}


void write_back_queue_display() {
  size_t queue_id;
  for (queue_id = 0; queue_id < WRITE_QUEUE_LENGTH; ++queue_id) {
    printf("queue %zu\n", queue_id);
    size_t i;
    for (i = 0; i < wq_size[queue_id]; ++i) {
      printf("tag %zu val %lld\n", wq[queue_id][i].tag, wq[queue_id][i].val);
    }
  }
  printf("\n");
}
