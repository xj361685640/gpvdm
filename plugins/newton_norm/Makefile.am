lib_type=-fPIC
link_command=-shared -Wl,-soname,plugin.so plugin.o newton.o -o plugin.so -g
lib=-I../../include/
link=-L../../
CFLAG=-Wall -g

all: plugin.o newton.o
	$(CC) $(link_command) $(CFLAGS) $(link) @LDFLAGS@
	mv plugin.so ../newton_norm.so

plugin.o: plugin.c
	$(CC) $(lib_type) -c plugin.c -o plugin.o $(CFLAGS) $(lib)

newton.o: newton.c
	$(CC) $(lib_type) -c newton.c -o newton.o $(CFLAGS) $(lib) -Wno-unused-but-set-variable -Wno-error=unused-variable


clean:
	rm *.o *.dll *.so -f
