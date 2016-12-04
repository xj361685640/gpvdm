AUTOMAKE_OPTIONS = foreign
EXTRA_DIST = materials
SUBDIRS = @BUILD_DIRS@

bin_PROGRAMS=gpvdm_core
gpvdm_core_SOURCES=


@IMAGES2@
@IMAGES3@

dist_pkgdata_DATA = sim.gpvdm

device_libdir = $(pkgdatadir)/device_lib
device_lib_DATA=device_lib/*.gpvdm

@PLOT0@
@PLOT1@

pluginsdir = $(pkglibdir)/plugins
plugins_DATA=plugins/*.@DLIB_EXT@

pythondir = $(pkgdatadir)/gui
python_PYTHON=gui/*.py

dist_doc_DATA = README

@MAN_PAGES@

@IMAGES0@
@IMAGES1@

desktopdir = $(datadir)/applications
desktop_DATA = $(srcdir)/desktop/gpvdm.desktop

packagesdir = $(datadir)/mime/packages
packages_DATA = $(srcdir)/desktop/gpvdm-gpvdm.xml

clean-local:
	rm gpvdm_core *.dat *.o ./plugins/*.so ./plugins/*.dll *.log *.a  *.lib *.exe *.chk -f
	rm autom4te.cache -rf 

gpvdm_core$(EXEEXT):
	@printf ""


install-data-hook:
	cp materials $(DESTDIR)$(datadir)/gpvdm/materials -rf
	cp ui $(DESTDIR)$(datadir)/gpvdm/ui -rf
	find lang -name "*.mo" -exec cp --parents {} $(DESTDIR)$(datadir)/gpvdm/ ";"
	ln -s $(pkgdatadir)/gui/gpvdm.py $(DESTDIR)$(bindir)/gpvdm
	chmod +x $(DESTDIR)$(pkgdatadir)/gui/gpvdm.py
