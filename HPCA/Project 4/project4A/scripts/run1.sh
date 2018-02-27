#!/bin/bash

throughput=()
read_throughput=()
write_throughput=()
response_time=()
read_response_time=()
write_response_time=()

for((i=1;i<=10;i++))
do
  gcc sim.c ./yacsim.o ./utils.o -lm -o runme -DNUM_OUTSTANDING_REQUESTS=$i -DSTRIPED=TRUE
  ./runme > tmp
  throughput+=(`grep "Average Throughput" ./tmp | cut -d ":" -f 2`)
  read_throughput+=(`grep "Read Throughput" ./tmp | cut -d ":" -f 2`)
  write_throughput+=(`grep "Write Throughput" ./tmp | cut -d ":" -f 2`)
  response_time+=(`grep "Average Response Time" ./tmp | cut -d ":" -f 2`)
  read_response_time+=(`grep "Read Response Time" ./tmp | cut -d ":" -f 2`)
  write_response_time+=(`grep "Write Response Time" ./tmp | cut -d ":" -f 2`)
done

echo "Throughput:"
echo ${throughput[*]}
echo "Read Throughput:"
echo ${read_throughput[*]}
echo "Write Throughput:"
echo ${write_throughput[*]}
echo "Response Time:"
echo ${response_time[*]}
echo "Read Response Time:"
echo ${read_response_time[*]}
echo "Write Response Time:"
echo ${write_response_time[*]}
echo ""

throughput=()
read_throughput=()
write_throughput=()
response_time=()
read_response_time=()
write_response_time=()

for((i=10;i<=170;i=i+20))
do
  gcc sim.c ./yacsim.o ./utils.o -lm -o runme -DNUM_OUTSTANDING_REQUESTS=$i -DSTRIPED=TRUE
  ./runme > tmp
  throughput+=(`grep "Average Throughput" ./tmp | cut -d ":" -f 2`)
  read_throughput+=(`grep "Read Throughput" ./tmp | cut -d ":" -f 2`)
  write_throughput+=(`grep "Write Throughput" ./tmp | cut -d ":" -f 2`)
  response_time+=(`grep "Average Response Time" ./tmp | cut -d ":" -f 2`)
  read_response_time+=(`grep "Read Response Time" ./tmp | cut -d ":" -f 2`)
  write_response_time+=(`grep "Write Response Time" ./tmp | cut -d ":" -f 2`)
done

echo "Throughput:"
echo ${throughput[*]}
echo "Read Throughput:"
echo ${read_throughput[*]}
echo "Write Throughput:"
echo ${write_throughput[*]}
echo "Response Time:"
echo ${response_time[*]}
echo "Read Response Time:"
echo ${read_response_time[*]}
echo "Write Response Time:"
echo ${write_response_time[*]}
echo ""

throughput=()
read_throughput=()
write_throughput=()
response_time=()
read_response_time=()
write_response_time=()

for((i=1;i<=10;i++))
do
  gcc sim.c ./yacsim.o ./utils.o -lm -o runme -DNUM_OUTSTANDING_REQUESTS=$i -DSTRIPED=FALSE
  ./runme > tmp
  throughput+=(`grep "Average Throughput" ./tmp | cut -d ":" -f 2`)
  read_throughput+=(`grep "Read Throughput" ./tmp | cut -d ":" -f 2`)
  write_throughput+=(`grep "Write Throughput" ./tmp | cut -d ":" -f 2`)
  response_time+=(`grep "Average Response Time" ./tmp | cut -d ":" -f 2`)
  read_response_time+=(`grep "Read Response Time" ./tmp | cut -d ":" -f 2`)
  write_response_time+=(`grep "Write Response Time" ./tmp | cut -d ":" -f 2`)
done

echo "Throughput:"
echo ${throughput[*]}
echo "Read Throughput:"
echo ${read_throughput[*]}
echo "Write Throughput:"
echo ${write_throughput[*]}
echo "Response Time:"
echo ${response_time[*]}
echo "Read Response Time:"
echo ${read_response_time[*]}
echo "Write Response Time:"
echo ${write_response_time[*]}
echo ""

throughput=()
read_throughput=()
write_throughput=()
response_time=()
read_response_time=()
write_response_time=()

for((i=10;i<=170;i=i+20))
do
  gcc sim.c ./yacsim.o ./utils.o -lm -o runme -DNUM_OUTSTANDING_REQUESTS=$i -DSTRIPED=FALSE
  ./runme > tmp
  throughput+=(`grep "Average Throughput" ./tmp | cut -d ":" -f 2`)
  read_throughput+=(`grep "Read Throughput" ./tmp | cut -d ":" -f 2`)
  write_throughput+=(`grep "Write Throughput" ./tmp | cut -d ":" -f 2`)
  response_time+=(`grep "Average Response Time" ./tmp | cut -d ":" -f 2`)
  read_response_time+=(`grep "Read Response Time" ./tmp | cut -d ":" -f 2`)
  write_response_time+=(`grep "Write Response Time" ./tmp | cut -d ":" -f 2`)
done

echo "Throughput:"
echo ${throughput[*]}
echo "Read Throughput:"
echo ${read_throughput[*]}
echo "Write Throughput:"
echo ${write_throughput[*]}
echo "Response Time:"
echo ${response_time[*]}
echo "Read Response Time:"
echo ${read_response_time[*]}
echo "Write Response Time:"
echo ${write_response_time[*]}
echo ""
