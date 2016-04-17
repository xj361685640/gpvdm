OBJS=interface.o light.o
CFLAGS=-Wall
inc=-I../../include/
lib=-L../../

all: $(OBJS)
	$(CC) -shared $(OBJS) @DLL_LINKER@ $(lib) @DLL_DEBUG@ $(LDFLAGS) -o plugin.@DLIB_EXT@
	mv plugin.@DLIB_EXT@ ../light_flat.@DLIB_EXT@

.c.o : 
	$(CC) $(inc) $(CFLAGS) @DLL_COMPILE@ @COMPILE_FLAG@ -c $< -o $@ -Wno-unused-but-set-variable

clean:
	rm *.o *.dll *.so -f
