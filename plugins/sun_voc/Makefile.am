OBJS=plugin.o sun_voc.o
lib=-L../../
inc=-I../../include/

all: $(OBJS)
	$(CC) -shared $(OBJS) @DLL_LINKER@ $(lib) @DLL_DEBUG@ $(LDFLAGS) -o plugin.@DLIB_EXT@
	mv plugin.@DLIB_EXT@ ../sun_voc.@DLIB_EXT@

.c.o : 
	$(CC) -Wall -g $(inc) $(CPPFLAGS) @DLL_COMPILE@ @COMPILE_FLAG@ -c $< -o $@

clean:
	rm *.o *.dll *.so -f

