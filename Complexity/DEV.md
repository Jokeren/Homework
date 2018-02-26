02/23

single-thread: 1100000
multi-thread:  5100000

  NUM_COMP_THREADS   ?= 22
  NUM_BULKS          ?= 32
  READ_QUEUE_LENGTH  ?= 192
  WRITE_QUEUE_LENGTH ?= 32
  NUM_READ_ITERS     ?= 3
  NUM_WRITE_ENTRIES  ?= 8
  SEC_SLEEP          ?= 10


1. Get single-thread performance
2. Set up all configurable parameters
3. RM comments
4. Search for the best parameter
5. Adjust execution patterns
6. Adjust data structures

02/24

1. reduce lock regions

single-thread: 1150000
multi-thread:  5600000

2. memory pool

multi-thread:  5600000
NUM_COMP_THREADS   ?= 32
NUM_BULKS          ?= 4
READ_QUEUE_LENGTH  ?= 384
WRITE_QUEUE_LENGTH ?= 80
NUM_READ_ITERS     ?= 4
NUM_WRITE_ENTRIES  ?= 12
SEC_SLEEP          ?= 10
# greater than 10000, or we have memory pool used up problem. consider dynamic memory pool?
NUM_POOL_SIZE      ?= 10000
LIFE               ?= 18
