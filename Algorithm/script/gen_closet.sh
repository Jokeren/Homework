#!/bin/bash

#AWKSCRIPT="BEGIN{srand();print rand()*100, rand()*100}"
NRAND=100
NMAX=1000
FILE="../data/closet.txt"
rm ${FILE}

for((i=0;i<NRAND;i++))
do
	N1=$[$RANDOM % $NRAND]
	N2=$[$RANDOM % $NRAND]
	echo -e $N1" "$N2 >> ${FILE}
done	
