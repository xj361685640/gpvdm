#! /usr/bin/env python3
import os
import zipfile
import glob
import shutil

from build_paths import get_ver
from build_paths import get_package_path

from build_io import copy_files


def make_deb(d):
	if d!=None:
		data_out="&>out.dat &"

	my_dir=os.getcwd()
	build_dir=os.path.join(my_dir,"pub","build","gpvdm-"+get_ver())
	data_out=""

	os.chdir(build_dir)

	os.system("debuild -us -uc "+data_out)
	if d!=None:
		ret=d.tailbox("out.dat", height=None, width=100)

	os.chdir(my_dir)

	path=os.path.dirname(build_dir)
	print("path=",path)
	copy_files(get_package_path(),path,".deb")

#		sudo dpkg -r gpvdm
#		sudo dpkg -i gpvdm_4.98-1_amd64.deb


