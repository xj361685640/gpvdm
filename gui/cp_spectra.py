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

parser = argparse.ArgumentParser(epilog="copy spectra database")
parser.add_argument("--copy", help="Copy spectra file", nargs=2)
args = parser.parse_args()

def safe_cpy(dest,src,f):
	if os.path.isfile(os.path.join(src,f))==True:
		shutil.copyfile(os.path.join(src,f),os.path.join(dest,f))

			
if args.copy:
	src=args.copy[0]
	dest=args.copy[1]

	for dirpath, dirnames, filenames in os.walk(args.copy[0]):
		for filename in [f for f in filenames if f=="mat.inp"]:
			mat_f_name=os.path.join(dirpath, filename)
			status=inp_get_token_value(mat_f_name, "#status")

			if status=="public_all" or status=="public":
				print("copy spectra",mat_f_name,status)
				src_mat_path=os.path.dirname(mat_f_name)
				delta_path=src_mat_path[len(src):]
				dst_mat_path=os.path.join(dest,delta_path)
				if not os.path.exists(dst_mat_path):
					os.makedirs(dst_mat_path)
				
				safe_cpy(dst_mat_path,src_mat_path,"spectra_gen.inp")
				
				safe_cpy(dst_mat_path,src_mat_path,"spectra.inp")
				
				safe_cpy(dst_mat_path,src_mat_path,"mat.inp")
				




