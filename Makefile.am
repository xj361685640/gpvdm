AUTOMAKE_OPTIONS = foreign
EXTRA_DIST = config.rpath m4/ChangeLog  materials
SUBDIRS = @BUILD_DIRS@

bin_PROGRAMS=gpvdm_core
gpvdm_core_SOURCES=

if INCLUDE_IMAGES
imagedir = $(pkgdatadir)/images
image_DATA = images/*.jpg images/*.png
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

check-gettext:
	@if test x$(USE_NLS) != "xyes" ; then echo "Missing gettext. Rerun configure and check for" \
	"'checking whether to use NLS... yes'!" ; exit 1 ; fi

update-po: check-gettext
	@find . \( -name "*.py" -or -name "*.c" \)| grep -v pub | sort > $(srcdir)/po/POTFILES.in.2 ; \
	if diff $(srcdir)/po/POTFILES.in $(srcdir)/po/POTFILES.in.2 >/dev/null 2>&1 ; then \
		rm -f $(srcdir)/po/POTFILES.in.2 ; \
	else \
		mv $(srcdir)/po/POTFILES.in.2 $(srcdir)/po/POTFILES.in ; \
	fi
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-po

update-gmo: check-gettext
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-gmo

force-update-gmo: check-gettext
	touch po/*.po
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-gmo

force-update-gmo-%: check-gettext
	@language=`echo $@ | sed s/force-update-gmo-//` ; \
	if test ! -f po/$$language.po ; then echo "file po/$$language.po does not exist" ; exit 1 ; fi ; \
	touch po/$$language.po ; \
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-gmo

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
