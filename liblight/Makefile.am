CFLAGS=-Wall -g
AROPT=

COMPILE_FLAG=@COMPILE_FLAG@
LINK_FLAG=@LINK_FLAG@

inc=-I../include/

OBJS=light_config.o light_dump.o light_interface.o light_laser.o light_materials.o light_memory.o light_mesh.o light_utils.o

all: $(OBJS)
	$(AR) rcs libgpvdm_light.@SLIB_EXT@ $(OBJS)
	ln -s -f ./liblight/libgpvdm_light.@SLIB_EXT@ ../libgpvdm_light.@SLIB_EXT@
#-cvq

.c.o : 
	$(CC) -c $(lib_type) $(CFLAGS) $(COMPILE_FLAG) $< -o $@  $(inc)

clean:
	rm *.o *.dll *.so *.a *.lib -f
