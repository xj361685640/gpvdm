
ifeq ($(platform),windows)
	CC=i686-w64-mingw32-gcc
	lib_type=-DBUILDING_EXAMPLE_DLL
	link_command=-shared plugin.o newton.o -o plugin.dll -lgpvdm_lib -lgpvdm_measure  -Wl,--out-implib,plugin.a
	CFLAGS+= -posix
else
	CC=gcc
	lib_type=-fPIC
	link_command=-shared -Wl,-soname,plugin.so plugin.o newton.o -o plugin.so -g
endif

lib+=-I../../include/
link+=-L../../lib/ -L../../libmeasure/

CFLAG+=-Wall -g

main: plugin.o newton.o
	$(CC) $(link_command) $(CFLAGS) $(link)

plugin.o: plugin.c
	$(CC) $(lib_type) -c plugin.c -o plugin.o $(CFLAGS) $(lib)

newton.o: newton.c
	$(CC) $(lib_type) -c newton.c -o newton.o $(CFLAGS) $(lib) -Wno-unused-but-set-variable -Wno-error=unused-variable


clean:
	rm *.o *.dll *.so
