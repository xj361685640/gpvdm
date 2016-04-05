inc=-I/usr/include/suitesparse/
lib_type=-fPIC
link_command= -shared -Wl,-soname,plugin.so solver.o lib.o -o plugin.so


CFLAGS=-Wall

lib=-I../../include/

all: solver.o lib.o
	$(CC) $(link_command) -lumfpack $(CFLAGS)
	mv plugin.so ../umfpack.so

solver.o: solver.c
	$(CC) $(lib_type) -c solver.c -o solver.o  $(inc) $(CFLAGS) $(lib)

lib.o: lib.c
	$(CC) $(lib_type) -c lib.c -o lib.o  $(inc) $(CFLAGS) $(lib)


clean:
	rm *.o *.dll *.so -f
