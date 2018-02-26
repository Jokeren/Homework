#ifndef _COMPLEXITY_WRITE_BACK_QUEUE_H_
#define _COMPLEXITY_WRITE_BACK_QUEUE_H_

#include <stdbool.h>
#include <stddef.h>
#include "common.h"

typedef struct write_back_entry write_back_entry_t;

void write_back_queue_init();

void write_back_queue_destory();

void write_back_queue_lock(size_t index);

void write_back_queue_unlock(size_t index);

bool write_back_queue_try_get_val(size_t index, long long *val);

void write_back_queue_set_val(size_t index, long long val);

void write_back_queue_display();

#endif
