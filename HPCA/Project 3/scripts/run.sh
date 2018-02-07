#!/bin/bash

exec_time=()
retire_ins=()
retire_time=()
rsfull_cycle=()

for((i=1;i<=128;i=i*2))
do
  gcc *.c ./yacsim.o -lm -o runme -DLOADFP_CYCLES=2 -DNUM_RESERVATION_STATIONS=$i
  ./runme > tmp
  exec_time+=(`grep "Execution completed at time" ./tmp | cut -d " " -f 6`)
  retire_ins+=(`grep "Number Instructions Retired" ./tmp | cut -d " " -f 4`)
  retire_time+=(`grep "Retirement completed  at time" ./tmp | cut -d " " -f 7`)
  rsfull_cycle+=(`grep "RSFull : " ./tmp | cut -d " " -f 9`)
done

echo ""
echo "Execution time:"
echo ${exec_time[*]}
echo "Instruction retired:"
echo ${retire_ins[*]}
echo "Retire time:"
echo ${retire_time[*]}
echo "RS full cycles:"
echo ${rsfull_cycle[*]}

exec_time=()
retire_ins=()
retire_time=()
rsfull_cycle=()

for((i=1;i<=128;i=i*2))
do
  gcc *.c ./yacsim.o -lm -o runme -DLOADFP_CYCLES=16 -DNUM_RESERVATION_STATIONS=$i
  ./runme > tmp
  exec_time+=(`grep "Execution completed at time" ./tmp | cut -d " " -f 6`)
  retire_ins+=(`grep "Number Instructions Retired" ./tmp | cut -d " " -f 4`)
  retire_time+=(`grep "Retirement completed  at time" ./tmp | cut -d " " -f 7`)
  rsfull_cycle+=(`grep "RSFull : " ./tmp | cut -d " " -f 9`)
done

echo ""
echo "Execution time:"
echo ${exec_time[*]}
echo "Instruction retired:"
echo ${retire_ins[*]}
echo "Retire time:"
echo ${retire_time[*]}
echo "RS full cycles:"
echo ${rsfull_cycle[*]}

exec_time=()
retire_ins=()
retire_time=()
rsfull_cycle=()

for((i=1;i<=5;i=i+1))
do
  gcc *.c ./yacsim.o -lm -o runme -DLOADFP_CYCLES=2 -DNUM_RESERVATION_STATIONS=4 -DNUM_COPIES=$i
  ./runme > tmp
  exec_time+=(`grep "Execution completed at time" ./tmp | cut -d " " -f 6`)
  retire_ins+=(`grep "Number Instructions Retired" ./tmp | cut -d " " -f 4`)
  retire_time+=(`grep "Retirement completed  at time" ./tmp | cut -d " " -f 7`)
  rsfull_cycle+=(`grep "RSFull : " ./tmp | cut -d " " -f 9`)
done

echo ""
echo "Execution time:"
echo ${exec_time[*]}
echo "Instruction retired:"
echo ${retire_ins[*]}
echo "Retire time:"
echo ${retire_time[*]}
echo "RS full cycles:"
echo ${rsfull_cycle[*]}

exec_time=()
retire_ins=()
retire_time=()
rsfull_cycle=()

for((i=1;i<=5;i=i+1))
do
  gcc *.c ./yacsim.o -lm -o runme -DLOADFP_CYCLES=2 -DNUM_RESERVATION_STATIONS=64 -DNUM_COPIES=$i
  ./runme > tmp
  exec_time+=(`grep "Execution completed at time" ./tmp | cut -d " " -f 6`)
  retire_ins+=(`grep "Number Instructions Retired" ./tmp | cut -d " " -f 4`)
  retire_time+=(`grep "Retirement completed  at time" ./tmp | cut -d " " -f 7`)
  rsfull_cycle+=(`grep "RSFull : " ./tmp | cut -d " " -f 9`)
done

echo ""
echo "Execution time:"
echo ${exec_time[*]}
echo "Instruction retired:"
echo ${retire_ins[*]}
echo "Retire time:"
echo ${retire_time[*]}
echo "RS full cycles:"
echo ${rsfull_cycle[*]}

exec_time=()
retire_ins=()
retire_time=()
rsfull_cycle=()

for((i=1;i<=5;i=i+1))
do
  gcc *.c ./yacsim.o -lm -o runme -DLOADFP_CYCLES=16 -DNUM_RESERVATION_STATIONS=4 -DNUM_COPIES=$i
  ./runme > tmp
  exec_time+=(`grep "Execution completed at time" ./tmp | cut -d " " -f 6`)
  retire_ins+=(`grep "Number Instructions Retired" ./tmp | cut -d " " -f 4`)
  retire_time+=(`grep "Retirement completed  at time" ./tmp | cut -d " " -f 7`)
  rsfull_cycle+=(`grep "RSFull : " ./tmp | cut -d " " -f 9`)
done

echo ""
echo "Execution time:"
echo ${exec_time[*]}
echo "Instruction retired:"
echo ${retire_ins[*]}
echo "Retire time:"
echo ${retire_time[*]}
echo "RS full cycles:"
echo ${rsfull_cycle[*]}

exec_time=()
retire_ins=()
retire_time=()
rsfull_cycle=()

for((i=1;i<=5;i=i+1))
do
  gcc *.c ./yacsim.o -lm -o runme -DLOADFP_CYCLES=16 -DNUM_RESERVATION_STATIONS=64 -DNUM_COPIES=$i
  ./runme > tmp
  exec_time+=(`grep "Execution completed at time" ./tmp | cut -d " " -f 6`)
  retire_ins+=(`grep "Number Instructions Retired" ./tmp | cut -d " " -f 4`)
  retire_time+=(`grep "Retirement completed  at time" ./tmp | cut -d " " -f 7`)
  rsfull_cycle+=(`grep "RSFull : " ./tmp | cut -d " " -f 9`)
done

echo ""
echo "Execution time:"
echo ${exec_time[*]}
echo "Instruction retired:"
echo ${retire_ins[*]}
echo "Retire time:"
echo ${retire_time[*]}
echo "RS full cycles:"
echo ${rsfull_cycle[*]}
