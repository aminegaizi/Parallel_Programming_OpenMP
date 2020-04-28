#!/bin/bash

# number of threads
for t in 1 2 4 6 8
do
	echo NUMT = $t
	# number of subdivisions:
	for s in 1 10 100 1000 100000 500000 
	do
		echo NUMTRIALS = $s
		g++ -DNUMTRIALS=$s -DNUMT=$t project1.cpp -o project1 -lm -fopenmp
		./project1
	done
done
