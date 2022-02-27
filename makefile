PROCESS := 8

serial:
	gcc -Wall -Wextra -o bin/serial other/serial.c

parallel:
	mpicc src/parallel.c -fopenmp -o bin/parallel


test-serial: serial
	@for tc in other/testcase/*; do \
		echo "$${tc}"; cat $${tc} | ./bin/serial; \
	done

test-parallel: parallel
	@for tc in other/testcase/*; do \
		echo "$${tc}"; mpirun -n ${PROCESS} bin/parallel $${tc}; \
	done

diff-test: serial parallel
	cat other/testcase/K04-06-TC1 | ./bin/serial > result/K04-06-TC1_serial.txt
	cat other/testcase/K04-06-TC2 | ./bin/serial > result/K04-06-TC2_serial.txt
	cat other/testcase/K04-06-TC3 | ./bin/serial > result/K04-06-TC3_serial.txt
	cat other/testcase/K04-06-TC4 | ./bin/serial > result/K04-06-TC4_serial.txt
	mpirun -n ${PROCESS} bin/parallel other/testcase/K04-06-TC1 > result/K04-06-TC1_paralel.txt
	mpirun -n ${PROCESS} bin/parallel other/testcase/K04-06-TC2 > result/K04-06-TC2_paralel.txt
	mpirun -n ${PROCESS} bin/parallel other/testcase/K04-06-TC3 > result/K04-06-TC3_paralel.txt
	mpirun -n ${PROCESS} bin/parallel other/testcase/K04-06-TC4 > result/K04-06-TC4_paralel.txt
	diff result/K04-06-TC1_serial.txt result/K04-06-TC1_paralel.txt
	diff result/K04-06-TC2_serial.txt result/K04-06-TC2_paralel.txt
	diff result/K04-06-TC3_serial.txt result/K04-06-TC3_paralel.txt
	diff result/K04-06-TC4_serial.txt result/K04-06-TC4_paralel.txt
