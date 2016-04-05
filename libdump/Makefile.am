CFLAGS=-Wall -g

COMPILE_FLAG=@COMPILE_FLAG@
LINK_FLAG=@LINK_FLAG@

inc=-I../include/

OBJS=dump_1d_slice.o dump.o dump_config.o dump_ctrl.o dump_dynamic.o dump_energy_slice.o dump_map.o

all: $(OBJS)
	$(AR) rcs libgpvdm_dump.a $(OBJS)
#-cvq

%.o : %.c
	$(CC) -c $(lib_type) $(CFLAGS) $(COMPILE_FLAG) $< -o $@  $(inc)

clean:
	rm *.o *.dll *.so *.a -f
