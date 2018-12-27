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

## @package materials_io
#  io functions related to the materials data base.
#

import os
import zipfile
import shutil
from inp import inp_search_token_value
from inp import inp_get_token_value
from util_zip import read_lines_from_archive
from cal_path import get_materials_path

def archive_materials(path):
	for root, dirs, files in os.walk(path):
		for file in files:
			if file.endswith("mat.inp"):
				mat_file=os.path.join(root, file)
				mat_dir=os.path.dirname(mat_file)
				zip_file=mat_dir+".zip"
				zf = zipfile.ZipFile(zip_file, 'a',zipfile.ZIP_DEFLATED)
				files=os.listdir(mat_dir)

				for mfile in files:
					m_file=os.path.join(mat_dir,mfile)
					if os.path.isfile(m_file):
						f=open(m_file, mode='rb')
						lines = f.read()
						f.close()

						zf.writestr(mfile, lines)

				zf.close()
				shutil.rmtree( mat_dir )
				print(mat_dir)

def is_mat_file(file_path):
	if os.path.isdir(file_path)==True:
		gpvdm_file_type=inp_get_token_value(os.path.join(file_path,"mat.inp"), "#gpvdm_file_type")
		if gpvdm_file_type=="mat":
			return True
		return False

	zfile=file_path

	if os.path.isfile(zfile)==False:
		zfile=zfile+".zip"

	if os.path.isfile(zfile)==True:
		lines=read_lines_from_archive(zfile,"mat.inp",mode="l")

		if lines!=False:
			gpvdm_file_type=inp_search_token_value(lines, "#gpvdm_file_type")

			if gpvdm_file_type=="mat":
				return True

		return False


def find_materials():
	ret=[]
	mat_path=get_materials_path()
	for root, dirs, files in os.walk(mat_path):
		for file in files:
			path=os.path.join(root, file)
			if is_mat_file(path)==True:
				if os.path.isdir(path)==True:
					path=os.path.dirname(path)
				else:
					path=path[:-4]
				s=os.path.relpath(path, mat_path)
				s=s.replace("\\","/")

				ret.append(s)
	
	return ret

