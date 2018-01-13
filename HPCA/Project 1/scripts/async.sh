#!/bin/bash

echo "Step 1:"
for((i=0;i<=10;i++))
do
  gcc -Wno-pointer-to-int-cast async_template.c utils.o ./yacsim.o -lm -o runme -DCPU_DELAY=$i.0
  ./runme &> tmp
  grep "Simulation ended" tmp
  grep "Total Memory Idle Time" tmp
done

echo "Step 2:"
for((i=0;i<=10;i++))
do
  gcc -Wno-pointer-to-int-cast async_template.c utils.o ./yacsim.o -lm -o runme -DCACHESIZE=2048 -DBLKSIZE=32 -DCPU_DELAY=$i.0
  ./runme &> tmp
  grep "Simulation ended" tmp
  grep "Total Memory Idle Time" tmp
done

echo "Step 3:"
BLKARR=(16 32 64 128 256)
for((i=0;i<5;i++))
do
  for((j=0;j<=10;j++))
  do
    gcc -Wno-pointer-to-int-cast async_template.c utils.o ./yacsim.o -lm -o runme -DBLKSIZE=${BLKARR[$i]} -DCPU_DELAY=$j.0
    ./runme &> tmp
    grep "Simulation ended" tmp
  done
done

rm tmp
