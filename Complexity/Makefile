.PHONY : all bin objs clean

PROJECT := test

SOURCES := write_back_queue.c compute_queue.c

CC := gcc

OBJS := $(SOURCES:.c=.o)

CFLAGS := -O3 -g -fopenmp -mavx2 -std=c99

NUM_COMP_THREADS   ?= 2
NUM_BULKS          ?= 8
READ_QUEUE_LENGTH  ?= 256
WRITE_QUEUE_LENGTH ?= 16
NUM_READ_ITERS     ?= 1
NUM_WRITE_ENTRIES  ?= 16
SEC_SLEEP          ?= 10
NUM_POOL_SIZE      ?= 8000

DFLAGS := -DNUM_COMP_THREADS=$(NUM_COMP_THREADS) \
          -DNUM_BULKS=$(NUM_BULKS) \
          -DREAD_QUEUE_LENGTH=$(READ_QUEUE_LENGTH) \
          -DWRITE_QUEUE_LENGTH=$(WRITE_QUEUE_LENGTH) \
		      -DNUM_READ_ITERS=$(NUM_READ_ITERS) \
          -DNUM_WRITE_ENTRIES=$(NUM_WRITE_ENTRIES) \
          -DNUM_POOL_SIZE=$(NUM_POOL_SIZE) \
          -DSEC_SLEEP=$(SEC_SLEEP)

LDFLAGS := -ldeterminant

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
