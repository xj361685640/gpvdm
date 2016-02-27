plugins:=$(shell ./get_elec_plugins.sh)

platform=linux

DEST_LIB=lib64
CFLAGS=-Werror -Wall -pg -ggdb -g -O2

ifeq ($(platform),"linux")
	SERVER=-D enable_server
else
	SERVER=
endif

DEFINE_FLAGS=-D full_time_domain -D enable_fx -D LONGDOUBLE -D dos_bin -D ${platform} ${SERVER} 

OBJS= solver_interface.o light_utils.o gui_hooks.o sim_find_n0.o sim_run.o newton_update.o dump_map.o dump_energy_slice.o pos.o inital.o advmath.o config.o plot.o timer.o memory.o dos.o gendosfdgaus.o exp.o pl.o time.o fast.o anal.o dump.o dump_config.o dump_1d_slice.o dump_dynamic.o ntricks.o ntricks_externalv.o dos_an.o startstop.o complex_solver.o thermal.o light_interface.o dump_ctrl.o light_dump.o inp.o rand.o buffer.o hard_limit.o epitaxy.o mesh.o patch.o cal_path.o log.o fx.o fit_sin.o newton_interface.o dll_interface.o i.o util.o server.o remesh.o


ifndef OPT_ARCH
	OPT_ARCH=x86_64
endif

LDLIBS=-lumfpack -lcrypto -lm -lgsl -lgslcblas
#LDLIBS= -lefence
inc=


ifeq ($(platform),linux)
	CC=gcc
	LD=ld
	LDLIBS+= -rdynamic -export-dynamic -lblas -ldl -lzip -lz -lmatheval

	ifeq ($(wildcard ~/.gpvdm_hpc_flag),)

		inc+= `pkg-config --cflags dbus-1` -I/usr/include/suitesparse/
		CFLAGS+= -D dbus
		LDLIBS+= `pkg-config --libs dbus-1`	

	else
		    inc+= -I/home/steve/rm/build/libmatheval-1.1.11/lib/
		    LDLIBS+= -L/home/steve/rm/build/libmatheval-1.1.11/lib/.libs/
	endif
else

	CC=i686-w64-mingw32-gcc
	LD=i686-w64-mingw32-ld
	CFLAGS+=-posix
	
	inc+=-I$(HOME)/windll/zlib/include/ -I$(HOME)/windll/openssl-1.0.1j/include/ -I$(HOME)/windll/libzip/libzip-0.11.2/lib/ -I$(HOME)/windll/gsl-1.16/ -I$(HOME)/windll/umfpack/UFconfig/ -I$(HOME)/windll/umfpack/AMD/Include/ -I$(HOME)/windll/umfpack/UMFPACK/Include/
	 LDLIBS+= -L$(HOME)/windll/gsl-1.16/.libs/ -L$(HOME)/windll/gsl-1.16/cblas/.libs/ ./images/res.o -lzip-2
endif

.PHONY: clean

main: main.c $(OBJS)
	./buildplugins.sh "$(CFLAGS)" "$(platform)" "$(CC)" "$(LD)"
	./build_fit_plugins.sh $(platform)
	$(CC) main.c $(OBJS) $(plugins) -o go.o -L.  $(DEFINE_FLAGS) $(inc) $(CFLAGS) $(LDLIBS)

.PHONY: install

%.o : %.c
	$(CC) -c $(DEFINE_FLAGS) $(inc) $(CFLAGS) $(warn) $< -o $@

