OBJS=plugin.o fxdomain.o fx.o fit_sin.o

CFLAGS=-fPIC
link_command=-shared -Wl,-soname,plugin.so $(OBJS) -o plugin.@DLIB_EXT@ -g

CFLAGS+= -Wall -g
lib=-L../../
inc=-I../../include/

all: $(OBJS)
	$(CC) -shared $(OBJS) @DLL_LINKER@ $(lib) @DLL_DEBUG@ -o plugin.@DLIB_EXT@ -lgsl -lgslcblas
	mv plugin.@DLIB_EXT@ ../fx_domain.@DLIB_EXT@

.c.o : 
	$(CC) -Wall -g $(inc) $(CPPFLAGS) @DLL_COMPILE@ @COMPILE_FLAG@ -c $< -o $@

clean:
	rm *.o *.dll *.so -f
