
import os
import sys
import shutil
try:
	from dialog import Dialog
except:
	from menu import Dialog

from to_web import to_github
from to_web import src_to_web

from build_paths import get_pub_path

from cp_materials import cp_materials
from cp_materials import cp_spectra
from cp_materials import cp_devices

from build_paths import get_ver

from build_io import copy_files
from build_io import copy_lib
from build_io import copy_plugin

def publish_src(d,distro=None,publication_mode="gpl_distro"):

	data_out=""
	ver=get_ver()
	my_dir=os.getcwd()

	pub_dir=get_pub_path()
	build_dir=os.path.join(pub_dir,"build")
	dest=os.path.join(build_dir,"gpvdm-"+str(ver))

	#mkdir tree
	shutil.rmtree(pub_dir)

	os.mkdir(pub_dir)
	os.mkdir(build_dir)

	os.mkdir(dest)
	os.mkdir(os.path.join(dest,"plugins"))

	spath=os.path.join(my_dir,"plot")
	dpath=os.path.join(dest,"plot")
	copy_files(dpath,spath,".plot")

	os.mkdir(os.path.join(dest,"scripts"))

	spath=os.path.join(my_dir,"gui")
	dpath=os.path.join(dest,"gui")
	copy_files(dpath,spath,".py")

	spath=os.path.join(my_dir,"images")
	dpath=os.path.join(dest,"images")
	copy_files(dpath,spath,".jpg")
	copy_files(dpath,spath,".ico")

	os.mkdir(os.path.join(dest,"images","scalable"))
	os.mkdir(os.path.join(dest,"images","splash"))
	os.mkdir(os.path.join(dest,"images","flags"))
	os.mkdir(os.path.join(dest,"images","32x32"))
	os.mkdir(os.path.join(dest,"images","64x64"))
	os.mkdir(os.path.join(dest,"images","16x16"))
	os.mkdir(os.path.join(dest,"images","48x32"))

	os.mkdir(os.path.join(dest,"css"))

	spath=os.path.join(my_dir,"html")
	dpath=os.path.join(dest,"html")
	copy_files(dpath,spath,".html")

	spath=os.path.join(my_dir,"man")
	dpath=os.path.join(dest,"man")
	copy_files(dpath,spath,"Makefile.am")

	spath=os.path.join(my_dir,"desktop")
	dpath=os.path.join(dest,"desktop")
	copy_files(dpath,spath,[".svg",".desktop",".xml"])

	os.mkdir(os.path.join(dest,"ui"))

	copy_lib(dest,my_dir,"lang",mode=publication_mode)
	copy_lib(dest,my_dir,"src",mode=publication_mode)
	copy_lib(dest,my_dir,"lib",mode=publication_mode)
	copy_lib(dest,my_dir,"libi",mode=publication_mode)
	copy_lib(dest,my_dir,"librpn",mode=publication_mode)
	copy_lib(dest,my_dir,"libmemory",mode=publication_mode)
	copy_lib(dest,my_dir,"liblight",mode=publication_mode)
	copy_lib(dest,my_dir,"libmeasure",mode=publication_mode)
	copy_lib(dest,my_dir,"libdump",mode=publication_mode)
	copy_lib(dest,my_dir,"libcontacts",mode=publication_mode)
	copy_lib(dest,my_dir,"libdumpctrl",mode=publication_mode)
	copy_lib(dest,my_dir,"libmesh",mode=publication_mode)
	copy_lib(dest,my_dir,"libserver",mode=publication_mode)
	copy_lib(dest,my_dir,"libdos",mode=publication_mode)
	copy_lib(dest,my_dir,"cluster",mode=publication_mode)

	spath=os.path.join(my_dir,"include")
	dpath=os.path.join(dest,"include")
	copy_files(dpath,spath,[".h"])

	copy_files(dest,my_dir,["Makefile.am","configure","configure.ac","doxygen.config","README","COPYING"])

	cp_materials(dest,my_dir)
	cp_spectra(dest,my_dir)
	cp_devices(dest,my_dir)

	if d!=None:
		data_out="&>out.dat &"
	
	if publication_mode=="windows":
		os.system("./pub.sh --windows "+data_out)
	else:
		os.system("./pub.sh --github "+data_out)

	if d!=None:
		ret=d.tailbox("out.dat", height=None, width=100)

	data_out=""

	os.chdir(build_dir)

	if distro=="debian":
		os.system("tar -zcf ./gpvdm_"+ver+".orig.tar.gz ./gpvdm-"+ver+"/")
	else:
		os.system("tar -zcf ./gpvdm-"+ver+".tar.gz ./gpvdm-"+ver+"/")
		os.system("tar -cf ./gpvdm-"+ver+".tar ./gpvdm-"+ver+"/")

	os.chdir(my_dir)


