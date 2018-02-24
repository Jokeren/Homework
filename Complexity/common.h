#ifndef _COMPLEXITY_COMMON_H_
#define _COMPLEXITY_COMMON_H_

#include <stdio.h>

#define D_ARRAY_SIZE 32
#define BUFFER_LENGTH sizeof(int) * D_ARRAY_SIZE * D_ARRAY_SIZE             

#ifndef WRITE_QUEUE_LENGTH
#define WRITE_QUEUE_LENGTH 100
#endif

#ifndef WRITE_QUEUE_ENTRIES
#define WRITE_QUEUE_ENTRIES 10
#endif

#ifndef READ_QUEUE_LENGTH
#define READ_QUEUE_LENGTH 100
#endif

#define LIFE 1

#ifndef NUM_COMP_THREADS
#define NUM_COMP_THREADS 2
#endif

#ifndef NUM_READ_BULKS
#define NUM_READ_BULKS 1
#endif

#ifndef NUM_COMP_BULKS
#define NUM_COMP_BULKS 1
#endif

#define READ_QUEUE_SIZE READ_QUEUE_LENGTH + NUM_READ_BULKS

#endif
