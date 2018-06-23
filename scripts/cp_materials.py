#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2017 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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


import os
import os.path
import argparse
import shutil
from inp import inp_get_token_value
from cp_gasses import copy_gasses
from cal_path import subtract_paths
from util import str2bool

def safe_cpy(dest,src,f):
	if os.path.isfile(os.path.join(src,f))==True:
		shutil.copyfile(os.path.join(src,f),os.path.join(dest,f))

def cp_spectra(dest,src):
	src=os.path.join(src,"spectra")
	dest=os.path.join(dest,"spectra")

	for dirpath, dirnames, filenames in os.walk(src):
		for filename in [f for f in filenames if f=="mat.inp"]:
			mat_f_name=os.path.join(dirpath, filename)
			status=inp_get_token_value(mat_f_name, "#status")

			if status=="public_all" or status=="public":
				print("copy spectra",mat_f_name,status)
				src_mat_path=os.path.dirname(mat_f_name)

				delta_path=subtract_paths(src,src_mat_path)
				dst_mat_path=os.path.join(dest,delta_path)

				if not os.path.exists(dst_mat_path):
					os.makedirs(dst_mat_path)
				
				safe_cpy(dst_mat_path,src_mat_path,"spectra_gen.inp")
				
				safe_cpy(dst_mat_path,src_mat_path,"spectra.inp")
				
				safe_cpy(dst_mat_path,src_mat_path,"mat.inp")

				safe_cpy(dst_mat_path,src_mat_path,"spectra_eq.inp")
				

def cp_devices(dest,src):
	src=os.path.join(src,"device_lib")
	dest=os.path.join(dest,"device_lib")

	if not os.path.exists(dest):
		os.makedirs(dest)

	for dirpath, dirnames, files in os.walk(src):
		for name in files:
			if name.endswith(".gpvdm")==True:
				src_file=os.path.join(dirpath, name)
				dst_file=os.path.join(dest,subtract_paths(src,src_file))
				dst_dir=os.path.dirname(dst_file)

				private=inp_get_token_value(os.path.join(os.path.dirname(src_file),"info.inp"), "#private",archive=os.path.basename(src_file))

				if private!=None:
					if str2bool(private)==False:
						if os.path.isdir(dst_dir)==False:
							os.makedirs(dst_dir)
						shutil.copyfile(src_file,dst_file)


def cp_materials(dest,src):
	
	dest=os.path.join(dest,"materials")
	src=os.path.join(src,"materials")

	print(dest,src)
	for dirpath, dirnames, filenames in os.walk(src):
		for filename in [f for f in filenames if f=="mat.inp"]:
			mat_f_name=os.path.join(dirpath, filename)
			status=inp_get_token_value(mat_f_name, "#status")

			if status=="public_all" or status=="public":
				print("copy materials",mat_f_name,status)
				src_mat_path=os.path.dirname(mat_f_name)

				delta_path=subtract_paths(src,src_mat_path)
				dst_mat_path=os.path.join(dest,delta_path)
				if not os.path.exists(dst_mat_path):
					os.makedirs(dst_mat_path)
				
				safe_cpy(dst_mat_path,src_mat_path,"alpha_gen.omat")
				
				safe_cpy(dst_mat_path,src_mat_path,"n_gen.omat")
				
				safe_cpy(dst_mat_path,src_mat_path,"n_eq.inp")
				
				safe_cpy(dst_mat_path,src_mat_path,"alpha_eq.inp")
				
				safe_cpy(dst_mat_path,src_mat_path,"dos.inp")

				safe_cpy(dst_mat_path,src_mat_path,"pl.inp")

				safe_cpy(dst_mat_path,src_mat_path,"mat.inp")
				safe_cpy(dst_mat_path,src_mat_path,"fit.inp")

				safe_cpy(dst_mat_path,src_mat_path,"cost.xlsx")


				safe_cpy(dst_mat_path,src_mat_path,"alpha.omat")
				safe_cpy(dst_mat_path,src_mat_path,"n.omat")
				safe_cpy(dst_mat_path,src_mat_path,"alpha.ref")
				safe_cpy(dst_mat_path,src_mat_path,"n.ref")


				files=os.listdir(src_mat_path)
				for i in range(0,len(files)):
					if files[i].endswith(".ref")==True:
						safe_cpy(dst_mat_path,src_mat_path,files[i])
