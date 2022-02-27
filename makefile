PROCESS := 8
THREAD  := 5
NODE_1  := k04-06@35.240.188.57
NODE_2  := k04-06@34.87.108.45
NODE_3  := k04-06@34.124.168.121
NODE_4  := k04-06@35.198.245.105
KEY     := ~/.ssh/k04-06


serial:
	gcc -Wall -Wextra -o bin/serial other/serial.c

parallel:
	mpicc src/parallel.c --openmp -o bin/parallel


test-serial: serial
	@for tc in other/testcase/*; do \
		echo "$${tc}"; cat $${tc} | ./bin/serial -timer; \
	done

test-parallel: parallel
	@for tc in other/testcase/*; do \
		echo "$${tc}"; mpirun -n ${PROCESS} bin/parallel $${tc} ${THREAD} -timer; \
	done

diff-test: serial parallel
	cat other/testcase/K04-06-TC1 | ./bin/serial > result/K04-06-TC1_serial.txt
	cat other/testcase/K04-06-TC2 | ./bin/serial > result/K04-06-TC2_serial.txt
	cat other/testcase/K04-06-TC3 | ./bin/serial > result/K04-06-TC3_serial.txt
	cat other/testcase/K04-06-TC4 | ./bin/serial > result/K04-06-TC4_serial.txt
	mpirun -n ${PROCESS} bin/parallel other/testcase/K04-06-TC1 ${THREAD} > result/K04-06-TC1_paralel.txt
	mpirun -n ${PROCESS} bin/parallel other/testcase/K04-06-TC2 ${THREAD} > result/K04-06-TC2_paralel.txt
	mpirun -n ${PROCESS} bin/parallel other/testcase/K04-06-TC3 ${THREAD} > result/K04-06-TC3_paralel.txt
	mpirun -n ${PROCESS} bin/parallel other/testcase/K04-06-TC4 ${THREAD} > result/K04-06-TC4_paralel.txt
	diff result/K04-06-TC1_serial.txt result/K04-06-TC1_paralel.txt
	diff result/K04-06-TC2_serial.txt result/K04-06-TC2_paralel.txt
	diff result/K04-06-TC3_serial.txt result/K04-06-TC3_paralel.txt
	diff result/K04-06-TC4_serial.txt result/K04-06-TC4_paralel.txt


connect-1:
	ssh -i ${KEY} ${NODE_1}

connect-2:
	ssh -i ${KEY} ${NODE_2}

connect-3:
	ssh -i ${KEY} ${NODE_3}

connect-4:
	ssh -i ${KEY} ${NODE_4}


scp-node:
	scp -i ${KEY} bin/parallel ${NODE_1}:~/bin/parallel
	scp -i ${KEY} bin/parallel ${NODE_2}:~/bin/parallel
	scp -i ${KEY} bin/parallel ${NODE_3}:~/bin/parallel
	scp -i ${KEY} bin/parallel ${NODE_4}:~/bin/parallel
	scp -i ${KEY} other/2host ${NODE_1}:~/bin/2host
	scp -i ${KEY} other/3host ${NODE_1}:~/bin/3host
	scp -i ${KEY} other/4host ${NODE_1}:~/bin/4host
	scp -i ${KEY} other/testcase/K04-06-TC1 ${NODE_1}:~/bin/tc1
	scp -i ${KEY} other/testcase/K04-06-TC2 ${NODE_1}:~/bin/tc2
	scp -i ${KEY} other/testcase/K04-06-TC3 ${NODE_1}:~/bin/tc3
	scp -i ${KEY} other/testcase/K04-06-TC4 ${NODE_1}:~/bin/tc4
