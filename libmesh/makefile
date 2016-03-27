ifndef platform
	platform=linux
endif

CFLAGS=-Wall -g
AROPT=

ifeq ($(platform),windows)
	CC=i686-w64-mingw32-gcc
	AR=i686-w64-mingw32-ar
	CFLAGS+=-posix
	inc+=-I$(HOME)/windll/libzip/libzip-0.11.2/lib/ -D windows
else
	CC=gcc
	AR=ar
endif

inc+=-I../include/
OBJS=time.o

main: $(OBJS)
	$(AR) rcs libgpvdm_mesh.a $(OBJS)
#-cvq

%.o : %.c
	$(CC) -c $(lib_type) $(CFLAGS) $< -o $@  $(inc)

clean:
	rm *.o *.dll *.so *.a
