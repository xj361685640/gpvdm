umf=-I/usr/include/suitesparse/

main: main.c solver.o vec.o
	#gcc main.c solver.o vec.o -o go.o  $(umf) -lumfpack -lm -I/usr/include/openmpi-x86_64/ -lmpi -L/usr/lib64/openmpi/lib
	/usr/lib64/openmpi/bin/mpicc main.c solver.o vec.o -o go.o  $(umf) -lumfpack -lm -I/usr/include/openmpi-x86_64/ -lmpi -L/usr/lib64/openmpi/lib

solver.o: solver.c
	/usr/lib64/openmpi/bin/mpicc -c solver.c -o solver.o

vec.o: vec.c
	/usr/lib64/openmpi/bin/mpicc -c vec.c -o vec.o
