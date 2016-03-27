
platform=linux
#windows

DEST_LIB=lib
#lib64
CFLAGS=-Werror -Wall -ggdb
# -g
#
ifeq ($(platform),linux)
	SERVER=-D enable_server
	CLFAGS+=-O2 -fstack-protector-strong -Wformat -Werror=format-security -Wl,-z,relro
else
	SERVER=
endif

DEFINE_FLAGS=-D full_time_domain -D enable_fx -D LONGDOUBLE -D ${platform} ${SERVER} 

OBJS= solver_interface.o sim_find_n0.o sim_run.o newton_update.o pos.o inital.o config.o memory.o anal.o ntricks.o ntricks_externalv.o complex_solver.o thermal.o mesh.o newton_interface.o dll_interface.o remesh.o run_electrical_dll.c


ifndef OPT_ARCH
	OPT_ARCH=x86_64
endif

LDLIBS=-lumfpack -lcrypto -lm -lgsl -lgslcblas 
#LDLIBS+= -lefence
inc=


ifeq ($(platform),linux)
	CC=gcc
	LD=ld
	LDLIBS+= -rdynamic -export-dynamic -ldl -lzip -lz -lmatheval
	#-pg
	
	#-lblas blas removed for arch

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
	
	inc+=-I$(HOME)/windll/zlib/include/ -I$(HOME)/windll/gsl-1.16/ -I$(HOME)/windll/umfpack/UFconfig/ -I$(HOME)/windll/umfpack/AMD/Include/ -I$(HOME)/windll/umfpack/UMFPACK/Include/
	 LDLIBS+= -L$(HOME)/windll/gsl-1.16/.libs/ -L$(HOME)/windll/gsl-1.16/cblas/.libs/ ./images/res.o -lzip-2
endif

LDLIBS+=-L./lib/ -L./libdump/ -L./liblight/ -L./libmeasure/ -L./libserver/  -L./libdos/ -L./libmesh/ -L./libfit/
LDLIBS+= 
inc+=-I./include/

.PHONY: clean

main: main.c $(OBJS)
	./buildplugins.sh "$(CFLAGS)" "$(platform)" "$(CC)" "$(LD)"
	./build_fit_plugins.sh $(platform)
	$(CC) main.c $(OBJS) -o go.o -L.  $(DEFINE_FLAGS) $(inc) $(CFLAGS) $(LDLIBS) -Wl,--whole-archive -lgpvdm_dos -lgpvdm_lib -lgpvdm_dump -lgpvdm_light  -lgpvdm_measure -lgpvdm_server -lgpvdm_mesh -lgpvdm_fit -Wl,--no-whole-archive

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
	mkdir $(DESTDIR)/usr/share/gpvdm/images
	mkdir $(DESTDIR)/usr/share/mime
	mkdir $(DESTDIR)/usr/share/mime/packages 
	mkdir $(DESTDIR)/usr/share/icons
	mkdir $(DESTDIR)/usr/share/icons/gnome
	mkdir $(DESTDIR)/usr/share/icons/gnome/scalable
	mkdir $(DESTDIR)/usr/share/icons/gnome/scalable/mimetypes
	mkdir $(DESTDIR)/usr/share/gpvdm/device_lib

	cp ./images/*.jpg $(DESTDIR)/usr/share/gpvdm/images/
	cp ./images/*.png $(DESTDIR)/usr/share/gpvdm/images/
	cp ./images/*.svg $(DESTDIR)/usr/share/gpvdm/images/
	cp ./device_lib/* $(DESTDIR)/usr/share/gpvdm/device_lib/
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

