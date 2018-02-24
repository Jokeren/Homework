#include "compute_queue.h"
#include <omp.h>
#include <stdlib.h>
#include <string.h>

struct compute_queue_entry {
  size_t tag;
  size_t mem_index;
  int *data;
};

__thread long long msize = 0;

// TODO improvement by separating them from array
static compute_queue_entry_t *cq[NUM_COMP_THREADS];
static size_t cq_size[NUM_COMP_THREADS];
static size_t cq_length[NUM_COMP_THREADS];
static size_t cq_head[NUM_COMP_THREADS];
static size_t cq_tail[NUM_COMP_THREADS];
static omp_lock_t cq_locks[NUM_COMP_THREADS];

void compute_queue_init() {
  size_t i;
  for (i = 0; i < NUM_COMP_THREADS; ++i) {
    omp_init_lock(&cq_locks[i]);
    cq_head[i] = 0;
    cq_tail[i] = 0;
    cq_size[i] = READ_QUEUE_SIZE;
    cq_length[i] = READ_QUEUE_LENGTH;
    cq[i] = (compute_queue_entry_t *)calloc(cq_size[i], sizeof(compute_queue_entry_t));
  }
}


void compute_queue_destory() {
  size_t i;
  for (i = 0; i < NUM_COMP_THREADS; ++i) {
    omp_destroy_lock(&cq_locks[i]);
    if (cq[i]->data != NULL) {
      free(cq[i]->data);
    }
    free(cq[i]);
  }
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


bool compute_queue_try_pop(size_t queue_id,
                           size_t bulk_size,
                           int *data[NUM_BULKS],
                           size_t tags[NUM_BULKS],
                           size_t mem_index[NUM_BULKS]) {
  if (cq_tail[queue_id] == cq_head[queue_id]) {
    return false;
  }

  size_t i;
  size_t idx = cq_tail[queue_id];
  for (i = 0; i < bulk_size; ++i) {
    if (idx == cq_size[queue_id]) {
      idx = 0;
    }
    data[i] = cq[queue_id][idx].data;
    tags[i] = cq[queue_id][idx].tag;
    mem_index[i] = cq[queue_id][idx].mem_index;
    cq[queue_id][idx].data = NULL;
    cq[queue_id][idx].tag = 0;
    idx = idx + 1;
  }

  //--msize;
  cq_tail[queue_id] = (cq_tail[queue_id] + bulk_size) % cq_size[queue_id];
  return true;
}


static void compute_queue_resize(size_t queue_id,
                                 size_t bulk_size) {
  compute_queue_entry_t *new_queue = (compute_queue_entry_t *)calloc(
      (cq_length[queue_id] * 2 + NUM_BULKS), sizeof(compute_queue_entry_t));
  size_t tail = cq_tail[queue_id];
  size_t i;
  for (i = 0; i < cq_length[queue_id]; ++i) {
    if (tail == cq_size[queue_id]) {
      tail = 0;
    }
    new_queue[i].tag = cq[queue_id][tail].tag;
    new_queue[i].data = cq[queue_id][tail].data;
    new_queue[i].mem_index = cq[queue_id][tail].mem_index;
    tail = tail + 1;
  }
  tail = cq_tail[queue_id];
  free(cq[queue_id]);
  cq[queue_id] = new_queue;
  cq_tail[queue_id] = 0;
  cq_head[queue_id] = cq_length[queue_id];
  cq_size[queue_id] = cq_length[queue_id] * 2 + NUM_BULKS;
  cq_length[queue_id] = cq_length[queue_id] * 2;
}


void compute_queue_push(size_t queue_id,
                        size_t tag_start,
                        size_t bulk_size,
                        int *receive[NUM_BULKS],
                        size_t receive_index[NUM_BULKS]) {
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
  if (cq_tail[queue_id] == (cq_head[queue_id] + bulk_size) % cq_size[queue_id]) {
    compute_queue_resize(queue_id, bulk_size);
  }

  size_t i;
  size_t head_idx = cq_head[queue_id];
  for (i = 0; i < bulk_size; ++i) {
    if (head_idx == cq_size[queue_id]) {
      head_idx = 0;
    }
    cq[queue_id][head_idx].tag = tag_start + i;
    //printf("push %zu\n", cq[queue_id][head_idx].tag);
    cq[queue_id][head_idx].data = receive[i];
    cq[queue_id][head_idx].mem_index = receive_index[i];
    head_idx = head_idx + 1;
  }

  cq_head[queue_id] = (cq_head[queue_id] + bulk_size) % cq_size[queue_id];
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
