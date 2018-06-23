#!/usr/bin/env python3

import os
import sys
try:
	from dialog import Dialog
except:
	from menu import Dialog

apt=True
dnf=True
from apt_install import apt_install
from dnf_install import dnf_install

def install_fedora(d):
	libs=["zlib-devel","libzip-devel","libmatheval-devel","suitesparse-devel","openssl-devel","gsl-devel","libcurl-devel","blas-devel","librsvg2-tools"]
	#libs=" ".join(libs)

	tools=["texlive","ghostscript","ImageMagick","mencoder", "valgrind","@development-tools","fedora-packager","mingw32-gcc","gnuplot"]
	#tools=" ".join(tools)+" "

	python=["python-crypto","python-awake", "python3-qt5-devel","python3-crypto","python3-matplotlib-qt5","python3-openpyxl","python3-pyopengl","numpy","notify-python","python-inotify.noarch","python-matplotlib","python-inotify","python-matplotlib"]
	#python=" ".join(python)+" "

	devel=["indent","unifdef","indent","libcurl-devel","poedit","ElectricFence","kcachegrind","help2man"]
	#devel=" ".join(devel)+" "

	all=[]
	all.extend(libs)
	all.extend(tools)
	all.extend(python)
	all.extend(devel)

	dnf_install(d,all)

def install_suse(d):
	#to compile C
	tools=["gcc","autoconf","make","libtool","automake","libzip","libzip-devel","suitesparse-devel","help2man","gsl-devel","gettext-tools","dbus-1-devel","zlib-devel","gnuplot"]

	#for gui
	python=["python-qt5-utils","python3-qt5-devel","python3-qt5","python3-pycrypto","python3-numpy","python3-matplotlib","python3-matplotlib-qt-shared","python3-psutil","python3-openpyxl","python3-opengl","texlive"]

	#building rpm
	devel=["rpmbuild","unifdef"]

	all=[]
	all.extend(tools)
	all.extend(python)
	all.extend(devel)

	dnf_install(d,all)

def install_debian(d):
	libs=["libsuitesparse-dev","indent","unifdef","libsuitesparse-dev","libssl-dev","libedbus-dev","libzip-dev","libgsl-dev","libmatheval-dev","help2man","pluma","build-essential","imagemagick","license-reconcile","autoconf","librsvg2-bin","zip"]
	#libs=" ".join(libs)+" "

	tools=["rsync","pluma","build-essential","imagemagick","license-reconcile","autoconf","python-bashate","codespell","apt-file","gettext-lint inkscape","pep8","i18nspector","gettext","git","texlive-latex-recommended","latex2html"]
	#tools=" ".join(tools)+" "

	python=["python3-numpy","python3","python3-matplotlib","python3-pyqt5","python3-pyqt5.qtopengl","python3-opengl","python3-numpy","python3-crypto","python3-dbus.mainloop.pyqt5","python3-psutil"]
	#python=" ".join(python)+" "

	#cmd="apt-get -q -y install "+ libs+tools+python+" >out.dat 2>out.dat &"
	#print(cmd)
	#os.system(cmd)

	all=[]
	all.extend(libs)
	all.extend(tools)
	all.extend(python)

	apt_install(d,all)

def install_ubuntu(d):
	libs=["libsuitesparse-dev","indent","unifdef","libsuitesparse-dev","libssl-dev","libedbus-dev","libzip-dev","libgsl-dev","libmatheval-dev" ,"help2man", "pluma", "build-essential" , "imagemagick", "license-reconcile", "autoconf", "codespell","librsvg2-bin"]
	#libs=" ".join(libs)

	tools=["rsync","pluma","build-essential","imagemagick","imagemagick","license-reconcile","autoconf","python-bashate","codespell","complexity" ,"apt-file","gettext-lint", "gettext-lint", "inkscape" ,"pep8" ,"i18nspector","python-bashate" ,"automake"]
	tools.append("pbuilder")	#added for Ubuntu 18.04
	tools.append("python3-dev")	#added for Ubuntu 18.04
	#tools=" ".join(tools)+" "

	python=["python3", "python3-matplotlib", "python3-pyqt5.qtopengl", "python3-opengl", "python3-numpy", "python3-crypto", "python3-dbus.mainloop.pyqt5","python3-psutil"]
	#python=" ".join(python)+" "

	devel=["dh-virtualenv","debhelper"]
	#devel=" ".join(python)+" "

	all=[]
	all.extend(libs)
	all.extend(tools)
	all.extend(python)
	all.extend(devel)

	apt_install(d,all)
	#os.system("sudo apt-get install "+ libs+tools+python+devel+" &>out.dat &")

