inc=-I../include/
OBJS=log.o util.o advmath.o buffer.o i.o inp.o rand.o cal_path.o list.o vec.o patch.o timer.o gui_hooks.o startstop.o hard_limit.o epitaxy.o exit.o plot.o device.o 

COMPILE_FLAG=@COMPILE_FLAG@
LINK_FLAG=@LINK_FLAG@

all: $(OBJS)
	$(AR) cr libgpvdm_lib.@SLIB_EXT@ $(OBJS)
	ln -s -f ./lib/libgpvdm_lib.@SLIB_EXT@ ../libgpvdm_lib.@SLIB_EXT@
#-cvq

.c.o:
	$(CC) -c $(CPPFLAGS) $(COMPILE_FLAG) $(inc) $< -o $@  

clean:
	rm *.o *.dll *.so *.a *.lib -f
