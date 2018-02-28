#!/bin/bash

make -f Makefile.simple LIFE=2 NUM_POOL_SIZE=$7 OVERLAP=$6 NUM_BULKS=$1 READ_QUEUE_LENGTH=$2 WRITE_QUEUE_LENGTH=$3 NUM_READ_ITERS=$4 NUM_WRITE_ENTRIES=$5 SEC_SLEEP=1
./test-simple
make -f Makefile.simple clean
