serial:
	gcc -Wall -Wextra -o bin/serial other/serial.c

parallel:
	mpicc src/parallel.c -fopenmp -o bin/parallel
