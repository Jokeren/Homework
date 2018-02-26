#ifndef _COMPLEXITY_COMPUTE_QUEUE_H_
#define _COMPLEXITY_COMPUTE_QUEUE_H_

#include <stdbool.h>
#include <stddef.h>
#include "common.h"

typedef struct compute_queue_entry compute_queue_entry_t;

void compute_queue_init();

void compute_queue_destory();

void compute_queue_wait(size_t queue_id);

void compute_queue_signal(size_t queue_id);

bool compute_queue_try_lock(size_t queue_id);

void compute_queue_lock(size_t queue_id);

void compute_queue_unlock(size_t queue_id);

void compute_queue_push(size_t queue_id, data_entry_t *data_entry);

bool compute_queue_try_pop(size_t queue_id, data_entry_t *data_entry);

#endif
