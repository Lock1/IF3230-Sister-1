serial:
	gcc -Wall -Wextra -o bin/serial other/serial.c

parallel:
	mpicc src/parallel.c -fopenmp -o bin/parallel


test-serial:
	cat other/testcase/K04-01-TC1 | ./bin/serial

test-parallel:
	mpirun -n 2 bin/parallel other/testcase/K04-01-TC1
