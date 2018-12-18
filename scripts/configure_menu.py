
import os
try:
	from dialog import Dialog
except:
	from menu import Dialog

from make_m4 import make_m4

from pathlib import Path
from shutil import copyfile

def test(d):
	if d.yesno("Run gpvdm") == d.OK:
		os.system("./go.o  >out.dat 2>out.dat &")
		et=d.tailbox("out.dat", height=None, width=150)

def make(d):
	if d.yesno("Run make clean") == d.OK:
		os.system("make clean  >out.dat 2>out.dat &")
		et=d.tailbox("out.dat", height=None, width=150)

	if d.yesno("Run make") == d.OK:
		jobs=os.cpu_count()
		os.system("make  -j "+str(jobs)+" >out.dat 2>out.dat &")
		et=d.tailbox("out.dat", height=None, width=150)

    	#d.msgbox("You have been warned...")


def build_configure():
	os.system("aclocal")
	os.system("automake --add-missing")
	os.system("automake")
	os.system("autoconf")

def configure_for_fedora(d):
	make_m4(hpc=False, win=False,usear=True)
	#d.infobox("aclocal", width=0, height=0, title="configure")
	build_configure()
	os.system("./configure CPPFLAGS=\"-I/usr/include/suitesparse/\" --datadir=\"/usr/share/\" --bindir=\"/usr/bin/\" &>out.dat &")
	et=d.tailbox("out.dat", height=None, width=100)

def configure_for_debian(d):
	make_m4(hpc=False, win=False,usear=True)
	#d.infobox("aclocal", width=0, height=0, title="configure")
	build_configure()
	os.system("./configure CPPFLAGS=\"-I/usr/include/\" --datadir=\"/usr/share/\" --bindir=\"/usr/bin/\" >out.dat 2>out.dat &")
	et=d.tailbox("out.dat", height=None, width=100)

def configure_for_ubuntu(d):
	make_m4(hpc=False, win=False,usear=True)

	build_configure()

	os.system("./configure CPPFLAGS=\"-I/usr/include/\" --datadir=\"/usr/share/\" --bindir=\"/usr/bin/\" >out.dat 2>out.dat &")
	et=d.tailbox("out.dat", height=None, width=100)

def configure_for_centos_hpc(d):
	make_m4(hpc=True, win=False,usear=False)

	build_configure()

	os.system("./configure CPPFLAGS=\"-I/usr/include/\" --enable-hpc --enable-noimages --enable-noplots --enable-noman --enable-nodesktop --enable-nodevicelib --enable-nohtml >out.dat 2>out.dat &")

	et=d.tailbox("out.dat", height=None, width=100)

def configure_for_centos(d):
	make_m4(hpc=False, win=False,usear=True)

	build_configure()

	os.system("./configure CPPFLAGS=\"-I/usr/include/\" >out.dat 2>out.dat &")
	et=d.tailbox("out.dat", height=None, width=100)

	make(d)

def configure_autodetect(d):
	import platform
	plat=platform.dist()[0].lower()
	chipset=os.uname().machine
	configured=False
	if d.yesno("Configure for "+plat+" "+chipset) == d.CANCEL:
		return
	if plat=="fedora":
		configured=True
		configure_for_fedora(d)
	elif plat=="debian":
		configured=True
		configure_for_debian(d)
	elif plat=="ubuntu":
		configured=True
		configure_for_ubuntu(d)
	elif plat=="centos":
		configured=True
		configure_for_centos_hpc(d)

	if configured==True:
		make(d)
		d.msgbox("Built")
	else:
		d.msgbox("Can't auto configure for this platform.")
		
