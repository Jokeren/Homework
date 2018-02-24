#include "compute_queue.h"
#include <omp.h>
#include <determinant.h>
#include <stdlib.h>
#include <string.h>

struct compute_queue_entry {
  size_t tag;
  double data[D_ARRAY_SIZE * D_ARRAY_SIZE];
};

// TODO improvement by separating them from array
static compute_queue_entry_t *cq[NUM_COMP_THREADS];
static size_t cq_size[NUM_COMP_THREADS];
static size_t cq_length[NUM_COMP_THREADS];
static size_t cq_head[NUM_COMP_THREADS];
static size_t cq_tail[NUM_COMP_THREADS];
static omp_lock_t cq_locks[NUM_COMP_THREADS];
static determinant_d_fn_t compute_fn;

void compute_queue_init() {
  compute_fn = lookup_determinant_func("double", "simd");
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


void compute_queue_compute(size_t queue_id,
                           size_t bulk_size,
                           size_t data_size,
                           long long *results,
                           size_t *tags) {
  /*Queue empty*/
  if (cq_tail[queue_id] == cq_head[queue_id])
    return;

  size_t i;
  for (i = 0; i < bulk_size; ++i) {
    size_t idx = (cq_tail[queue_id] + i) % cq_size[queue_id];
    results[i] = compute_fn(data_size, cq[queue_id][idx].data);
    tags[i] = cq[queue_id][idx].tag;
    //printf("pop tag %zu\n", tags[i]);
  }
}


bool compute_queue_try_pop(size_t queue_id, size_t bulk_size) {
  if (cq_tail[queue_id] == cq_head[queue_id])
    return false;
  //size_t tail = cq_tail[queue_id];
  //printf("cq_tail[queue_id] %zu cq_head[queue_id] %zu bulk_size %zu cq_size[queue_id] %zu cq_length[queue_id] %zu\n", cq_tail[queue_id], cq_head[queue_id], bulk_size, cq_size[queue_id], cq_length[queue_id]);
  //printf("tags-before-before:\n");
  //size_t i;
  //for (i = 0; i < cq_length[queue_id]; ++i) {
  //  if (tail == cq_length[queue_id]) {
  //    tail = 0;
  //  } else {
  //    ++tail;
  //  }
  //  printf("%zu ", cq[queue_id][tail].tag);
  //}
  //printf("\n");

  size_t i;
  for (i = 0; i < bulk_size; ++i) {
    size_t idx = (cq_tail[queue_id] + i) % cq_size[queue_id];
    cq[queue_id][idx].tag = 0;
  }
  //tail = cq_tail[queue_id];
  //printf("tags-before-after:\n");
  //for (i = 0; i < cq_length[queue_id]; ++i) {
  //  if (tail == cq_length[queue_id]) {
  //    tail = 0;
  //  } else {
  //    ++tail;
  //  }
  //  printf("%zu ", cq[queue_id][tail].tag);
  //}
  //printf("\n");

  cq_tail[queue_id] = (cq_tail[queue_id] + bulk_size) % cq_size[queue_id];
  //printf("tail %zu\n", cq_tail[queue_id]);
  return true;
}


void compute_queue_push(size_t queue_id,
                        size_t tag_start,
                        size_t bulk_size,
                        size_t data_size,
                        int *receive) {
   //size_t tail = cq_tail[queue_id];
   //printf("tail %zu cq_tail[queue_id] %zu cq_head[queue_id] %zu bulk_size %zu cq_size[queue_id] %zu cq_length[queue_id] %zu\n", tail, cq_tail[queue_id], cq_head[queue_id], bulk_size, cq_size[queue_id], cq_length[queue_id]);
  if (cq_tail[queue_id] == (cq_head[queue_id] + bulk_size) % cq_size[queue_id]) {
    compute_queue_entry_t *new_queue = (compute_queue_entry_t *)calloc(
      (cq_size[queue_id] + READ_QUEUE_LENGTH), sizeof(compute_queue_entry_t));
    size_t tail = cq_tail[queue_id];
    size_t i;
    if (tail == cq_length[queue_id]) {
      tail = 0;
    }
    //printf("tags-before:\n");
    //for (i = 0; i < cq_length[queue_id]; ++i) {
    //  if (tail == cq_length[queue_id]) {
    //    tail = 0;
    //  }
    //  printf("%zu ", cq[queue_id][tail].tag);
    //  ++tail;
    //}
    //printf("\n");
    //tail = cq_tail[queue_id];
    for (i = 0; i < cq_length[queue_id]; ++i) {
      if (tail == cq_length[queue_id]) {
        tail = 0;
      }
      new_queue[i].tag = cq[queue_id][tail].tag;
      size_t j;
      for (j = 0; j < data_size; ++j) {
        new_queue[i].data[j] = cq[queue_id][tail].data[j];
      }
      tail = tail + 1;
    }
    tail = cq_tail[queue_id];
    free(cq[queue_id]);
    cq[queue_id] = new_queue;
    cq_tail[queue_id] = 0;
    cq_head[queue_id] = cq_length[queue_id];
    cq_size[queue_id] = cq_size[queue_id] + READ_QUEUE_LENGTH;
    cq_length[queue_id] = cq_length[queue_id] + READ_QUEUE_LENGTH;
  }

  size_t i;
  size_t head_idx = cq_head[queue_id];
  //tail = cq_tail[queue_id];
  //printf("tags-after:\n");
  //for (i = 0; i < cq_length[queue_id]; ++i) {
  //  if (tail == cq_length[queue_id]) {
  //    tail = 0;
  //  }
  //  printf("%zu ", cq[queue_id][tail].tag);
  //  ++tail;
  //}
  //printf("\n");
  for (i = 0; i < bulk_size; ++i) {
    cq[queue_id][(head_idx + i) % cq_size[queue_id]].tag = tag_start + i;
    double *data = cq[queue_id][(head_idx + i) % cq_size[queue_id]].data;
    int *buffer = receive + i * data_size;
    size_t j;
    for (j = 0; j < data_size; ++j) {
      data[j] = buffer[j];
    }
  }
  
  cq_head[queue_id] = (cq_head[queue_id] + bulk_size) % cq_size[queue_id];
}
