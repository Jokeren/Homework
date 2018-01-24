#!/bin/bash

sim=()
mem_read=()
mem_write=()
cpu_busy=()
mem_busy=()
hits=()
misses=()
evictions=()
write_backs=()

for((i=0;i<=300;i+=20))
do
  gcc write_TEMPLATE.c utils.o ./yacsim.o -lm -o runme -DCPU_DELAY=$i.0
  ./runme &> tmp
  sim+=(`grep "Simulation ended" tmp | cut -d " " -f 5`)
  mem_read+=(`grep "Total Memory Read Time" tmp | cut -d ":" -f 2`)
  mem_write+=(`grep "Total Memory Write Time" tmp | cut -d ":" -f 2`)
  cpu_busy+=(`grep "Total time CPU Busy" tmp | cut -d ":" -f 2`)
  hits+=(`grep "Total number of Hits" tmp | cut -d ":" -f 2`)
  misses+=(`grep "Total number of Misses" tmp | cut -d ":" -f 2`)
  evictions+=(`grep "Total number of Evictions" tmp | cut -d ":" -f 2`)
  write_backs+=(`grep "Total number of Writebacks" tmp | cut -d ":" -f 2`)
  index=`expr $i / 20`
  mem_busy+=(`echo ${mem_read[$index]} + ${mem_write[$index]} | bc`)
done

echo "Total Simulation Time:"
echo ${sim[*]}
echo "Total Memory Read Time:"
echo ${mem_read[*]}
echo "Total Memory Write Time:"
echo ${mem_write[*]}
echo "Total Memory Busy Time:"
echo ${mem_busy[*]}
echo "Total CPU Busy Time:"
echo ${cpu_busy[*]}
echo "Total Number of Hits:"
echo ${hits[*]}
echo "Total Number of Misses:"
echo ${misses[*]}
echo "Total Number of Eviction:"
echo ${evictions[*]}
echo "Total Number of Writebacks:"
echo ${write_backs[*]}

rm tmp
