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
