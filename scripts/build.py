#! /usr/bin/env python3

import sys

sys.path.append('./scripts/')

import os
import locale
import argparse

from install import install
from install import uninstall


try:
	from dialog import Dialog
except:
	from menu import Dialog

#	print("The python3 module Dialog is not installed")
#	print("If you are on Ubuntu/Debian system, try apt-get install python3-dialog")
#	print("If you are on Fedora/Redhat system, try yum install python3-dialog")
#	sys.exit()

from package_menu import package_menu
from configure_menu import configure_menu
from publish_menu import publish_menu
from distributable import distributable

parser = argparse.ArgumentParser()
parser.add_argument("--adv", help="Advanced options", action='store_true')

args = parser.parse_args()

hpc=False
win=False
usear=True

# You may want to use 'autowidgetsize=True' here (requires pythondialog >= 3.1)
d = Dialog(dialog="dialog")

# Dialog.set_background_title() requires pythondialog 2.13 or later
d.set_background_title("https://www.gpvdm.com build configure, Roderick MacKenzie 2018")


while(1):
	menu=[]
	if args.adv:
		menu.append(("(publish)", "Publish"))
	menu.append(("(packages)", "Install packages needed by gpvdm"))
	menu.append(("(build)", "Build gpvdm"))
	menu.append(("(install)", "Install gpvdm"))
	menu.append(("(uninstall)", "Uninstall gpvdm"))
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
			install(d)

		if tag=="(uninstall)":
			uninstall(d)

		if tag=="(distributable)":
			distributable(d)

		if tag=="(about)":
			d.msgbox("This is the gpvdm build system, use it to configure the build system, make, and install gpvdm. Copyright Roderick MacKenzie 2018.  Released under the GPL v2 license.")


		if tag=="(exit)":
			break

		print(tag)
	else:
		break

