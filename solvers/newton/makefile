
ifeq ($(platform),windows)
	CC=i686-w64-mingw32-gcc
	lib_type=-DBUILDING_EXAMPLE_DLL
	link_command=-shared plugin.o newton.o -o plugin.dll -lgpvdm_lib -lgpvdm_measure -posix -Wl,--out-implib,plugin.a
	CFLAGS+=-posix
else
	CC=gcc
	lib_type=-fPIC
	link_command=-shared -lgpvdm_lib -Wl,-soname,plugin.so plugin.o newton.o -o plugin.so -g
endif

CFLAGS+=-Wall -g
lib=-L../../lib/ -L../../libmeasure/
inc=-I../../include/

main: plugin.o newton.o
	$(CC)  $(lib) $(link_command) $(CFLAGS)

plugin.o: plugin.c
	$(CC) $(lib_type) -c plugin.c -o plugin.o  $(inc) $(CFLAGS) 

newton.o: newton.c
	$(CC) $(lib_type) -c newton.c -o newton.o  $(inc) $(CFLAGS)


clean:
	rm *.o *.dll *.so
