inc=-I../include/
OBJS=memory.o memory_basic.o

COMPILE_FLAG=@COMPILE_FLAG@
LINK_FLAG=@LINK_FLAG@

all: $(OBJS)
	$(AR) cr libgpvdm_memory.@SLIB_EXT@ $(OBJS)
	ln -s -f ./libmemory/libgpvdm_memory.@SLIB_EXT@ ../libgpvdm_memory.@SLIB_EXT@
#-cvq

.c.o:
	$(CC) -c $(CPPFLAGS) $(COMPILE_FLAG) $(inc) $< -o $@  

clean:
	rm *.o *.dll *.so *.a *.lib -f
