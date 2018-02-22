#ifndef _COMPLEXITY_COMMON_H_
#define _COMPLEXITY_COMMON_H_

#define D_ARRAY_SIZE 32
#define BUFFER_LENGTH sizeof(int) * D_ARRAY_SIZE * D_ARRAY_SIZE             

#ifndef WRITE_QUEUE_LENGTH
#define WRITE_QUEUE_LENGTH 100
#endif

#ifndef READ_QUEUE_LENGTH
#define READ_QUEUE_LENGTH 100
#endif

#define LIFE 18

#define NUM_COMP_THREADS 4
#define NUM_BULKS 4

#endif
