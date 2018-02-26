#include "lf_compute_queue.h"
#include <stdlib.h>
#include <string.h>
#include "queues/queue.h"
//#include "queues/delay.h"
#include "queues/primitives.h"
#include "queues/align.h"
//#include "queues/hzdptr.h"

static queue_t *cq[NUM_COMP_THREADS];
static handle_t *cq_handle[NUM_COMP_THREADS];

void queue_init(queue_t * q, int nprocs)
{
  node_t * node = malloc(sizeof(node_t));
  node->next = NULL;

  q->head = node;
  q->tail = node;
  q->nprocs = nprocs;
}

void queue_register(queue_t * q, handle_t * th, int id)
{
//  hzdptr_init(&th->hzd, q->nprocs, 2);
}

void enqueue(queue_t * q, handle_t * handle, void * data)
{
  node_t * node = malloc(sizeof(node_t));

  node->data = data;
  node->next = NULL;

  node_t * tail;
  node_t * next;

  while (1) {
//    tail = hzdptr_setv(&q->tail, &handle->hzd, 0);
    tail = q->tail;
    next = tail->next;

    if (tail != q->tail) {
      continue;
    }

    if (next != NULL) {
      CAS(&q->tail, &tail, next);
      continue;
    }

    if (CAS(&tail->next, &next, node)) break;
  }

  CAS(&q->tail, &tail, node);
}

void * dequeue(queue_t * q, handle_t * handle)
{
  void * data;

  node_t * head;
  node_t * tail;
  node_t * next;

  while (1) {
//    head = hzdptr_setv(&q->head, &handle->hzd, 0);
    head = q->head;
    tail = q->tail;
//   next = hzdptr_set(&head->next, &handle->hzd, 1);
    next = head->next;

    if (head != q->head) {
      continue;
    }

    if (next == NULL) {
      return (void *) -1;
    }

    if (head == tail) {
      CAS(&q->tail, &tail, next);
      continue;
    }

    data = next->data;
    if (CAS(&q->head, &head, next)) break;
  }

//  hzdptr_retire(&handle->hzd, head);
  return data;
}

void queue_free(queue_t *queue, handle_t *handle) {}

void compute_queue_init() {
  size_t i;
  for (i = 0; i < NUM_COMP_THREADS; ++i) {
    queue_init(cq[i], 2);
    queue_register(cq[i], cq_handle[i], i);
  }
}


void compute_queue_destory() {
  size_t i;
  for (i = 0; i < NUM_COMP_THREADS; ++i) {
    queue_free(cq[i], cq_handle[i]);
  }
}

bool compute_queue_try_pop(size_t queue_id, data_entry_t **data_entry) {
  void *ret = NULL;
  if ((ret == dequeue(cq[queue_id], cq_handle[queue_id])) == EMPTY) {
    return false;
  } else {
    *data_entry = (data_entry_t *)ret;
    return true;
  }
}

void compute_queue_push(size_t queue_id, data_entry_t *data_entry) {
  enqueue(cq[queue_id], cq_handle[queue_id], (void *)data_entry);
}

bool compute_queue_try_lock(size_t queue_id) {
  return true;
}


void compute_queue_lock(size_t queue_id) {
}


void compute_queue_unlock(size_t queue_id) {
}
