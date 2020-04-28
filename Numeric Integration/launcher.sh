#!/bin/bash

# number of threads
for t in 1 2 4 6 8
do
   #    echo NUMT = $t
		
	# number of nodes
	for v in 4 8 16 32 64 128 256 512 1024 
	do	
	#	echo NUMNODES = $v
		g++ -DNUMT=$t -DNUMNODES=$v project2.cpp -o project2 -lm -fopenmp
		./project2
        done
done
