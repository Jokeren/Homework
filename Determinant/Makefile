PROJECT := determinant

CONFIGS := Makefile.config
include $(CONFIGS)

.PHONY: clean all objects bins install

LIB_DIR := lib/
INC_DIR := include/
BIN_DIR := bin/
BUILD_DIR := build/

LIB := $(LIB_DIR)libdeterminant.so

ifdef DEBUG
OFLAGS += -g
endif

ifdef PERFORMANCE
OFLAGS += -O3
endif

ifdef OPENMP
OFLAGS += -fopenmp
endif

ifdef BLAS
ifeq BLAS openblas
OFLGAS += -lopenblas
endif
endif

CFLAGS := -fPIC $(OFLAGS)
LDFLAGS := 
SRCS := $(shell find . -maxdepth 3 -name "*.c")
OBJECTS := $(addprefix $(BUILD_DIR), $(patsubst %.c, %.o, $(SRCS)))
OBJECTS_DIR := $(sort $(addprefix $(BUILD_DIR), $(dir $(SRCS))))
BINS := $(BIN_DIR)$(PROJECT)

all: dirs bin objects lib

dirs: $(OBJECTS_DIR) $(LIB_DIR) $(BIN_DIR)

bin: $(BINS)

objects: $(OBJECTS)

lib: $(LIB)

$(OBJECTS_DIR):
	mkdir -p $@

$(LIB_DIR):
	mkdir -p $@

$(BIN_DIR):
	mkdir -p $@

$(BINS): $(BIN_DIR)% : %.c $(LIB)
	$(CC) $(CFLAGS) -I$(INC_DIR) $< -o $@ $(LDFLAGS) -L$(LIB_DIR) -ldeterminant

$(LIB): $(OBJECTS)
	$(CC) -shared $^ -o $@ $(LDFLAGS)

$(OBJECTS): $(BUILD_DIR)%.o : %.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -o $@ -c $<

clean:
	-rm -rf $(BUILD_DIR) $(LIB_DIR) $(BIN_DIR)

ifdef PREFIX
install:
	mkdir -p $(PREFIX)/$(BUILD_DIR)
	mkdir -p $(PREFIX)/$(INC_DIR)
	mkdir -p $(PREFIX)/$(BIN_DIR)
	cp -rf $(LIB_DIR) $(PREFIX)
	cp -rf $(INC_DIR)$(PROJECT).h $(PREFIX)/$(INC_DIR)
	cp -rf $(BIN_DIR) $(PREFIX)
endif

#utils
print-% : ; $(info $* is $(flavor $*) variable set to [$($*)]) @true
