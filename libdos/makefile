ifndef platform
	platform=linux
endif

ifeq ($(platform),windows)
	CC=i686-w64-mingw32-gcc
	AR=i686-w64-mingw32-ar
	AROPT+=-posix
	inc+= -D windows
else
	CC=gcc
	AR=ar
endif

CFLAGS=-Wall -g -D dos_bin
AROPT=

inc+=-I../include/
OBJS=dos_an.o dos.o gendosfdgaus.o checksum.o hash.o

main: $(OBJS)
	$(AR) rcs libgpvdm_dos.a $(OBJS)
#-cvq

%.o : %.c
	$(CC) -c $(lib_type) $(CFLAGS) -posix $< -o $@  $(inc)

clean:
	rm *.o *.dll *.so *.a
