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
    thread_range = [2, 8, 16, 32, 64, 70]
    for num_threads in thread_range:
        for num_bulks in range(4, 8, 4):
            for write_queue_length in range(256, 272, 16):
                if write_queue_length < num_bulks:
                    continue
                if write_queue_length % num_bulks != 0:
                    continue
                for read_queue_length in range(384, 512, 128):
                    if read_queue_length < write_queue_length:
                        continue
                    if read_queue_length % num_bulks != 0:
                        continue
                    for num_read_iters in range(3, 4):
                        if write_queue_length > num_read_iters * num_bulks:
                            for num_write_entries in range(12, 16, 4):
                                for num_pool_size in range(10000, 10001, 1):
                                    os.system("scripts/exec.sh %d %d %d %d %d %d %d\n" % (num_bulks, read_queue_length, write_queue_length, num_read_iters, num_write_entries, num_threads, num_pool_size))

