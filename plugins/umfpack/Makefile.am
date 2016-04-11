OBJS=solver.o lib.o
CFLAGS=-Wall
inc=-I../../include/
lib=-L../../

all: $(OBJS)
	$(CC) -shared $(OBJS) @DLL_LINKER@ $(lib) @DLL_DEBUG@ @LDFLAGS@ -o plugin.@DLIB_EXT@
	mv plugin.@DLIB_EXT@ ../umfpack.@DLIB_EXT@

.c.o : 
	$(CC) -Wall -g $(inc) $(CPPFLAGS) @DLL_COMPILE@ @COMPILE_FLAG@ -c $< -o $@

clean:
	rm *.o *.dll *.so -f
