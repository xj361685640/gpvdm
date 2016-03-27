OBJS=plugin.o pulse.o pulse_voc_ntricks.o

ifeq ($(platform),windows)
	CC=i686-w64-mingw32-gcc
	lib_type=-DBUILDING_EXAMPLE_DLL
	link_command=-shared $(OBJS) -o plugin.dll  -lgpvdm_light -lgpvdm_mesh -lgpvdm_lib -lgpvdm_dump -lgpvdm_measure -lgpvdm_server -lgpvdm_dos -lzip-2 -posix -Wl,--out-implib,plugin.a
	CFLAGS+=-D windows -posix
	lib+=-DBUILDING_EXAMPLE_DLL
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

