inc=-I/usr/include/suitesparse/

ifeq ($(platform),windows)
	CC=i686-w64-mingw32-gcc
	lib_type=-DBUILDING_EXAMPLE_DLL
	link_command=-L$(HOME)/windll/compiled_dlls/ -shared solver.o lib.o -o plugin.dll -Wl,--out-implib,plugin.a

else
	CC=gcc
	lib_type=-fPIC
	link_command= -shared -Wl,-soname,plugin.so solver.o lib.o -o plugin.so
endif


CFLAGS=-Wall


main: solver.o lib.o
	$(CC) $(link_command) -lumfpack $(CFLAGS)

solver.o: solver.c
	$(CC) $(lib_type) -c solver.c -o solver.o  $(inc) $(CFLAGS)

lib.o: lib.c
	$(CC) $(lib_type) -c lib.c -o lib.o  $(inc) $(CFLAGS)


clean:
	rm *.o *.dll *.so