install:
	
	mkdir $(DESTDIR)/usr
	mkdir $(DESTDIR)/usr/bin
	mkdir $(DESTDIR)/usr/share
	mkdir $(DESTDIR)/usr/share/gpvdm
	mkdir $(DESTDIR)/usr/$(DEST_LIB)
	mkdir $(DESTDIR)/usr/$(DEST_LIB)/gpvdm
	mkdir $(DESTDIR)/usr/$(DEST_LIB)/gpvdm/light
	mkdir $(DESTDIR)/usr/$(DEST_LIB)/gpvdm/solvers

	cp sim.gpvdm $(DESTDIR)/usr/share/gpvdm/
	cp README $(DESTDIR)/usr/share/gpvdm/
	cp ./light/*.so $(DESTDIR)/usr/$(DEST_LIB)/gpvdm/light/
	cp ./solvers/*.so $(DESTDIR)/usr/$(DEST_LIB)/gpvdm/solvers/

	#cp ./exp $(DESTDIR)/usr/share/gpvdm/ -r
	cp plot $(DESTDIR)/usr/share/gpvdm/ -rf

	cp go.o $(DESTDIR)/usr/bin/gpvdm_core


	chmod 755 $(DESTDIR)/usr/bin/gpvdm_core
	chmod 0755 $(DESTDIR)/usr/share/gpvdm/plot
	chmod 0644 $(DESTDIR)/usr/share/gpvdm/plot/*

	#chmod 755 $(DESTDIR)/usr/share/gpvdm/exp -R

	#now install the gui

	mkdir $(DESTDIR)/usr/share/applications
	mkdir $(DESTDIR)/usr/share/gpvdm/gui
	mkdir $(DESTDIR)/usr/share/gpvdm/images
	mkdir $(DESTDIR)/usr/share/mime
	mkdir $(DESTDIR)/usr/share/mime/packages 
	mkdir $(DESTDIR)/usr/share/icons
	mkdir $(DESTDIR)/usr/share/icons/gnome
	mkdir $(DESTDIR)/usr/share/icons/gnome/scalable
	mkdir $(DESTDIR)/usr/share/icons/gnome/scalable/mimetypes

	cp ./images/*.jpg $(DESTDIR)/usr/share/gpvdm/images/
	cp ./images/*.png $(DESTDIR)/usr/share/gpvdm/images/
	cp ./images/*.svg $(DESTDIR)/usr/share/gpvdm/images/
	cp ./gui/*.py $(DESTDIR)/usr/$(DEST_LIB)/gpvdm/
	cp ./gui/gpvdm.desktop $(DESTDIR)/usr/share/applications/
	cp ./gui/gpvdm-gpvdm.xml $(DESTDIR)/usr/share/mime/packages/
	cp ./images/application-gpvdm.svg $(DESTDIR)/usr/share/icons/gnome/scalable/mimetypes/

	cp ./lang $(DESTDIR)/usr/share/gpvdm/ -rf

	cp gpvdm $(DESTDIR)/usr/bin/gpvdm

	#man pages
	mkdir $(DESTDIR)/usr/share/man
	mkdir $(DESTDIR)/usr/share/man/man1
	cp ./man_pages/gpvdm.1.gz $(DESTDIR)/usr/share/man/man1/
	cp ./man_pages/gpvdm_core*.gz $(DESTDIR)/usr/share/man/man1/

	chmod 755 $(DESTDIR)/usr/bin/gpvdm

	chmod 0644 $(DESTDIR)/usr/share/gpvdm/images/image.jpg
	chmod 0644 $(DESTDIR)/usr/share/gpvdm/images/icon.png
	chmod 0644 $(DESTDIR)/usr/share/gpvdm/*.gpvdm
	chmod 0644 $(DESTDIR)/usr/$(DEST_LIB)/gpvdm/*.py
	chmod 0644 $(DESTDIR)/usr/share/applications/gpvdm.desktop
	chmod 0644 $(DESTDIR)/usr/share/mime/packages/gpvdm-gpvdm.xml
	chmod 0644 $(DESTDIR)/usr/share/icons/gnome/scalable/mimetypes/application-gpvdm.svg
	chmod 755 $(DESTDIR)/usr/$(DEST_LIB)/gpvdm/gpvdm.py
	chmod 755 $(DESTDIR)/usr/$(DEST_LIB)/gpvdm/gpvdm_zip.py

	#material files
	cp ./materials $(DESTDIR)/usr/share/gpvdm/ -r
	find $(DESTDIR)/usr/share/gpvdm/materials -type f -exec chmod 644 {} +
	#chmod 0644 $(DESTDIR)/usr/share/gpvdm/materials -R

clean:
	./clean_all.sh

