serial:
	gcc -Wall -Wextra -o bin/serial other/serial.c

parallel:
	mpicc src/parallel.c -fopenmp -o bin/parallel


test-serial: serial
	cat other/testcase/K04-06-TC1 | ./bin/serial
	cat other/testcase/K04-06-TC2 | ./bin/serial
	cat other/testcase/K04-06-TC3 | ./bin/serial
	cat other/testcase/K04-06-TC4 | ./bin/serial

test-parallel: parallel
	mpirun -n 8 bin/parallel other/testcase/K04-06-TC1
	mpirun -n 8 bin/parallel other/testcase/K04-06-TC2
	mpirun -n 8 bin/parallel other/testcase/K04-06-TC3
	mpirun -n 8 bin/parallel other/testcase/K04-06-TC4

diff-test: serial parallel
	cat other/testcase/K04-06-TC1 | ./bin/serial > bin/ser-tc1.txt
	cat other/testcase/K04-06-TC2 | ./bin/serial > bin/ser-tc2.txt
	cat other/testcase/K04-06-TC3 | ./bin/serial > bin/ser-tc3.txt
	cat other/testcase/K04-06-TC4 | ./bin/serial > bin/ser-tc4.txt
	mpirun -n 8 bin/parallel other/testcase/K04-06-TC1 > bin/par-tc1.txt
	mpirun -n 8 bin/parallel other/testcase/K04-06-TC2 > bin/par-tc2.txt
	mpirun -n 8 bin/parallel other/testcase/K04-06-TC3 > bin/par-tc3.txt
	mpirun -n 8 bin/parallel other/testcase/K04-06-TC4 > bin/par-tc4.txt
	diff bin/ser-tc1.txt bin/par-tc1.txt
	diff bin/ser-tc2.txt bin/par-tc2.txt
	diff bin/ser-tc3.txt bin/par-tc3.txt
	diff bin/ser-tc4.txt bin/par-tc4.txt
