CFLAGS=-Wall -g
AROPT=

COMPILE_FLAG=@COMPILE_FLAG@
LINK_FLAG=@LINK_FLAG@

inc=-I../include/

OBJS=fdtd.o

all: $(OBJS)
	$(AR) rcs libgpvdm_fdtd.@SLIB_EXT@ $(OBJS)
	ln -s -f ./libfdtd/libgpvdm_fdtd.@SLIB_EXT@ ../libgpvdm_fdtd.@SLIB_EXT@
#-cvq

.c.o : 
	$(CC) -c $(lib_type) $(CFLAGS) $(COMPILE_FLAG) $< -o $@  $(inc)

clean:
	rm *.o *.dll *.so *.a *.lib -f
