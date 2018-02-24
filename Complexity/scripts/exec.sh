#!/bin/bash

make -f Makefile clean
make -f Makefile NUM_COMP_THREADS=$6 NUM_BULKS=$1 READ_QUEUE_LENGTH=$2 WRITE_QUEUE_LENGTH=$3 NUM_READ_ITERS=$4 NUM_WRITE_ENTRIES=$5 SEC_SLEEP=1
echo ompl | sudo -S LD_LIBRARY_PATH=/home/ompl/Install/determinant:$LD_LIBRARY_PATH ./test
