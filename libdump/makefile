ifndef platform
	platform=linux
endif

CFLAGS=-Wall -g
AROPT=

ifeq ($(platform),windows)
	CC=i686-w64-mingw32-gcc
	AR=i686-w64-mingw32-ar
	CFLAGS+=-posix
	inc+=-D windows
else
	CC=gcc
	AR=ar
endif

inc+=-I../include/

OBJS=dump_1d_slice.o dump.o dump_config.o dump_ctrl.o dump_dynamic.o dump_energy_slice.o dump_map.o

main: $(OBJS)
	$(AR) rcs libgpvdm_dump.a $(OBJS)
#-cvq

%.o : %.c
	$(CC) -c $(lib_type) $(CFLAGS) $< -o $@  $(inc)

clean:
	rm *.o *.dll *.so *.a
