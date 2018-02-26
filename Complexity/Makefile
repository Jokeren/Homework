.PHONY : all bin objs clean

PROJECT := test

SOURCES := write_back_queue.c compute_queue.c


CC := gcc

OBJS := $(SOURCES:.c=.o)

CFLAGS := -O3 -g -fopenmp -mavx2 -std=c99

NUM_COMP_THREADS   ?= 34
NUM_BULKS          ?= 4
READ_QUEUE_LENGTH  ?= 384
WRITE_QUEUE_LENGTH ?= 256
NUM_READ_ITERS     ?= 4
NUM_WRITE_ENTRIES  ?= 12
SEC_SLEEP          ?= 10
# greater than 10000, or we have memory pool used up problem. consider dynamic memory pool?
NUM_POOL_SIZE      ?= 6000
LIFE               ?= 1

DFLAGS := -DNUM_COMP_THREADS=$(NUM_COMP_THREADS) \
          -DNUM_BULKS=$(NUM_BULKS) \
          -DREAD_QUEUE_LENGTH=$(READ_QUEUE_LENGTH) \
          -DWRITE_QUEUE_LENGTH=$(WRITE_QUEUE_LENGTH) \
		      -DNUM_READ_ITERS=$(NUM_READ_ITERS) \
          -DNUM_WRITE_ENTRIES=$(NUM_WRITE_ENTRIES) \
          -DNUM_POOL_SIZE=$(NUM_POOL_SIZE) \
          -DSEC_SLEEP=$(SEC_SLEEP) \
          -DLIFE=$(LIFE)

LDFLAGS := -ldeterminant -lpthread

all: bin objs

bin: $(PROJECT)

objs: $(OBJS)

$(PROJECT): $(OBJS) $(PROJECT).c
	$(CC) $(CFLAGS) $(LDFLAGS) $(DFLAGS) -o $@ $^

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(DFLAGS) -c $<

clean:
	rm $(OBJS) $(PROJECT)

#utils
print-% : ; $(info $* is $(flavor $*) variable set to [$($*)]) @true
