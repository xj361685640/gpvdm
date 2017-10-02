#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
#
#	https://www.gpvdm.com
#	Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License v2.0, as published by
#    the Free Software Foundation.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.


import sys
import os
import shutil
from scan_io import get_scan_dirs

from util import copy_scan_dir

from util import delete_second_level_link_tree


from util_zip import zip_lsdir
from inp import inp_issequential_file
from clone import gpvdm_clone
from util_zip import zip_remove_file
from util_zip import archive_copy_file
from cal_path import get_inp_file_path
from util_zip import archive_isfile
from util_zip import archive_merge_file
from util_zip import archive_get_file_ver

from cal_path import get_materials_path
from cal_path import get_base_material_path
from util_zip import archive_compact_files
from util_zip import extract_file_from_archive

from inp_util import inp_merge

def update_simulaton_to_new_ver(file_name):
	pre, ext = os.path.splitext(file_name)
	back_file = pre + ".bak"


	if os.path.isfile(back_file)==False:
		archive_compact_files(file_name)
		os.rename(file_name, back_file)

		dest_dir = os.path.dirname(file_name)

		pre, ext = os.path.splitext(file_name)
		dest_archive = pre + ".gpvdm"

		gpvdm_clone(dest_dir,False)

		merge_archives(back_file,dest_archive,False)
	else:
		print("Can't merge bak file already exists")
	return True


def remove_non_used_index_files(dest_archive,src_archive):
	ls_dest=zip_lsdir(dest_archive)
	if ls_dest==False:
		print("File ",ls_dest, "not found")
		return

	ls_src=zip_lsdir(src_archive)
	if ls_src==False:
		print("File ",src_archive, "not found")


	for my_file in ls_dest:

		if my_file.endswith(".inp"):
			if my_file.startswith("dos"):
				if ls_src.count(my_file)==0:
					zip_remove_file(dest_archive,my_file)

			if my_file.startswith("pulse"):
				if ls_src.count(my_file)==0:
					zip_remove_file(dest_archive,my_file)

			if my_file.startswith("pl"):
				if ls_src.count(my_file)==0:
					zip_remove_file(dest_archive,my_file)

			if my_file.startswith("time_mesh_config"):
				if ls_src.count(my_file)==0:
					zip_remove_file(dest_archive,my_file)

			if my_file.startswith("laser"):
				if ls_src.count(my_file)==0:
					zip_remove_file(dest_archive,my_file)

