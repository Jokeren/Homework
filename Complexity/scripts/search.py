#!/usr/bin/python

# Statistics
# data = 1024 * 8 byte * READ_QUEUE_LENGTH = 8KB * READ_QUEUE_LENGTH

# Enssential rules
# NUM_COMP_BULKS <= NUM_READ_BULKS <= WRITE_QUEUE_LENGTH <= READ_QUEUE_LENGTH
# NUM_READ_BULKS % NUM_COMP_BULKS = 0
# WRITE_QUEUE_LENGTH % NUM_COMP_BULKS = 0
# READ_QUEUE_LENGTH % NUM_READ_BULKS = 0

# performance rules
# ?WRITE_QUEUE_LENGTH < L2 cache / 8kb = 256k / 8 k = 32
# ?READ_QUEUE_LENGTH < L3 cache / 8kb = 4MB / 8k = 512

import os

if __name__ == "__main__":
    for num_comp_bulks in range(4, 64, 4):
        for num_read_bulks in range(16, 128, 16):
            if num_read_bulks < num_comp_bulks:
                continue
            if num_read_bulks % num_comp_bulks != 0:
                continue
            for write_queue_length in range(16, 128, 16):
                if write_queue_length < num_read_bulks:
                    continue
                if write_queue_length % num_comp_bulks != 0:
                    continue
                for read_queue_length in range(64, 512, 64):
                    if read_queue_length < write_queue_length:
                        continue
                    if read_queue_length % num_read_bulks != 0:
                        continue
                    os.system("./exec.sh %d %d %d %d\n" % (num_comp_bulks, num_read_bulks, read_queue_length, write_queue_length))
