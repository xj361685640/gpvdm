CC=gcc
lib_type=-fPIC
HELP2MAN=help2man

CFLAGS=-Wall -g
lib=-L../../lib/ -L../../libmeasure/
inc=-I../../include/

all: gpvdm.1 gpvdm_core.1
	echo "Man page built"


gpvdm.1: ../gpvdm
	-$(HELP2MAN) --output=gpvdm.1 --name="GUI for gpvdm_core, a solar cell device model" ../gpvdm

gpvdm_core.1: ../go.o
	-$(HELP2MAN) --output=gpvdm_core.1 --name="A solar cell device model" ../go.o

clean:
	rm *.1 -f

