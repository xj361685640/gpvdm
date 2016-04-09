CFLAGS=-Wall -g -D dos_bin
AROPT=

COMPILE_FLAG=@COMPILE_FLAG@
LINK_FLAG=@LINK_FLAG@

inc=-I../include/
OBJS=dos_an.o dos.o gendosfdgaus.o checksum.o hash.o

all: $(OBJS)
	$(AR) rcs libgpvdm_dos.a $(OBJS)
	ln -s -f ./libdos/libgpvdm_dos.a ../libgpvdm_dos.a
#-cvq

%.o : %.c
	$(CC) -c $(lib_type) $(CFLAGS) $(COMPILE_FLAG) $< -o $@  $(inc)

clean:
	rm *.o *.dll *.so *.a -f

