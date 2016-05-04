AUTOMAKE_OPTIONS = foreign
EXTRA_DIST = materials
SUBDIRS = @BUILD_DIRS@

bin_PROGRAMS=gpvdm_core
gpvdm_core_SOURCES=

bin_SCRIPTS = gui/gpvdm


@HPC@


imagedir = $(pkgdatadir)/images
image_DATA=images/*.jpg images/*.png


dist_pkgdata_DATA = sim.gpvdm

device_libdir = $(pkgdatadir)/device_lib
device_lib_DATA=device_lib/*.gpvdm

plotdir = $(pkgdatadir)/plot
plot_DATA=plot/*.plot

pluginsdir = $(pkglibdir)/plugins
plugins_DATA=plugins/*.@DLIB_EXT@

pythondir = $(pkgdatadir)/gui
python_PYTHON=gui/*.py

dist_doc_DATA = README

appicondir=$(pkgdatadir)/images
appicon_DATA=$(srcdir)/desktop/application-gpvdm.svg

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
	find lang -name "*.mo" -exec cp --parents {} $(DESTDIR)$(datadir)/gpvdm/ ";"


