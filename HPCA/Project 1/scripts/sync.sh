#!/bin/bash

echo "Step 1:"
for((i=0;i<=10;i++))
do
  gcc sync.c utils.o ./yacsim.o -lm -o runme -DCPU_DELAY=$i.0
  ./runme &> tmp
  grep "Simulation ended" tmp
  grep "Total Memory Idle Time" tmp
done

echo "Step 2:"
for((i=0;i<=10;i++))
do
  gcc sync.c utils.o ./yacsim.o -lm -o runme -DCACHESIZE=2048 -DBLKSIZE=32 -DCPU_DELAY=$i.0
  ./runme &> tmp
  grep "Simulation ended" tmp
  grep "Total Memory Idle Time" tmp
done

rm tmp
