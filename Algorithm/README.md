*Keren Zhou's algorithm homework at Dongbo bu's class.*

-----------------------------------------------------------------

###User Guide

	cd ./src
	make

- HW1:

-src

	-stable_match.cc
-data

	-boys_ranking.txt
	-girls_ranking.txt

- HW2:

-src

	-inversions_qsort.cc(just a test, in fact we can never use qsort to do inversions counting)
	-inversions_merge.cc
	-closet.cc
	-closet_naive.cc(test correctness of the divide-and-conquer algorithm)
-script

	-gen_closet.sh(generate a test case for testing closet pairs)
-data

	-closet.txt
	-inversions.txt
	-inversions_small.txt(small amount of data for testing inversions counting)

- HW3:

-src

	-needleman.cc
	-print_neatly.cc
	-optimal_keypad.cc	
-data

	-print_neatly.txt
	-needleman.cc

- HW4:

-src

	-huffman.cc
	-dijkstra_heap.cc
	-dijkstra_list.cc
-data

	-graph.txt
	-aesop_fables.txt

- HW5:

-src

	-simplex.cc
	
-data

	-simplex.txt(only solve minimum form input, values should be transformed first)
	
-src_lp

	-airport.mod
	-color.mod
	-interval.mod
	-simplex.mod
	
-------------------------------------------------------------------
###Logs

10/30/2014
1. Add Makefile
2. Add instruction of file related with each homework

10/23/2014
1. Executable file is in the `bin` directory; output files are in `output` directory; source files are in `src` directory.
2. Makefiles are on schedule.
3. A interactive running script is under developing.
4. I also want to combine other problems' implementation into this project, but now I merely have time to finish the heavy homework.
