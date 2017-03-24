#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
#
#	www.gpvdm.com
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


#import sys
import os
import shutil
#import glob
from cal_path import get_inp_file_path
from util_zip import zip_lsdir
from util_zip import read_lines_from_archive
from util_zip import write_lines_to_archive
from util_zip import archive_make_empty
from shutil import copyfile
from inp_util import inp_search_token_value
from cal_path import get_materials_path
from cp_gasses import copy_gasses

def gpvdm_clone(dest,copy_dirs,materials=["all"]):
	src_dir=get_inp_file_path()
	src_archive=os.path.join(src_dir,"sim.gpvdm")
	dest_archive=os.path.join(dest,"sim.gpvdm")
	print(src_archive)
	files=zip_lsdir(src_archive)
	lines=[]

	archive_make_empty(dest_archive)

	for i in range(0,len(files)):
		if files[i].endswith(".inp"):
			read_lines_from_archive(lines,src_archive,files[i])
			write_lines_to_archive(dest_archive,files[i],lines)


	if copy_dirs==True:
		if os.path.isdir(os.path.join(src_dir,"plot")):
			shutil.copytree(os.path.join(src_dir,"plot"), os.path.join(dest,"plot"))

		if os.path.isdir(os.path.join(src_dir,"exp")):
			shutil.copytree(os.path.join(src_dir,"exp"), os.path.join(dest,"exp"))

		if os.path.isdir(os.path.join(src_dir,"materials")):
			shutil.copytree(os.path.join(src_dir,"materials"), os.path.join(dest,"materials"))

		clone_materials(dest,materials)

def clone_materials(dest,materials=["all"]):
	src_dir=os.path.join(get_materials_path())
	dest_dir=os.path.join(dest,"materials")
	if os.path.isdir(dest_dir)==False:
		os.mkdir(dest_dir)
	copy_gasses(dest_dir,src_dir)

	files=os.listdir(src_dir)
	for i in range(0,len(files)):
		src_file=os.path.join(src_dir,files[i])
		dest_file=os.path.join(dest_dir,files[i])

		if files[i].endswith(".spectra"):
			copyfile(src_file, dest_file)

		if os.path.isdir(src_file)==True:
			lines=[]
			mat_sub_path=os.path.join("materials",files[i],"mat.inp")
			if read_lines_from_archive(lines,os.path.join(get_inp_file_path(),"sim.gpvdm"),mat_sub_path)==True:
				do_copy=False
				mat_type=inp_search_token_value(lines, "#material_type")
				if mat_type!=False:
					if materials.count("all")!=0:
						do_copy=True

					if materials.count(mat_type)!=0:
						do_copy=True


				if do_copy==True:
					print("copy",dest_file)
					if os.path.isdir(dest_file)==False:
						os.mkdir(dest_file)

					for copy_file in ["alpha_eq.inp","alpha.omat","dos.inp","info.txt","n_eq.inp","n.omat","alpha_gen.omat","cost.xlsx","fit.inp","mat.inp","n_gen.omat","pl.inp"]:
						src_mat_file=os.path.join(src_file,copy_file)
						if os.path.isfile(src_mat_file)==True:
							copyfile(src_mat_file,os.path.join(dest_file,copy_file))
				else:
					print("not copy",dest_file)

