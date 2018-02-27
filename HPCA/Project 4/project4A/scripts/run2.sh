#!/bin/bash

read_fraction=(1.0 0.75 0.5 0.25 0.0)
write_fraction=(0.0 0.25 0.5 0.75 1.0)

for((i=0;i<5;i++))
do
  rf=${read_fraction[$i]}
  wf=${write_fraction[$i]}
  echo "Read Fraction vs Write Fraction"
  echo $rf" : "$wf
  echo ""
  gcc maketrace.c -DREAD_FRACTION=$rf
  ./a.out > tmp
  ./scripts/run1.sh

  echo ""
done
