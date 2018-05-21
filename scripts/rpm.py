#! /usr/bin/env python3
import os
import zipfile
import glob
import shutil



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
	if os.path.isdir("rpm"):
		shutil.rmtree("rpm")

	rpm_dir="./rpm"
	os.mkdir(rpm_dir)

	os.mkdir("./rpm/BUILD")
	os.mkdir("./rpm/RPMS")
	os.mkdir("./rpm/SOURCES")
	os.mkdir("./rpm/SPECS")
	os.mkdir("./rpm/SRPMS")
	os.mkdir("./rpm/BUILDROOT")

	if os.path.isdir('./pub/build')==False:
		d.msgbox("You have not publishe the code")
		return
	for file in glob.glob("./pub/build/*.tar"):
		shutil.copy(file, "./rpm/SOURCES/")

	copy_spec()

	os.system("rpmbuild -v --target x86_64 -ba --define \"_topdir "+os.getcwd()+"/rpm/"+"\" --noclean ./rpm/SPECS/gpvdm.spec &>out.dat &")
	ret=d.tailbox("out.dat", height=None, width=100)

	for file in glob.glob("./rpm/RPMS/x86_64/*.rpm"):
		shutil.copy(file, "./pub/")

	if os.path.isdir("rpm"):
		shutil.rmtree("rpm")


