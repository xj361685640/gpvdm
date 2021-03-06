AUTOMAKE_OPTIONS = foreign
EXTRA_DIST = config.rpath m4/ChangeLog  materials
SUBDIRS = @BUILD_DIRS@

bin_PROGRAMS=gpvdm_core
gpvdm_core_SOURCES=

if INCLUDE_HTML
htmlsrcdir = $(pkgdatadir)/html
htmlsrc_DATA = html/*.html

endif

if INCLUDE_IMAGES
imagedir = $(pkgdatadir)/images
image_DATA = images/*.jpg images/*.png  images/*.ico

image16dir = $(pkgdatadir)/images/16x16
image16_DATA = images/16x16/*.png

image32dir = $(pkgdatadir)/images/32x32
image32_DATA = images/32x32/*.png

image64dir = $(pkgdatadir)/images/64x64
image64_DATA = images/64x64/*.png

splashdir = $(pkgdatadir)/images/splash
splash_DATA = images/splash/*.png

flagdir = $(pkgdatadir)/images/48x32
flag_DATA = images/48x32/*.png

icon16dir = $(datadir)/icons/hicolor/16x16/mimetypes
icon16_DATA = images/icons/16x16/*.png
icon32dir = $(datadir)/icons/hicolor/32x32/mimetypes
icon32_DATA = images/icons/32x32/*.png
icon48dir = $(datadir)/icons/hicolor/48x48/mimetypes
icon48_DATA = images/icons/48x48/*.png
icon64dir = $(datadir)/icons/hicolor/64x64/mimetypes
icon64_DATA = images/icons/64x64/*.png
icon128dir = $(datadir)/icons/hicolor/128x128/mimetypes
icon128_DATA = images/icons/128x128/*.png
icon256dir = $(datadir)/icons/hicolor/256x256/mimetypes
icon256_DATA = images/icons/256x256/*.png
icon512dir = $(datadir)/icons/hicolor/512x512/mimetypes
icon512_DATA = images/icons/512x512/*.png

iconsdir=$(datadir)/icons/hicolor/scalable/mimetypes
icons_DATA=$(srcdir)/images/icons/scalable/*.svg

cssdir = $(pkgdatadir)/css
css_DATA = css/*.css

endif



dist_pkgdata_DATA = base.gpvdm

if INCLUDE_DEVICELIB
device_libdir = $(pkgdatadir)/device_lib
device_lib_DATA=device_lib/*.gpvdm

device_lib_maeckel_et_aldir = $(pkgdatadir)/device_lib/maeckel_et_al/
device_lib_maeckel_et_al_DATA=device_lib/maeckel_et_al/*.gpvdm
endif

if INCLUDE_CLUSTER
clusterdir = $(pkgdatadir)/cluster_
cluster_DATA=cluster_/*.zip
endif

if INCLUDE_PLOTS
plotdir = $(pkgdatadir)/plot
plot_DATA=plot/*.plot
endif

pluginsdir = $(pkglibdir)/plugins
plugins_DATA=plugins/*.@DLIB_EXT@

pythondir = $(pkgdatadir)/gui
python_PYTHON=gui/*.py gui/*.bat

dist_doc_DATA = README.md

if INCLUDE_DOCS
docsdir = $(docdir)
docs_DATA = docs/gpvdm.pdf
endif

if INCLUDE_MAN
man1_MANS = man/gpvdm.1 man/gpvdm_core.1
endif

if INCLUDE_DESKTOP
appicondir=$(pkgdatadir)/images
appicon_DATA=$(srcdir)/desktop/application-gpvdm.svg

desktopdir = $(datadir)/applications
desktop_DATA = $(srcdir)/desktop/gpvdm.desktop

packagesdir = $(datadir)/mime/packages
packages_DATA = $(srcdir)/desktop/gpvdm-gpvdm.xml

endif

clean-local:
	rm gpvdm_core *.dat *.o ./plugins/*.so ./plugins/*.dll  *.log *.a  *.lib *.exe *.chk *.oplot -f
	rm ./gui/__pycache__/* -f
	rm autom4te.cache -rf
	rm code_docs -rf

	find . -type f -name '.~lock*' -delete
	
gpvdm_core$(EXEEXT):
	@printf ""


install-data-hook:
	cp materials $(DESTDIR)$(datadir)/gpvdm/materials -rf
	cp spectra $(DESTDIR)$(datadir)/gpvdm/spectra -rf
	cp ui $(DESTDIR)$(datadir)/gpvdm/ui -rf
	find lang -name "*.mo" -exec cp --parents {} $(DESTDIR)$(datadir)/gpvdm/ ";"
	ln -s $(pkgdatadir)/gui/gpvdm.py $(DESTDIR)$(bindir)/gpvdm
	chmod +x $(DESTDIR)$(pkgdatadir)/gui/gpvdm.py

uninstall-hook:
	-rm $(DESTDIR)$(pkgdatadir) -rf
	-rm $(DESTDIR)$(bindir)/gpvdm

ACLOCAL_AMFLAGS = -I m4
