inc=-I ../i/

CFLAGS=-Wall

ifndef platform
	platform=linux
endif

main: jv.o
	$(LD) -r jv.o -o plugin.o

jv.o: jv.c
	$(CC) -c jv.c  $(inc) -I ../../ -o jv.o $(CFLAGS) -D $(platform)

clean:
	rm *.o
