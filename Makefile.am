AUTOMAKE_OPTIONS = foreign
EXTRA_DIST = materials
SUBDIRS = @BUILD_DIRS@

bin_PROGRAMS=gpvdm_core
gpvdm_core_SOURCES=src 

bin_SCRIPTS = gui/gpvdm

man1_MANS = man/gpvdm.1 man/gpvdm_core.1

imagedir = $(pkgdatadir)/images
image_DATA=images/*.jpg images/*.png


dist_pkgdata_DATA = sim.gpvdm

device_libdir = $(pkgdatadir)/device_lib
device_lib_DATA=device_lib/*.gpvdm

plotdir = $(pkgdatadir)/plot
plot_DATA=plot/*.plot

pluginsdir = $(pkglibdir)/plugins
plugins_DATA=plugins/*.so

pythondir = $(pkgdatadir)/gui
python_PYTHON=gui/*.py

dist_doc_DATA = README

CFLAGS=-D dbus -D roderick

clean-local:
	rm *.dat *.o ./plugins/*.so ./plugins/*.dll *.log *.a -f
	rm autom4te.cache -rf 

#ln -s -f $(datadir)/gpvdm/gui/gpvdm.py $(DESTDIR)$(bindir)/gpvdm
#chmod 755 $(DESTDIR)$(datadir)/gpvdm/gui/gpvdm.py
#ln -s $(DESTDIR)$(datadir)/gpvdm/gui/gpvdm.py


install-data-hook:
	cp materials $(DESTDIR)$(datadir)/gpvdm/materials -rf
	find lang -name "*.mo" -exec cp --parents {} $(DESTDIR)$(datadir)/gpvdm/ ";"

	mkdir --parents $(DESTDIR)/usr/share/applications
	cp --parents ./desktop/gpvdm.desktop $(DESTDIR)/usr/share/applications

	mkdir --parents $(DESTDIR)/usr/share/mime/packages
	cp --parents ./desktop/gpvdm-gpvdm.xml $(DESTDIR)/usr/share/mime/packages

	mkdir --parents $(DESTDIR)/usr/share/icons/gnome/scalable/mimetypes
	cp --parents ./desktop/application-gpvdm.svg $(DESTDIR)/usr/share/icons/gnome/scalable/mimetypes


