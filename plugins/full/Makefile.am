lib_type=-fPIC
link_command=-shared -Wl,-soname,plugin.so interface.o light.o -o plugin.so
CFLAGS=-Wall
inc=-I../../include/
link=-L../../

all: light.o interface.o
	$(CC) $(link_command) $(CFLAGS) $(link) @LDFLAGS@
	mv plugin.so ../full.so

interface.o: interface.c
	$(CC) $(lib_type) -c interface.c -o interface.o  $(inc) $(CFLAGS)

light.o: light.c
	$(CC) $(lib_type) -c light.c -o light.o  $(inc) $(CFLAGS) -Wno-unused-but-set-variable


clean:
	rm *.o *.dll *.so -f
