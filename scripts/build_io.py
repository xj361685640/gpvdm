import os
import sys
import shutil

def copy_lib(dest,src,name,mode="gpl_distro"):
	dest=os.path.join(dest,name)
	src=os.path.join(src,name)
	copy_unifdef(dest,src,[".c",".h",".inp"],mode=mode)
	copy_files(dest,src,[".po",".pot",".ref",".rc","Makefile.am","makefile",".cl"])

def copy_plugin(dest,src,name,mode="gpl_distro"):
	dest=os.path.join(dest,"plugin",name)
	src=os.path.join(src,"plugin",name)
	copy_unifdef(dest,src,[".c",".h",".inp"],mode=mode)
	copy_files(dest,src,[".po",".pot",".ref",".rc","Makefile.am"])

def copy_unifdef(dest,src,file_name_array,mode="gpl_distro"):
	if type(file_name_array)==str:
		file_name_array=[file_name_array]

	if os.path.isdir(dest)==False:
		os.mkdir(dest)

	for item in os.listdir(src):
		for file_name in file_name_array:
			if item.endswith(file_name)==True:
				src_file=os.path.join(src,item)
				dest_file=os.path.join(dest,item)

				if mode == "windows":
					os.system("unifdef -Denable_multi_layers -Dprivate  -Denable_clever_contacts -Denable_time -Denable_left_right_bias -Uenable_dos_an -Uenable_interface -Ufull_time_domain -Uenable_server -Uenable_remesh -Uenable_fit "+src_file+" > "+dest_file)
				if mode == "gpl_distro":
					os.system("unifdef -Denable_multi_layers -Uprivate  -Uenable_clever_contacts  -Denable_time -Uenable_left_right_bias -Uenable_dos_an -Uenable_interface -Ufull_time_domain -Uenable_server -Uenable_remesh -Uwindows -Uenable_fit "+src_file+" > "+dest_file)

def copy_files(dest,src,file_name_array):
	if type(file_name_array)==str:
		file_name_array=[file_name_array]

	if os.path.isdir(dest)==False:
		os.mkdir(dest)

	for item in os.listdir(src):
		for file_name in file_name_array:
			if item.endswith(file_name)==True:
				src_file=os.path.join(src,item)
				dest_file=os.path.join(dest,item)
				print("src=",src_file)
				print("dest=",dest_file)
				shutil.copy2(src_file, dest_file)
