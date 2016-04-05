CFLAGS=-Wall -g
AROPT=

COMPILE_FLAG=@COMPILE_FLAG@
LINK_FLAG=@LINK_FLAG@

inc=-I../include/
OBJS=time.o

all: $(OBJS)
	$(AR) rcs libgpvdm_mesh.a $(OBJS)
#-cvq

%.o : %.c
	$(CC) -c $(lib_type) $(CFLAGS) $(COMPILE_FLAG) $< -o $@  $(inc)

clean:
	rm *.o *.dll *.so *.a -f
