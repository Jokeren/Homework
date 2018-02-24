#!/bin/bash

make -f Makefile.test clean
make -f Makefile.test NUM_COMP_THREADS=2 NUM_COMP_BULKS=$1 NUM_READ_BULKS=$2 READ_QUEUE_LENGTH=$3 WRITE_QUEUE_LENGTH=$4 
echo ompl | sudo -S LD_LIBRARY_PATH=/home/ompl/Install/determinant:$LD_LIBRARY_PATH ./test
