.PHONY : all bin objs clean

PROJECT := test

SOURCES := write_back_queue.c lf_compute_queue.c


CC := gcc

OBJS := $(SOURCES:.c=.o)

CFLAGS := -O4 -g -fopenmp -mavx2 -std=c99 -D_GNU_SOURCE -lpthread -DMSQUEUE

NUM_COMP_THREADS   ?= 12
NUM_BULKS          ?= 4
READ_QUEUE_LENGTH  ?= 384
WRITE_QUEUE_LENGTH ?= 512
NUM_READ_ITERS     ?= 4
NUM_WRITE_ENTRIES  ?= 24
SEC_SLEEP          ?= 10
NUM_POOL_SIZE      ?= 6000
LIFE               ?= 18

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
	$(CC) $(CFLAGS) $(LDFLAGS) $(DFLAGS) -o $@ $^ queues/hzdptr.o queues/xxhash.o

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(DFLAGS) -c $<

clean:
	rm $(OBJS) $(PROJECT)

#utils
print-% : ; $(info $* is $(flavor $*) variable set to [$($*)]) @true
