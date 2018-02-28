#!/usr/bin/python

# Statistics
# data = 1024 * 8 byte * READ_QUEUE_LENGTH = 8KB * READ_QUEUE_LENGTH

# Enssential rules
# NUM_BULKS <= WRITE_QUEUE_LENGTH <= READ_QUEUE_LENGTH
# WRITE_QUEUE_LENGTH % NUM_BULKS = 0
# READ_QUEUE_LENGTH % NUM_BULKS = 0
# READ_QUEUE_LENGTH % NUM_BULKS = 0
# NUM_READ_ITERS * NUM_BULKS <= WRITE_QUEUE_LENGTH

# performance rules
# ?WRITE_QUEUE_LENGTH < L2 cache / 8kb = 256k / 8 k = 32
# ?READ_QUEUE_LENGTH < L3 cache / 8kb = 4MB / 8k = 512
# NUM_WRITE_ENTRIES < 32

import os


if __name__ == "__main__":
    read_queue_length = 0
    write_queue_length = 0
    num_read_iters = 0
    num_write_entries = 0
    num_pool_size = 0
    for num_threads in range(8, 17):
        for num_bulks in range(4, 64, 4):
            os.system("scripts/exec.sh %d %d %d %d %d %d %d\n" % (num_bulks, read_queue_length, write_queue_length, num_read_iters, num_write_entries, num_threads, num_pool_size))
