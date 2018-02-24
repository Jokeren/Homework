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

num_threads=2

if __name__ == "__main__":
    for num_bulks in range(16, 64, 16):
        for write_queue_length in range(32, 128, 32):
            if write_queue_length < num_bulks:
                continue
            if write_queue_length % num_bulks != 0:
                continue
            for read_queue_length in range(64, 512, 64):
                if read_queue_length < write_queue_length:
                    continue
                if read_queue_length % num_bulks != 0:
                    continue
                for num_read_iters in range(1, 4):
                    if write_queue_length < num_read_iters * num_bulks:
                        for num_write_entries in range(8, 16, 8):
                            os.system("scripts/exec.sh %d %d %d %d %d %d\n" % (num_bulks, read_queue_length, write_queue_length, num_read_iters, num_write_entries, num_threads))
