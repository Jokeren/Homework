#ifndef _COMPLEXITY_COMMON_H_
#define _COMPLEXITY_COMMON_H_

#include <stdio.h>

#define D_ARRAY_SIZE 32
#define BUFFER_LENGTH sizeof(int) * D_ARRAY_SIZE * D_ARRAY_SIZE             

#ifndef WRITE_QUEUE_LENGTH
#define WRITE_QUEUE_LENGTH 100
#endif

#ifndef NUM_WRITE_ENTRIES
#define NUM_WRITE_ENTRIES 10
#endif

#ifndef READ_QUEUE_LENGTH
#define READ_QUEUE_LENGTH 100
#endif

#ifndef LIFE
#define LIFE 1
#endif

#ifndef NUM_COMP_THREADS
#define NUM_COMP_THREADS 2
#endif

#ifndef NUM_BULKS
#define NUM_BULKS 1
#endif

#ifndef NUM_READ_ITERS
#define NUM_READ_ITERS 2
#endif

#ifndef SEC_SLEEP
#define SEC_SLEEP 10
#endif

#ifndef NUM_POOL_SIZE
#define NUM_POOL_SIZE 8000
#endif

#define READ_QUEUE_SIZE READ_QUEUE_LENGTH + 1

typedef struct data_entry {
  size_t tag;
  size_t mem_index;
  int *data[NUM_BULKS];
} data_entry_t;

#endif
