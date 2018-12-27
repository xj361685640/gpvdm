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

## @package import_archive
#  Logic to import .gpvdm files, even if they are in an older fromat.
#


import sys
import os
import shutil
from scan_io import get_scan_dirs

from util import copy_scan_dir

from util import gpvdm_delete_file


from util_zip import zip_lsdir
from inp import inp_issequential_file
from clone import gpvdm_clone
from util_zip import zip_remove_file
from util_zip import archive_copy_file
from cal_path import get_inp_file_path
from util_zip import archive_isfile
from util_zip import archive_merge_file
from inp import inp_get_file_ver

from cal_path import get_materials_path
from cal_path import get_base_material_path
from util_zip import archive_compress
from util_zip import extract_file_from_archive

from inp_util import inp_merge
from cal_path import get_default_material_path

from progress import progress_class
from process_events import process_events
import re

class file_type():
	JUST_COPY=0
	CHECK_VER_THEN_COPY=1
	MERGE=2
	
	def __init__(self,name="fit_data",dest="archive",copy_opp=JUST_COPY,base_file=""):
		self.name=name
		self.dest=dest
		self.copy_opp=copy_opp
		self.base_file=base_file
		if base_file=="":
			self.base_file=name
		if name.endswith(".inp")==True:
			self.index_file=False
		else:
			self.index_file=True

file_list=[]

file_list.append(file_type(name="genrate",dest="file",copy_opp=file_type().JUST_COPY))
file_list.append(file_type(name="fit",dest="archive",copy_opp=file_type().JUST_COPY))
file_list.append(file_type(name="fit_data",dest="archive",copy_opp=file_type().JUST_COPY))
file_list.append(file_type(name="fit_import_config",dest="archive",copy_opp=file_type().JUST_COPY))
file_list.append(file_type(name="fit_math",dest="archive",copy_opp=file_type().JUST_COPY))
file_list.append(file_type(name="fit_patch",dest="archive",copy_opp=file_type().JUST_COPY))
file_list.append(file_type(name="fit_target",dest="archive",copy_opp=file_type().JUST_COPY))
file_list.append(file_type(name="fit_vars.inp",dest="archive",copy_opp=file_type().JUST_COPY))
file_list.append(file_type(name="fit.inp",dest="archive",copy_opp=file_type().JUST_COPY))

file_list.append(file_type(name="fit_error_delta",dest="file",copy_opp=file_type().JUST_COPY))
file_list.append(file_type(name="fit_error_exp",dest="file",copy_opp=file_type().JUST_COPY))
file_list.append(file_type(name="fit_error_sim",dest="file",copy_opp=file_type().JUST_COPY))

file_list.append(file_type(name="windows_list2.inp",dest="archive",copy_opp=file_type().CHECK_VER_THEN_COPY,base_file="windows_list2.inp"))
file_list.append(file_type(name="epitaxy.inp",dest="archive",copy_opp=file_type().CHECK_VER_THEN_COPY,base_file="epitaxy.inp"))
file_list.append(file_type(name="contacts.inp",dest="archive",copy_opp=file_type().CHECK_VER_THEN_COPY,base_file="contacts.inp"))

file_list.append(file_type(name="constraints.inp",dest="archive",copy_opp=file_type().CHECK_VER_THEN_COPY,base_file="constraints.inp"))
file_list.append(file_type(name="duplicate.inp",dest="archive",copy_opp=file_type().CHECK_VER_THEN_COPY,base_file="duplicate.inp"))
file_list.append(file_type(name="thermal.inp",dest="archive",copy_opp=file_type().MERGE,base_file="thermal.inp"))
file_list.append(file_type(name="fdtd.inp",dest="archive",copy_opp=file_type().MERGE,base_file="fdtd.inp"))
file_list.append(file_type(name="mesh_x.inp",dest="archive",copy_opp=file_type().CHECK_VER_THEN_COPY,base_file="mesh_x.inp"))
file_list.append(file_type(name="mesh_y.inp",dest="archive",copy_opp=file_type().CHECK_VER_THEN_COPY,base_file="mesh_y.inp"))
file_list.append(file_type(name="mesh_z.inp",dest="archive",copy_opp=file_type().CHECK_VER_THEN_COPY,base_file="mesh_z.inp"))
file_list.append(file_type(name="dump_file.inp",dest="archive",copy_opp=file_type().CHECK_VER_THEN_COPY,base_file="dump_file.inp"))
file_list.append(file_type(name="time_mesh_config",dest="archive",copy_opp=file_type().CHECK_VER_THEN_COPY,base_file="time_mesh_config0.inp"))
file_list.append(file_type(name="homo",dest="archive",copy_opp=file_type().CHECK_VER_THEN_COPY,base_file="homo0.inp"))
file_list.append(file_type(name="lumo",dest="archive",copy_opp=file_type().CHECK_VER_THEN_COPY,base_file="lumo0.inp"))


file_list.append(file_type(name="dos",dest="archive",copy_opp=file_type().MERGE,base_file=os.path.join(get_default_material_path(),"dos.inp")))
file_list.append(file_type(name="pl",dest="archive",copy_opp=file_type().MERGE,base_file=os.path.join(get_default_material_path(),"pl.inp")))
file_list.append(file_type(name="pulse",dest="archive",copy_opp=file_type().MERGE,base_file="pulse0.inp"))
file_list.append(file_type(name="laser",dest="archive",copy_opp=file_type().MERGE,base_file="laser0.inp"))
file_list.append(file_type(name="jv",dest="archive",copy_opp=file_type().MERGE,base_file="jv0.inp"))

