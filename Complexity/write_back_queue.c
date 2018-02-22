#include "write_back_queue.h"
#include <omp.h>

struct write_back_entry {
  volatile bool avail;
  long long val;
};

static write_back_entry_t wq[WRITE_QUEUE_LENGTH];
static omp_lock_t wq_locks[WRITE_QUEUE_LENGTH];


void write_back_queue_init() {
  size_t i;
  for (i = 0; i < WRITE_QUEUE_LENGTH; ++i) {
    omp_init_lock(&wq_locks[i]);
    wq[i].avail = false;
  }
}


void write_back_queue_destory() {
  size_t i;
  for (i = 0; i < WRITE_QUEUE_LENGTH; ++i) {
    omp_destroy_lock(&wq_locks[i]);
  }
}


bool write_back_queue_avail(size_t index) {
  return wq[index].avail; 
}


void write_back_queue_lock(size_t index) {
  omp_set_lock(&wq_locks[index]);
}


void write_back_queue_unlock(size_t index) {
  omp_unset_lock(&wq_locks[index]);
}


void write_back_queue_set_avail(size_t index) {
  wq[index].avail = true;
}


void write_back_queue_set_unavail(size_t index) {
  wq[index].avail = false;
}


long long write_back_queue_get_val(size_t index) {
  return wq[index].val;
}


void write_back_queue_set_val(size_t index, long long val) {
  wq[index].val = val;
}
