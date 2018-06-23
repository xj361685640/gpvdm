
import os
import platform

from inp import inp_get_token_value

package_path=None
pub_path=None
rpm_build_dir=None

def get_ver():
	return inp_get_token_value("ver.inp", "#core")

def build_setup_paths():
	global package_path
	global pub_path
	global rpm_build_dir

	ret=platform.dist()
	distro_name=ret[0]
	os_numer=ret[1]
	os_cute_name=ret[2].replace(" ","_")

	distro=ret[0]+"_"+ret[1]
	package_path=os.path.join(os.getcwd(),"package_lib",distro_name,os_numer+"_"+os_cute_name)
	if os.path.isdir(package_path)==False:
		os.makedirs(package_path)

	pub_path=os.path.join(os.getcwd(),"pub")
	rpm_build_dir=os.path.join(os.getcwd(),"rpm")

def get_package_path():
	global package_path
	return package_path

def get_pub_path():
	global pub_path
	return pub_path

def get_rpm_build_dir():
	global rpm_build_dir
	return rpm_build_dir
