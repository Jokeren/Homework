#include "compute_queue.h"
#include <omp.h>
#include <determinant.h>

struct compute_queue_entry {
  size_t tag;
  double data[D_ARRAY_SIZE * D_ARRAY_SIZE];
};

// TODO improvement by separating them from array
static compute_queue_entry_t cq[NUM_COMP_THREADS][READ_QUEUE_LENGTH];
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
  }
}


void compute_queue_destory() {
  size_t i;
  for (i = 0; i < NUM_COMP_THREADS; ++i) {
    omp_destroy_lock(&cq_locks[i]);
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
    size_t idx = cq_tail[queue_id] - bulk_size + i;
    results[i] = compute_fn(data_size, cq[queue_id][idx].data);
    tags[i] = cq[queue_id][idx].tag;
  }
}


bool compute_queue_try_pop(size_t queue_id, size_t bulk_size) {
  if (cq_tail[queue_id] == cq_head[queue_id])
    return false;

  cq_tail[queue_id] = (cq_tail[queue_id] + bulk_size) % READ_QUEUE_LENGTH;
  return true;
}


bool compute_queue_try_push(size_t queue_id,
                            size_t tag_start,
                            size_t bulk_size,
                            size_t data_size,
                            int *buffer) {
  if (cq_tail[queue_id] == (cq_head[queue_id] + bulk_size) % READ_QUEUE_LENGTH)
    return false;

  size_t i;
  size_t head_idx = cq_head[queue_id];
  for (i = 0; i < bulk_size; ++i) {
    cq[queue_id][head_idx + i].tag = tag_start + i;
    double *data = cq[queue_id][head_idx + i].data;
    size_t j;
    for (j = 0; j < data_size; ++j) {
      data[j] = buffer[j];
    }
  }
  
  cq_head[queue_id] = (cq_head[queue_id] + bulk_size) % READ_QUEUE_LENGTH;
  return true;
}
