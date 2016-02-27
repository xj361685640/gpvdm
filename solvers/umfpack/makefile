inc=-I/usr/include/suitesparse/ -I ../../plugins/i/

ifeq ($(platform),windows)
	CC=i686-w64-mingw32-gcc
	lib_type=-DBUILDING_EXAMPLE_DLL
	link_command=-shared solver.o lib.o -o plugin.dll -Wl,--out-implib,plugin.a

else
	CC=gcc
	lib_type=-fPIC
	link_command=-shared -Wl,-soname,plugin.so solver.o lib.o -o plugin.so
endif


ifndef opt_normal
	opt_normal=-Wall
endif

main: solver.o lib.o
	$(CC) $(link_command) -lumfpack

solver.o: solver.c
	$(CC) $(lib_type) -c solver.c -o solver.o  $(inc) $(opt_normal)

lib.o: lib.c
	$(CC) $(lib_type) -c lib.c -o lib.o  $(inc) $(opt_normal)


clean:
	rm *.o *.dll *.so
