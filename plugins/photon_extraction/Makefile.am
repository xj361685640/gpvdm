OBJS=photon_extraction_plugin.o plugin.o

lib=-L../../
inc=-I../../include/

all: $(OBJS)
	$(CC) -shared $(OBJS) @DLL_LINKER@ $(lib) @DLL_DEBUG@ -o plugin.@DLIB_EXT@
	mv plugin.@DLIB_EXT@ ../photon_extraction.@DLIB_EXT@

.c.o : 
	$(CC) -Wall -g $(inc) $(CPPFLAGS) @DLL_COMPILE@ @COMPILE_FLAG@ -c $< -o $@

clean:
	rm *.o *.dll *.so *.a -f

