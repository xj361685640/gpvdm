AUTOMAKE_OPTIONS = foreign
EXTRA_DIST = materials
SUBDIRS = @BUILD_DIRS@

bin_PROGRAMS=gpvdm_core
gpvdm_core_SOURCES=src 

lib_LIBRARIES = libgpvdm_lib.a libgpvdm_measure.a libgpvdm_dump.a libgpvdm_light.a libgpvdm_fit.a libgpvdm_server.a libgpvdm_mesh.a libgpvdm_dos.a
man1_MANS = man/gpvdm.1 man/gpvdm_core.1

imagedir = $(pkgdatadir)/images
image_DATA=images/*.jpg images/*.png


dist_pkgdata_DATA = sim.gpvdm

device_libdir = $(pkgdatadir)/device_lib
device_lib_DATA=device_lib/*.gpvdm

plotdir = $(pkgdatadir)/plot
plot_DATA=plot/*.plot

pluginsdir = $(pkgdatadir)/plugins
plugins_DATA=plugins/*.so

guidir = $(pkgdatadir)/gui
gui_DATA=gui/*.py

dist_doc_DATA = README COPYING

libgpvdm_lib_a_SOURCES=lib
libgpvdm_measure_a_SOURCES=libmeasure
libgpvdm_dump_a_SOURCES=libdump
libgpvdm_light_a_SOURCES=liblight
libgpvdm_fit_a_SOURCES=libfit
libgpvdm_server_a_SOURCES=libserver
libgpvdm_mesh_a_SOURCES=libmesh
libgpvdm_dos_a_SOURCES=libdos
man_gpvdm.1_SOURCES=man
man_gpvdm_core.1_SOURCES=man

CFLAGS=-D dbus -D roderick

clean-local:
	rm *.dat *.o ./plugins/*.so ./plugins/*.dll *.log *.a -f
	rm autom4te.cache -rf 

install-data-hook:
	cp materials $(DESTDIR)$(datadir)/gpvdm/materials -rf
	find lang -name "*.mo" -exec cp --parents {} $(DESTDIR)$(datadir)/gpvdm/ ";"

	mkdir --parents $(DESTDIR)/usr/share/applications
	cp --parents ./desktop/gpvdm.desktop $(DESTDIR)/usr/share/applications

	mkdir --parents $(DESTDIR)/usr/share/mime/packages
	cp --parents ./desktop/gpvdm-gpvdm.xml $(DESTDIR)/usr/share/mime/packages

	mkdir --parents $(DESTDIR)/usr/share/icons/gnome/scalable/mimetypes
	cp --parents ./desktop/application-gpvdm.svg $(DESTDIR)/usr/share/icons/gnome/scalable/mimetypes

	echo chmod 755 $(DESTDIR)$(datadir)/gpvdm/gui/gpvdm.py
	ln -s -f $(datadir)/gpvdm/gui/gpvdm.py $(DESTDIR)$(bindir)/gpvdm


