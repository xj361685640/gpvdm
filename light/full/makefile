ifeq ($(platform),windows)
	CC=i686-w64-mingw32-gcc
	lib_type=-DBUILDING_EXAMPLE_DLL
	link_command=-shared interface.o light.o -o plugin.dll -Wl,--out-implib,plugin.a

else
	CC=gcc
	lib_type=-fPIC
	link_command=-shared -Wl,-soname,plugin.so interface.o light.o -o plugin.so
endif


CFLAGS=-Wall


main: light.o interface.o
	$(CC) $(link_command) $(CFLAGS)

interface.o: interface.c
	$(CC) $(lib_type) -c interface.c -o interface.o  $(inc) $(CFLAGS)

light.o: light.c
	$(CC) $(lib_type) -c light.c -o light.o  $(inc) $(CFLAGS) -Wno-unused-but-set-variable


clean:
	rm *.o *.dll *.so
