OBJS=plugin.o jv.o jv_ntricks.o
CFLAGS=-fPIC
link_command=-shared -Wl,-soname,plugin.so $(OBJS) -o plugin.so -g

CFLAGS+= -Wall -g
lib=-L../../lib/ -L../../liblight/ -L../../libdump/ -L../../libmeasure/  -L../../libserver/ -L../../libdos/ -L$(HOME)/windll/compiled_dlls/
inc=-I../../include/

all: $(OBJS)
	$(CC) $(link_command) $(CFLAGS) $(lib)
	mv plugin.so ../jv.so

%.o : %.c
	$(CC) $(inc) $(CFLAGS) -Wno-error=unused-but-set-variable -Wno-error=unused-variable -c $< -o $@

clean:
	rm *.o *.dll *.so -f

