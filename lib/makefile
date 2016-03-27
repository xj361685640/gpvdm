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
OBJS=log.o util.o advmath.o buffer.o i.o inp.o rand.o cal_path.o list.o vec.o patch.o timer.o gui_hooks.o startstop.o hard_limit.o epitaxy.o exit.o plot.o device.o 

main: $(OBJS)
	$(AR) cr libgpvdm_lib.a $(OBJS)
#-cvq

%.o : %.c
	$(CC) -c $(lib_type) $(CFLAGS) $(inc) $< -o $@  

clean:
	rm *.o *.dll *.so *.a
