OBJS=plugin.o equilibrium.o

lib=-L../../
inc=-I../../include/

all: $(OBJS)
	$(CC) -shared $(OBJS) @DLL_LINKER@ $(lib) @DLL_DEBUG@ -o plugin.@DLIB_EXT@
	mv plugin.@DLIB_EXT@ ../equilibrium.@DLIB_EXT@

.c.o : 
	$(CC) -Wall -g $(inc) @DLL_COMPILE@ @COMPILE_FLAG@ -Wno-error=unused-but-set-variable -Wno-error=unused-variable -c $< -o $@

clean:
	rm *.o *.dll *.so -f

