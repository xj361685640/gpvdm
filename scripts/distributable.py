#!/usr/bin/env python3

import os
import sys
import shutil
try:
	from dialog import Dialog
except:
	from menu import Dialog
from build_rpm import make_rmp_dir
from deb import make_deb

from shutil import copyfile
from shutil import rmtree

def windows_install(d):
	if d.yesno("make install?") == d.OK:
		dll_opengl_path="/home/rod/windll/opengl_dlls/"
		dll_compiled_path="/home/rod/windll/compiled_dlls/"

		output_path="./pub"		
		if os.path.isdir(output_path)==True:
			rmtree(output_path)

		os.mkdir(output_path)
		os.system("make DESTDIR="+output_path+" install  >out.dat 2>out.dat &")
		ret=d.tailbox("out.dat", height=None, width=150)

		#copy dlls
		for file in os.listdir(dll_opengl_path):
			copyfile(os.path.join(dll_opengl_path,file), os.path.join(output_path,"gpvdm",file))

		for file in os.listdir(dll_compiled_path):
			copyfile(os.path.join(dll_compiled_path,file), os.path.join(output_path,"gpvdm",file))

		publish_dir="/home/rod/windows/share/pub"
		if os.path.isdir(publish_dir)==True:
			rmtree(publish_dir)

		#os.mkdir(publish_dir)
		os.remove("./pub/gpvdm/gpvdm")
		shutil.copytree("./pub/gpvdm", publish_dir, symlinks=False)

def distributable(d):
	if os.geteuid() == 0:
		d.msgbox("Don't run me as root.")
		return
	menu=[]

	menu.append(("(rpm)", "Build rpm"))
	menu.append(("(deb)", "Build deb"))

	menu.append(("(win)", "Windows installer"))


	while(1):
		code, tag = d.menu("Build:", choices=menu)
		if code == d.OK:
			if tag=="(rpm)":
				make_rmp_dir(d)

			if tag=="(rpm)":
				make_deb(d)

			if tag=="(win)":
				windows_install(d)

		else:
			return

