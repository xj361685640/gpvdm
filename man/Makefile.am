HELP2MAN=help2man

lib=-L../../
inc=-I../../include/

all: gpvdm.1 gpvdm_core.1
	echo "Man page built"


gpvdm.1: ../gpvdm
	cd ..; $(HELP2MAN) --output=./man/gpvdm.1 --name="GUI for gpvdm_core, a solar cell device model" ./gpvdm

gpvdm_core.1: ../gpvdm_core@EXE_EXT@
	cd ..; $(HELP2MAN) --output=./man/gpvdm_core.1 --name="A solar cell device model" ./gpvdm_core@EXE_EXT@

clean:
	rm *.1 -f

