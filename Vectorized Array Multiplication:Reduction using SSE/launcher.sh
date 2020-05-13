#!/bin/bash

for v in 1 2 4 8 
do
	#echo NUMT = $v
	for t in 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608

	do
		echo $'\n'
		echo NUMT = $v
		echo ARRAYSIZE = $t
	#	NUM_ELEMENTS_PER_CORE = $v/$t
		g++ -DARRAYSIZE=$t -DNUMT=$v project4.cpp -o project4 -lm -fopenmp
	#	echo $'\n'
		./project4
	done
done
