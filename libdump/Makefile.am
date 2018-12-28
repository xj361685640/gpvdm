CFLAGS=-Wall -g

COMPILE_FLAG=@COMPILE_FLAG@
LINK_FLAG=@LINK_FLAG@

inc=-I../include/

OBJS=dump_1d_slice.o dump.o dump_snapshot_utils.o dump_config.o dump_ctrl.o dump_dynamic.o dump_energy_slice.o dump_map.o buffer.o dump_contacts.o cache.o

all: $(OBJS)
	$(AR) rcs libgpvdm_dump.@SLIB_EXT@ $(OBJS)
	ln -s -f ./libdump/libgpvdm_dump.@SLIB_EXT@ ../libgpvdm_dump.@SLIB_EXT@
#-cvq

.c.o :
	$(CC) -c $(lib_type) $(CPPFLAGS) $(COMPILE_FLAG) $< -o $@  $(inc)

clean:
	rm *.o *.dll *.so *.a *.lib -f