def configure_menu(d):
	if os.geteuid() == 0:
		d.msgbox("Don't do a build as root")
		return
	code, tag = d.menu("build for:",
		               choices=[("(auto)", "Detect distro (x86_64/ARM)"),
								("(fedora)", "fedora (x86_64)"),
								("(debian)", "debian (x86_64)"),
								("(raspberry)", "Raspberry (ARM)"),
								("(centos)", "CENTOS (x86_64)"),
								("(mint)", "Mint (x86_64)"),
								("(ubuntu)", "Ubuntu (x86_64)"),
								("(suse)", "Open Suse (x86_64)"),
								("(arch)", "Arch (x86_64)"),
								("(win)", "Windows (x86_64)"),
								("(debian-i386)","Debian (i386)"),
								("(default)", "generic Linux (x86_64)")
								])

	if code == d.OK:
		if tag=="(auto)":
			configure_autodetect(d)

		if tag=="(default)":
			make_m4(hpc=False, win=False,usear=True)
			#d.infobox("aclocal", width=0, height=0, title="configure")
			build_configure()
			os.system("./configure CPPFLAGS=\"-I/usr/include/\"  &>out.dat &")
			et=d.tailbox("out.dat", height=None, width=100)

			make(d)

			d.msgbox("Built")

		if tag=="(fedora)":
			configure_for_fedora(d)
			make(d)
			d.msgbox("Built")


		if tag=="(debian)":
			configure_for_debian(d)
			make(d)
			d.msgbox("Built")

		if tag=="(raspberry)":
			make_m4(hpc=False, win=False,usear=True)

			os.system("aclocal")
			os.system("autoconf")
			os.system("autoheader")
			os.system("automake")
			os.system("automake --add-missing")
			os.system("automake")
			os.system("./configure CPPFLAGS=\"-I/usr/include/\" --host=arm-linux >out.dat 2>out.dat &")
			et=d.tailbox("out.dat", height=None, width=100)

			make(d)

			d.msgbox("Built")


		if tag=="(centos)":
			configure_for_centos(d)

			d.msgbox("Built")


		if tag=="(mint)":
			make_m4(hpc=False, win=False,usear=True)

			build_configure()

			os.system("./configure CPPFLAGS=\"-I/usr/include/\" >out.dat 2>out.dat &")
			et=d.tailbox("out.dat", height=None, width=100)

			make(d)

			d.msgbox("Built")

		if tag=="(ubuntu)":
			configure_for_ubuntu(d)
			make(d)
			d.msgbox("Built")

		if tag=="(suse)":
			make_m4(hpc=False, win=False,usear=True)

			build_configure()

			os.system("./configure CPPFLAGS=\"-I/usr/include/\" >out.dat 2>out.dat &")
			et=d.tailbox("out.dat", height=None, width=100)

			make(d)

			d.msgbox("Built")

		if tag=="(arch)":
			make_m4(hpc=False, win=False,usear=True)

			build_configure()

			os.system("./configure CPPFLAGS=\"-I/usr/include/\" >out.dat 2>out.dat &")
			et=d.tailbox("out.dat", height=None, width=100)

			make(d)

			d.msgbox("Built")

		if tag=="(win)":
			make_m4(hpc=False, win=True,usear=True)

			build_configure()

			home=str(Path.home())
			flags="-I"+home+"/windll/libzip/libzip-0.11.2/lib/ -I"+home+"/windll/gsl-1.16/ -I"+home+"/windll/umfpack/UFconfig/ -I"+home+"/windll/umfpack/AMD/Include/ -I"+home+"/windll/umfpack/UMFPACK/Include/ -I"+home+"/windll/OpenCL-Headers-master/"
			os.system("./configure --host=i686-w64-mingw32 CPPFLAGS=\""+flags+"\"  --enable-noplots --enable-noman --docdir=/gpvdm/ --datadir=/ --bindir=/gpvdm/  --libdir=/ --enable-nodesktop >out.dat 2>out.dat &")
			et=d.tailbox("out.dat", height=None, width=100)

			make(d)

			#windows_install(d)

			d.msgbox("Built")

		if tag=="(debian-i386)":
			make_m4(hpc=False, win=False,usear=True)
			#d.infobox("aclocal", width=0, height=0, title="configure")
			build_configure()
			os.system("./configure CPPFLAGS=\"-I/usr/include/\" --host=i686-linux-gnu --build=i686-linux-gnu CC=\"gcc -m32\" CXX=\"g++ -m32\" >out.dat 2>out.dat &")
			et=d.tailbox("out.dat", height=None, width=100)

			make(d)

			d.msgbox("Built")





