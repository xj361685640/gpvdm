CFLAGS=-Wall -g

COMPILE_FLAG=@COMPILE_FLAG@
LINK_FLAG=@LINK_FLAG@

inc=-I../include/
OBJS=exp.o pl.o

all: $(OBJS)
	$(AR) rcs libgpvdm_measure.a $(OBJS)
	ln -s -f ./libmeasure/libgpvdm_measure.a ../libgpvdm_measure.a
#-cvq

.c.o:
	$(CC) -c $(lib_type) $(COMPILE_FLAG) $< -o $@  $(inc)

clean:
	rm *.o *.dll *.so *.a -f
