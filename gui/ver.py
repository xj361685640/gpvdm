#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012 Roderick C. I. MacKenzie
#
#	roderick.mackenzie@nottingham.ac.uk
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

import os
from cal_path import get_inp_file_path
from cal_path import get_share_path
from cal_path import get_bin_path
from inp import inp_load_file
from inp import inp_update
from util_zip import read_lines_from_archive

global core
global mat
global ver_error

def ver_error():
	global ver_error
	return ver_error

def ver_core():
	global core
	return core

def ver_mat():
	global mat
	return mat

def version():
	global core
	global mat
	string="core: Version "+core+" materials: Version "+mat
	return string

def ver():
	global core
	return str(core)

def ver_load_info():
	lines=[]
	global core
	global mat
	global ver_error

	core=""
	mat=""
	ver_error=""

	ver_file_path=os.path.join(get_inp_file_path(),"ver.inp")

	if inp_load_file(lines,ver_file_path)==True:
		core=lines[1]
		mat=lines[5]
		return True
	else:
		ver_error="I can not find the file sim.gpvdm/ver.inp.\n\nI have tried looking in "+ver_file_path+"\n\nThe share path is"+get_share_path()+"\n\nThe bin path is"+get_bin_path()+"\n\nThe current working dir is "+os.getcwd()+"\n\nTry reinstalling a new version of gpvdm and/or report the bug to me at  roderick.mackenzie@nottingham.ac.uk."
		return False

def ver_sync_ver():
	file_name=os.path.join(os.getcwd(),"include","version.h")
	found=False

	if os.path.isfile(file_name)==True:
		f = open(file_name, "r")
		lines = f.readlines()
		f.close()
		for l in range(0, len(lines)):
			lines[l]=lines[l].rstrip()
			if lines[l].startswith("#define")==True:
				text=(lines[l].split("\t")[2].strip("\""))
				found=True

	if found==True:
		print "seting ver to: ",text
		inp_update("ver.inp","#core",text)
	else:
		print _("version.h not found")


def ver_check_compatibility(file_name):
	lines=[]
	core=""
	mat=""

	if read_lines_from_archive(lines,file_name,"ver.inp")==True:
		core=lines[1]
		if core==ver_core():
			return True
		else:
			return False

	return False


