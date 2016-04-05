lib_type=-fPIC
link_command=-shared -lgpvdm_lib -Wl,-soname,plugin.so plugin.o newton.o -o plugin.so -g


CFLAGS=-Wall -g
lib=-L../../lib/ -L../../libmeasure/
inc=-I../../include/

all: plugin.o newton.o
	$(CC)  $(lib) $(link_command) $(CFLAGS)
	mv plugin.so ../newton.so

plugin.o: plugin.c
	$(CC) $(lib_type) -c plugin.c -o plugin.o  $(inc) $(CFLAGS) 

newton.o: newton.c
	$(CC) $(lib_type) -c newton.c -o newton.o  $(inc) $(CFLAGS)


clean:
	rm *.o *.dll *.so -f