file_list.append(file_type(name="ver.inp",copy_opp=file_type().MERGE))
file_list.append(file_type(name="sim.ref",copy_opp=file_type().JUST_COPY))
file_list.append(file_type(name="sim.inp",copy_opp=file_type().MERGE))
file_list.append(file_type(name="device.inp",copy_opp=file_type().MERGE))
file_list.append(file_type(name="parasitic.inp",copy_opp=file_type().MERGE))
file_list.append(file_type(name="led.inp",copy_opp=file_type().MERGE))
file_list.append(file_type(name="ray.inp",copy_opp=file_type().MERGE))
file_list.append(file_type(name="stark.inp",copy_opp=file_type().MERGE))
file_list.append(file_type(name="shg.inp",copy_opp=file_type().MERGE))
file_list.append(file_type(name="math.inp",copy_opp=file_type().MERGE))
file_list.append(file_type(name="dump.inp",copy_opp=file_type().MERGE))
file_list.append(file_type(name="light.inp",copy_opp=file_type().MERGE))
file_list.append(file_type(name="server.inp",copy_opp=file_type().MERGE))
file_list.append(file_type(name="cluster.inp",copy_opp=file_type().MERGE))
file_list.append(file_type(name="light_exp.inp",copy_opp=file_type().MERGE))
file_list.append(file_type(name="info.inp",copy_opp=file_type().MERGE))
file_list.append(file_type(name="config.inp",copy_opp=file_type().MERGE))


def get_file_info(file_name):
	match = re.match(r"([a-z_]+)([0-9]+)(.inp)", file_name, re.I)
	if match==None:
		match = re.match(r"([a-z_]+)([0-9]+)(.dat)", file_name, re.I)

	if match!=None:
		for i in range(0,len(file_list)):
			if file_list[i].name==match.groups()[0]:
				return file_list[i]
	
	for i in range(0,len(file_list)):
		if file_list[i].name==file_name:
			return file_list[i]
		
	return False
	
def update_simulaton_to_new_ver(file_name):
	pre, ext = os.path.splitext(file_name)
	back_file = pre + ".bak"


	if os.path.isfile(back_file)==False:
		archive_compress(file_name)
		os.rename(file_name, back_file)

		dest_dir = os.path.dirname(file_name)

		pre, ext = os.path.splitext(file_name)
		dest_archive = pre + ".gpvdm"

		gpvdm_clone(dest_dir,copy_dirs=False)

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
			file_info=get_file_info(my_file)
			if file_info!=False:
				if file_info.index_file==True:
					if ls_src.count(my_file)==0:
						zip_remove_file(dest_archive,my_file)


def merge_archives(src_archive,dest_archive,only_over_write):
	debug=False

	progress_window=progress_class()
	progress_window.show()
	progress_window.start()

	process_events()

#	src_dir=os.path.dirname(src_archive)
#	dest_dir=os.path.dirname(dest_archive)
	dest_path=os.path.dirname(dest_archive)
	template_archive=os.path.join(get_inp_file_path(),"base.gpvdm")

	remove_non_used_index_files(dest_archive,src_archive)

	ls=zip_lsdir(src_archive)

	#copy files without checking ver

	for i in range(0,len(ls)):
		info=get_file_info(ls[i])
		if info!=False:
			if info.copy_opp==file_type().JUST_COPY:
				#print(ls[i])
				archive_copy_file(dest_archive,ls[i],src_archive,ls[i],dest=info.dest)

			if info.copy_opp==file_type().CHECK_VER_THEN_COPY:
				template_ver=inp_get_file_ver(template_archive,info.base_file)
				src_ver=inp_get_file_ver(src_archive,ls[i])

				if template_ver!="" and src_ver!="":
					if template_ver==src_ver:
						archive_copy_file(dest_archive,ls[i],src_archive,ls[i])
						#print("complex copy")

			if info.copy_opp==file_type().MERGE:
				if only_over_write==False:
					if archive_isfile(dest_archive,ls[i])==False:
						if archive_copy_file(dest_archive,ls[i],template_archive,info.base_file)==False:
							print("problem copying",template_archive,info.base_file)
						#print("made new file",dest_archive,ls[i])

				ret=archive_merge_file(dest_archive,src_archive,ls[i])
		
		progress_window.set_fraction(float(i)/float(len(ls)))
		progress_window.set_text("Importing "+ls[i])
		process_events()

	#if you find a materials directory in the archive try to merge it
	for i in range(0,len(ls)):
		zip_dir_name=ls[i].split("/")
		if zip_dir_name[0]=="materials":
			dest=os.path.join(os.path.dirname(get_materials_path()))
			#print("Try to read",src_archive,ls[i],dest)
			extract_file_from_archive(dest,src_archive,ls[i])

		if zip_dir_name[0]=="sim":
			extract_file_from_archive(dest_path,src_archive,ls[i])

		if zip_dir_name[0]=="calibrate":
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
				#print("Try to read",src_archive,ls[i])
				extract_file_from_archive(dest_path,src_archive,ls[i])
	print("search",scan_dirs)

	progress_window.stop()

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
			gpvdm_delete_file(dest)

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
