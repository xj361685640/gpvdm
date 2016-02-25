inc=-I ../../plugins/i/

ifeq ($(platform),windows)
	CC=i686-w64-mingw32-gcc
	lib_type=-DBUILDING_EXAMPLE_DLL
	link_command=-shared plugin.o newton.o -o plugin.dll -Wl,--out-implib,plugin.a

else
	CC=gcc
	lib_type=-fPIC
	link_command=-shared -Wl,-soname,plugin.so plugin.o newton.o -o plugin.so -g
endif


ifndef opt_normal
	opt_normal=-Wall -g
endif

main: plugin.o newton.o
	$(CC) $(link_command)

plugin.o: plugin.c
	$(CC) $(lib_type) -c plugin.c -o plugin.o  $(inc) $(opt_normal) 

newton.o: newton.c
	$(CC) $(lib_type) -c newton.c -o newton.o  $(inc) $(opt_normal)


clean:
	rm *.o *.dll *.so
