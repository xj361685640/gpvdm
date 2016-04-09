OBJS=plugin.o fxdomain.o fxdomain_voc_ntricks.o fx.o fit_sin.o

CFLAGS=-fPIC
link_command=-shared -Wl,-soname,plugin.so $(OBJS) -o plugin.so -g

CFLAGS+= -Wall -g
lib=-L../../ -L$(HOME)/windll/compiled_dlls/
inc=-I../../include/

all: $(OBJS)
	$(CC) $(link_command) $(CFLAGS) $(lib) @LDFLAGS@
	mv plugin.so ../fxdomain.so

%.o : %.c
	$(CC) $(inc) $(CFLAGS) -Wno-error=unused-but-set-variable -Wno-error=unused-variable -c $< -o $@

clean:
	rm *.o *.dll *.so -f
