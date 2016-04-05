inc=-I../include/
OBJS=server.o

COMPILE_FLAG=@COMPILE_FLAG@
LINK_FLAG=@LINK_FLAG@

all: $(OBJS)
	$(AR) rcs libgpvdm_server.a $(OBJS)
#-cvq

%.o : %.c
	$(CC) -c $(lib_type) $(COMPILE_FLAG) $< -o $@  $(inc)

clean:
	rm *.o *.dll *.so *.a -f
