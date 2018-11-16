#! /usr/bin/env python3

import sys

sys.path.append('./scripts/')
sys.path.append('./gui/')
sys.path.append('../gui/')

import os
import locale
import argparse

from install import install_menu
from deb import make_deb
from dnf_install import dnf_install

try:
	from dialog import Dialog
except:
	from menu import Dialog
#	sys.exit()

import platform

from package_menu import package_menu
from configure_menu import configure_menu
from publish_menu import publish_menu
from distributable import distributable
from to_web import rpm_to_web
from to_web import package_to_lib
from publish import publish_src
from build_rpm import make_rmp_dir
from build_paths import build_setup_paths

build_setup_paths()

parser = argparse.ArgumentParser()
parser.add_argument("--adv", help="Advanced options", action='store_true')
parser.add_argument("--buildlinuxpackage", help="Build rpm or deb", action='store_true')


args = parser.parse_args()


hpc=False
win=False
usear=True

# You may want to use 'autowidgetsize=True' here (requires pythondialog >= 3.1)
d = Dialog(dialog="dialog")
#dnf_install(d,["gnuplot"])
#sys.exit(0)

if args.buildlinuxpackage:
	ret=platform.dist()
	distro=ret[0]
	rel=ret[1]
	if distro=="fedora":
		publish_src(None,publication_mode="gpl_distro")
		make_rmp_dir(None)
		package_to_lib()
		#rpm_to_web(None)
	elif distro=="Ubuntu":
		publish_src(None,distro="debian",publication_mode="gpl_distro")
		make_deb(None)
	elif distro=="debian":
		publish_src(None,distro="debian",publication_mode="gpl_distro")
		make_deb(None)
	else:
		print("distro not known",distro)
	
	sys.exit(0)

# Dialog.set_background_title() requires pythondialog 2.13 or later
d.set_background_title("https://www.gpvdm.com build configure, Roderick MacKenzie 2018")


while(1):
	menu=[]
	if args.adv:
		menu.append(("(publish)", "Publish"))
	menu.append(("(packages)", "Install packages needed by gpvdm"))
	menu.append(("(build)", "Build gpvdm"))
	menu.append(("(install)", "Install/Remove"))
	menu.append(("(distributable)", "Build rpm"))
	menu.append(("(about)", "About"))

	menu.append(("(exit)", "Exit"))

	code, tag = d.menu("gpvdm build system:", choices=menu)
	if code == d.OK:
		if tag=="(publish)":
			publish_menu(d)

		if tag=="(packages)":
			package_menu(d)

		if tag=="(build)":
			configure_menu(d)

		if tag=="(install)":
			install_menu(d)


		if tag=="(distributable)":
			distributable(d)

		if tag=="(about)":
			d.msgbox("This is the gpvdm build system, use it to configure the build system, make, and install gpvdm. Copyright Roderick MacKenzie 2018.  Released under the GPL v2 license.")


		if tag=="(exit)":
			break

		print(tag)
	else:
		break

