ifndef platform
	platform=linux
endif

CFLAGS=-Wall -g
AROPT=

ifeq ($(platform),windows)
	CC=i686-w64-mingw32-gcc
	AR=i686-w64-mingw32-ar
	AROPT+=-posix
	inc+= -D windows
else
	CC=gcc
	AR=ar
endif

inc+=-I../include/
OBJS=exp.o pl.o

main: $(OBJS)
	$(AR) rcs libgpvdm_measure.a $(OBJS)
#-cvq

%.o : %.c
	$(CC) -c $(lib_type) $< -o $@  $(inc)

clean:
	rm *.o *.dll *.so *.a
