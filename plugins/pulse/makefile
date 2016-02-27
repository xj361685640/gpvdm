inc=-I ../i/

CFLAGS=-Wall

ifndef platform
	platform=linux
endif

main: pulse.o pulse_voc_ntricks.o
	$(LD) -r pulse.o pulse_voc_ntricks.o -o plugin.o

pulse.o: pulse.c
	$(CC) -c pulse.c -o pulse.o  $(inc) $(CFLAGS) -D $(platform)


pulse_voc_ntricks.o: pulse_voc_ntricks.c
	$(CC) -c pulse_voc_ntricks.c -o pulse_voc_ntricks.o  $(inc) $(CFLAGS) -D $(platform)

clean:
	rm *.o
