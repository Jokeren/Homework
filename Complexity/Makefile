.PHONY : all bin objs clean

PROJECT := test

SOURCES := write_back_queue.c compute_queue.c

CC := gcc

OBJS := $(SOURCES:.c=.o)

CFLAGS := -O3 -fopenmp -mavx2 -std=c99

LDFLAGS := -ldeterminant

all: bin objs

bin: $(PROJECT)

objs: $(OBJS)

$(PROJECT): $(OBJS) $(PROJECT).c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $<

clean:
	rm $(OBJS) $(PROJECT)

#utils
print-% : ; $(info $* is $(flavor $*) variable set to [$($*)]) @true