def merge_archives(src_archive,dest_archive,only_over_write):
#	src_dir=os.path.dirname(src_archive)
#	dest_dir=os.path.dirname(dest_archive)
	dest_path=os.path.dirname(dest_archive)
	template_archive=os.path.join(get_inp_file_path(),"base.gpvdm")

	remove_non_used_index_files(dest_archive,src_archive)

	files=[ "ver.inp","sim.inp", "device.inp", "parasitic.inp", "led.inp","ray.inp","stark.inp" ,"shg.inp"   ,"jv.inp" , "math.inp",  "dump.inp" , "light.inp", "server.inp", "light_exp.inp","info.inp","config.inp" ]

	base_file=files[:]

	print(src_archive)
	ls=zip_lsdir(src_archive)

	for i in range(0,len(ls)):
		if inp_issequential_file(ls[i],"dos"):
			files.append(ls[i])
			base_file.append(os.path.join(get_base_material_path(),"generic","generic_organic","dos.inp"))

		if inp_issequential_file(ls[i],"pl"):
			files.append(ls[i])
			base_file.append(os.path.join(get_base_material_path(),"generic","generic_organic","pl.inp"))

		if inp_issequential_file(ls[i],"pulse"):
			files.append(ls[i])
			base_file.append("pulse0.inp")

		if inp_issequential_file(ls[i],"laser"):
			files.append(ls[i])
			base_file.append("laser0.inp")

	for i in range(0,len(files)):
		print("Importing",files[i])
		print("dest_archive:",dest_archive)
		print("src_archive",src_archive)
		print("template:",template_archive)
		print("base_file:",base_file[i])
		print("only overwrite",only_over_write)

		if only_over_write==False:
			if archive_isfile(dest_archive,files[i])==False:
				if archive_copy_file(dest_archive,files[i],template_archive,base_file[i])==False:
					print("problem copying",template_archive,base_file[i])
				print("made new file",dest_archive,files[i])

		ret=archive_merge_file(dest_archive,src_archive,files[i])
		print("ret=",ret)
		print("-----------------------")

	files=[ "windows_list2.inp","epitaxy.inp", "contacts.inp", "fit.inp", "constraints.inp","duplicate.inp", "thermal.inp","mesh_x.inp","mesh_y.inp","mesh_z.inp", "dump_file.inp" ]
	base_file=files[:]

	ls=zip_lsdir(src_archive)
	for i in range(0,len(ls)):

		if inp_issequential_file(ls[i],"time_mesh_config"):
			files.append(ls[i])
			base_file.append("time_mesh_config0.inp")

		if inp_issequential_file(ls[i],"homo"):
			files.append(ls[i])
			base_file.append("homo0.inp")

		if inp_issequential_file(ls[i],"lumo"):
			files.append(ls[i])
			base_file.append("lumo0.inp")

	for i in range(0,len(files)):

		template_ver=archive_get_file_ver(template_archive,base_file[i])
		src_ver=archive_get_file_ver(src_archive,files[i])
		print("Importing",files[i])
		print("template ver",template_ver)
		print("src_ver",src_ver)
		print("template_archive",template_archive)
		print("src_archive",src_archive)
		print("dest_archive",dest_archive)

		if template_ver!="" and src_ver!="":
			if template_ver==src_ver:
				archive_copy_file(dest_archive,files[i],src_archive,files[i])
				print("complex copy")

		print("-----------------------")

	#copy files without checking ver
	for i in range(0,len(ls)):
		if inp_issequential_file(ls[i],"genrate"):
			archive_copy_file(dest_archive,ls[i],src_archive,ls[i],dest="file")

	#if you find a materials directory in the archive try to merge it
	for i in range(0,len(ls)):
		zip_dir_name=ls[i].split("/")
		if zip_dir_name[0]=="materials":
			dest=os.path.join(os.path.dirname(get_materials_path()))
			print("Try to read",src_archive,ls[i],dest)
			extract_file_from_archive(dest,src_archive,ls[i])

	for i in range(0,len(ls)):
		zip_dir_name=ls[i].split("/")
		if zip_dir_name[0]=="sim":
			extract_file_from_archive(dest_path,src_archive,ls[i])

	#search for scan directories
	scan_dirs=[]
	for i in range(0,len(ls)):
		if ls[i].endswith("gpvdm_gui_config.inp"):
			scan_dirs.append(os.path.dirname(ls[i]))

	#extract scan directories
	for i in range(0,len(ls)):
		for ii in range(0,len(scan_dirs)):
			if ls[i].startswith(scan_dirs[ii])==True:
				print("Try to read",src_archive,ls[i])
				extract_file_from_archive(dest_path,src_archive,ls[i])
	print("search",scan_dirs)
		
def import_archive(src_archive,dest_archive,only_over_write):
	src_dir=os.path.dirname(src_archive)
	dest_dir=os.path.dirname(dest_archive)

	if src_archive.endswith('.gpvdm')==False:
		print("I can only import from .gpvdm files")
		return

	if dest_archive.endswith('.gpvdm')==False:
		print("I can only import to .gpvdm files")
		return

	merge_archives(src_archive,dest_archive,only_over_write)

	import_scan_dirs(dest_dir,src_dir)

def import_scan_dirs(dest_dir,src_dir):
	sim_dirs=[]
	get_scan_dirs(sim_dirs,src_dir)
	for my_file in sim_dirs:
		dest=os.path.join(dest_dir,os.path.basename(my_file))
		print("copy scan dir",my_file,"to",dest)

		if os.path.exists(dest):
			delete_second_level_link_tree(dest)

		copy_scan_dir(dest,my_file)

def clean_scan_dirs(path):
	sim_dirs=[]
	get_scan_dirs(sim_dirs,path)

	for my_dir in sim_dirs:
		print("cleaning ",my_dir)
		files = os.listdir(my_dir)
		for file in files:
			file_name=os.path.join(my_dir,file)
			if file_name.endswith(".dat"):
				print("Remove",file_name)
				os.remove(file_name)
			if os.path.isdir(file_name):
				print("remove dir",file_name)
				shutil.rmtree(file_name)

def read_lines_from_file(file_name):
	if os.path.isfile(file_name)==True:
		f = open(file_name, mode='rb')
		lines = f.read()
		f.close()

	try:
		lines=lines.decode('utf-8')
		lines=lines.split("\n")
	except:
		lines=[]
		
	return lines

def patch_file(dest_file,base_file,input_file):
	
	src_lines=[]
	dest_lines=[]

	base_file_lines=read_lines_from_file(base_file)
	
	input_file_lines=read_lines_from_file(input_file)
	

	errors=inp_merge(base_file_lines,input_file_lines)

	f=open(dest_file, mode='wb')
	lines = f.write(str.encode('\n'.join(base_file_lines)))
	f.close()
