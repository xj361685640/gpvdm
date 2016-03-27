OBJS=plugin.o fxdomain.o fxdomain_voc_ntricks.o fx.o fit_sin.o

ifeq ($(platform),windows)
	CC=i686-w64-mingw32-gcc
	link_command=-shared $(OBJS) -o plugin.dll  -lgpvdm_light -lgpvdm_lib -lgpvdm_dump -lgpvdm_measure -lgpvdm_server -lgpvdm_dos -lgpvdm_mesh -lgsl -lgslcblas -lzip-2 -posix -Wl,--out-implib,plugin.a
	CFLAGS+=-D windows -posix
	lib+= -L$(HOME)/windll/gsl-1.16/.libs/ -L$(HOME)/windll/gsl-1.16/cblas/.libs/ -DBUILDING_EXAMPLE_DLL
	inc+= -I$(HOME)/windll/gsl-1.16/
else
	CC=gcc
	CFLAGS+=-fPIC
	link_command=-shared -Wl,-soname,plugin.so $(OBJS) -o plugin.so -g
endif

CFLAGS+= -Wall -g
lib+=-L../../lib/ -L../../libmesh/ -L../../liblight/ -L../../libdump/ -L../../libmeasure/  -L../../libserver/ -L../../libdos/ -L$(HOME)/windll/compiled_dlls/
inc+=-I../../include/

main: $(OBJS)
	$(CC) $(link_command) $(CFLAGS) $(lib)

%.o : %.c
	$(CC) $(inc) $(CFLAGS) -Wno-error=unused-but-set-variable -Wno-error=unused-variable -c $< -o $@

clean:
	rm *.o *.dll *.so
