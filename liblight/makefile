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

OBJS=light_config.o light_dump.o light_interface.o light_laser.o light_materials.o light_memory.o light_mesh.o light_utils.o

main: $(OBJS)
	$(AR) rcs libgpvdm_light.a $(OBJS)
#-cvq

%.o : %.c
	$(CC) -c $(lib_type) $(CFLAGS) $< -o $@  $(inc)

clean:
	rm *.o *.dll *.so *.a