def install_raspbian(d):

	libs=["libsuitesparse-dev","indent","unifdef","libsuitesparse-dev","libssl-dev","libedbus-dev","libzip-dev","libgsl0-dev","libmatheval-dev","help2man","pluma","build-essential","imagemagick","license-reconcile","autoconf","librsvg2-bin"]

	#python
	python=["python3-numpy","python3","python3-matplotlib","python3-pyqt5.qtopengl","python3-opengl","python3-numpy","python3-crypto","python3-dbus.mainloop.pyqt5","python3-psutil"]

	#usefull tools
	tools=["rsync","pluma","build-essential","imagemagick","license-reconcile","autoconf","python-bashate","codespell","apt-file","gettext-lint","inkscape","pep8","i18nspector","qttools5-dev-tools","qtcreator"]

	all=[]
	all.extend(libs)
	all.extend(python)
	all.extend(tools)

	apt_install(d,all)


def install_centos(d):
	libs=["suitesparse-devel", "openssl-libs", "openssl-libs-devel", "openssl-devel", "libzip", "libzip-devel", "gsl-devel", "blas-devel", "mencoder",  "unifdef", "indent","zlib-devel" ,"libzip-devel", "suitesparse-devel", "openssl-devel", "gsl-devel" ,"libcurl-devel" ,"blas-devel" ,"help2man" ,"librsvg2-tools", "libmatheval-devel" ,"valgrind", "@development-tools" ,"fedora-packager", "pciutils-devel" ,"mingw32-gcc"]
	libs=" ".join(libs)

	tools=["unifdef", "indent", "ghostscript","ImageMagick"]
	tools=" ".join(tools)+" "

	python=["python34", "python34-matplotlib",  "python34-inotify", "python34-crypto", "python34-awake" ,"numpy" ,"notify34-python", "python34-inotify.noarch" ]
	python=" ".join(python)+" "

	devel=["epel-release"]
	devel=" ".join(python)+" "

	os.system("sudo yum install "+ libs+tools+python+devel+" &>out.dat &")



def install_mint(d):
	python=["python3","python3-matplotlib","python3-pyqt5.qtopengl","python3-opengl","python3-numpy","python3-crypto","python3-dbus.mainloop.pyqt5"]

	libs=["libsuitesparse-dev","indent","unifdef","libsuitesparse-dev","libssl-dev","libedbus-dev","libzip-dev","libgsl0-dev","libmatheval-dev","help2man","pluma","build-essential","imagemagick","license-reconcile","autoconf","codespell","librsvg2-bin"]

	tools=["rsync","pluma","build-essential","convert","imagemagick","license-reconcile","autoconf","python-bashate","codespell","complexity","apt-file","pofileSpell","gettext-lint","inkscape","spellintian","pep8","i18nspector","python-bashate","automake"]

	devel=["dh-virtualenv","debhelper"]

	all=[]

	all.extend(python)
	all.extend(libs)
	all.extend(tools)
	all.extend(devel)

	apt_install(d,all)





def package_menu(d):
	if os.geteuid() != 0:
		d.msgbox("You need to be root to install packages")
		return
	menu=[]

	menu.append(("(fedora)", "Fedora rpms"))
	menu.append(("(suse)","Open Suse rpms"))
	menu.append(("(debian)", "Debian debs"))
	menu.append(("(ubuntu)", "Ubuntu debs"))
	menu.append(("(centos)", "Centos debs"))
	menu.append(("(mint)","Mint debs"))
	menu.append(("(raspbian)","raspbian debs"))

	while(1):
		code, tag = d.menu("publish for:", choices=menu)
		if code == d.OK:
			if tag=="(fedora)":
				install_fedora(d)
				ret=d.tailbox("out.dat", height=None, width=100)
				sys.exit()

			if tag=="(debian)":
				install_debian(d)
				ret=d.tailbox("out.dat", height=100, width=100)

			if tag=="(ubuntu)":
				install_ubuntu(d)
				ret=d.tailbox("out.dat", height=None, width=100)

			if tag=="(centos)":
				install_centos(d)
				ret=d.tailbox("out.dat", height=None, width=100)

			if tag=="(mint)":
				install_mint(d)
				ret=d.tailbox("out.dat", hieght=None, width=100)

			if tag=="(suse)":
				install_suse(d)
				ret=d.tailbox("out.dat",height=None, width=100)

			if tag=="(raspbian)":
				install_raspbian(d)
				ret=d.tailbox("out.dat",height=None, width=100)


		else:
			return

if __name__ == "__main__":
	d = Dialog(dialog="dialog")

	d.set_background_title("gpvdm build configure")
	package_menu(d)
