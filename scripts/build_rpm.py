#! /usr/bin/env python3
import os
import zipfile
import glob
import shutil

from build_paths import get_rpm_build_dir



def get_ver():
	zf = zipfile.ZipFile("base.gpvdm", 'r')
	read_lines = zf.read("ver.inp")
	zf.close()
	lines=read_lines.decode('utf-8')
	lines=lines.split("\n")
	ver=lines[1]
	return ver

def copy_spec():
	ver=get_ver()
	f = open("./scripts/gpvdm.spec", mode='r')
	lines = f.read()
	f.close()

	lines=lines.replace("${ver}",ver)

	f=open("./rpm/SPECS/gpvdm.spec", mode='w')
	lines = f.write(lines)
	f.close()

def make_rmp_dir(d):

	data_out=""
	if d!=None:
		data_out="&>out.dat &"

	if os.path.isdir(get_rpm_build_dir()):
		shutil.rmtree(get_rpm_build_dir())

	os.mkdir(get_rpm_build_dir())

	os.mkdir(os.path.join(get_rpm_build_dir(),"BUILD"))
	os.mkdir(os.path.join(get_rpm_build_dir(),"RPMS"))
	os.mkdir(os.path.join(get_rpm_build_dir(),"SOURCES"))
	os.mkdir(os.path.join(get_rpm_build_dir(),"SPECS"))
	os.mkdir(os.path.join(get_rpm_build_dir(),"SRPMS"))
	os.mkdir(os.path.join(get_rpm_build_dir(),"BUILDROOT"))

	if os.path.isdir('./pub/build')==False:
		d.msgbox("You have not publishe the code")
		return
	for file in glob.glob("./pub/build/*.tar"):
		shutil.copy(file, "./rpm/SOURCES/")

	copy_spec()

	os.system("rpmbuild -v --target x86_64 -ba --define \"_topdir "+os.getcwd()+"/rpm/"+"\" --noclean ./rpm/SPECS/gpvdm.spec "+data_out)
	if d!=None:
		ret=d.tailbox("out.dat", height=None, width=100)

	for file in glob.glob("./rpm/RPMS/x86_64/*.rpm"):
		shutil.copy(file, "./pub/")

	if os.path.isdir(get_rpm_build_dir()):
		shutil.rmtree(get_rpm_build_dir())


