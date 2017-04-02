AUTOMAKE_OPTIONS = foreign
EXTRA_DIST = config.rpath m4/ChangeLog  materials
SUBDIRS = @BUILD_DIRS@

bin_PROGRAMS=gpvdm_core
gpvdm_core_SOURCES=

if INCLUDE_IMAGES
imagedir = $(pkgdatadir)/images
image_DATA = images/*.jpg images/*.png

image16dir = $(pkgdatadir)/images/16x16
image16_DATA = images/16x16/*.png

image32dir = $(pkgdatadir)/images/32x32
image32_DATA = images/32x32/*.png

image64dir = $(pkgdatadir)/images/64x64
image64_DATA = images/64x64/*.png

flagdir = $(pkgdatadir)/flags
flag_DATA = flags/*.png

cssdir = $(pkgdatadir)/css
css_DATA = css/*.css
endif

dist_pkgdata_DATA = sim.gpvdm

device_libdir = $(pkgdatadir)/device_lib
device_lib_DATA=device_lib/*.gpvdm

if INCLUDE_PLOTS
plotdir = $(pkgdatadir)/plot
plot_DATA=plot/*.plot
endif

pluginsdir = $(pkglibdir)/plugins
plugins_DATA=plugins/*.@DLIB_EXT@

pythondir = $(pkgdatadir)/gui
python_PYTHON=gui/*.py

dist_doc_DATA = README

if INCLUDE_MAN
man1_MANS = man/gpvdm.1 man/gpvdm_core.1
endif

if INCLUDE_IMAGES
appicondir=$(pkgdatadir)/images
appicon_DATA=$(srcdir)/desktop/application-gpvdm.svg
endif

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

ACLOCAL_AMFLAGS = -I m4
