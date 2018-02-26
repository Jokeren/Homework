#include "compute_queue.h"
#include <omp.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "common.h"

__thread long long msize = 0;

// TODO improvement by separating them from array
typedef data_entry_t *data_entry_pointer_t;
static data_entry_pointer_t *cq[NUM_COMP_THREADS];
static size_t cq_size[NUM_COMP_THREADS];
static size_t cq_length[NUM_COMP_THREADS];
static size_t cq_head[NUM_COMP_THREADS];
static size_t cq_tail[NUM_COMP_THREADS];
static omp_lock_t cq_locks[NUM_COMP_THREADS];
static sem_t cq_sems[NUM_COMP_THREADS];

void compute_queue_init() {
  size_t i;
  for (i = 0; i < NUM_COMP_THREADS; ++i) {
    omp_init_lock(&cq_locks[i]);
    sem_init(&cq_sems[i], 0, 0);
    cq_head[i] = 0;
    cq_tail[i] = 0;
    cq_size[i] = READ_QUEUE_SIZE;
    cq_length[i] = READ_QUEUE_LENGTH;
    cq[i] = (data_entry_pointer_t *)calloc(cq_size[i], sizeof(data_entry_pointer_t));
  }
}


void compute_queue_destory() {
  size_t i;
  for (i = 0; i < NUM_COMP_THREADS; ++i) {
    omp_destroy_lock(&cq_locks[i]);
    sem_destroy(&cq_sems[i]);
    free(cq[i]);
  }
}


void compute_queue_wait(size_t queue_id) {
  sem_wait(&cq_sems[queue_id]);
}


void compute_queue_signal(size_t queue_id) {
  sem_post(&cq_sems[queue_id]);
}


bool compute_queue_try_lock(size_t queue_id) {
  return omp_test_lock(&cq_locks[queue_id]);
}


void compute_queue_lock(size_t queue_id) {
  omp_set_lock(&cq_locks[queue_id]);
}


void compute_queue_unlock(size_t queue_id) {
  omp_unset_lock(&cq_locks[queue_id]);
}


bool compute_queue_try_pop(size_t queue_id, data_entry_t *data_entry) {
  if (cq_tail[queue_id] == cq_head[queue_id]) {
    return false;
  }

  size_t idx = cq_tail[queue_id];
  data_entry = cq[queue_id][idx];

  //--msize;
  cq_tail[queue_id] = (cq_tail[queue_id] + 1) % cq_size[queue_id];
  return true;
}


static void compute_queue_resize(size_t queue_id) {
  data_entry_pointer_t *new_queue = (data_entry_pointer_t *)calloc(
      (cq_length[queue_id] * 2 + 1), sizeof(data_entry_pointer_t));
  size_t tail = cq_tail[queue_id];
  size_t i;
  for (i = 0; i < cq_length[queue_id]; ++i) {
    if (tail == cq_size[queue_id]) {
      tail = 0;
    }
    new_queue[i] = cq[queue_id][tail];
    tail = tail + 1;
  }
  tail = cq_tail[queue_id];
  free(cq[queue_id]);
  cq[queue_id] = new_queue;
  cq_tail[queue_id] = 0;
  cq_head[queue_id] = cq_length[queue_id];
  cq_size[queue_id] = cq_length[queue_id] * 2 + 1;
  cq_length[queue_id] = cq_length[queue_id] * 2;
}


void compute_queue_push(size_t queue_id, data_entry_t *data_entry) {
  //size_t tail = cq_tail[queue_id];
  //printf("before\n");
  //printf("tail %zu head %zu tag_start %zu\n", tail, cq_head[queue_id], tag_start);
  //size_t i;
  //for (i = 0; i < cq_length[queue_id]; ++i) {
  //  if (tail == cq_size[queue_id]) {
  //    tail = 0;
  //  }
  //  printf("%zu ", cq[queue_id][tail].tag);
  //  tail = tail + 1;
  //}
  //printf("\n");
  if (cq_tail[queue_id] == (cq_head[queue_id] + 1) % cq_size[queue_id]) {
    compute_queue_resize(queue_id);
  }

  size_t head_idx = cq_head[queue_id];
  cq[queue_id][head_idx] = data_entry;

  cq_head[queue_id] = (cq_head[queue_id] + 1) % cq_size[queue_id];
  //++msize;
  //printf("resize %zu\n", msize);
  //printf("afater\n");
  //printf("tail %zu head %zu tag_start %zu\n", tail, cq_head[queue_id], tag_start);
  ////size_t i;
  //tail = cq_tail[queue_id];
  //for (i = 0; i < cq_length[queue_id]; ++i) {
  //  if (tail == cq_size[queue_id]) {
  //    tail = 0;
  //  }
  //  printf("%zu ", cq[queue_id][tail].tag);
  //  tail = tail + 1;
  //}
  //printf("\n");
}
