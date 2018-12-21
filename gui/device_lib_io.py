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

## @package device_lib_io
#  Make changes to the device lib enmass.
#

import os
import glob
from cal_path import get_device_lib_path
from util_zip import archive_add_file
from util_zip import zip_remove_file

def device_lib_replace(file_name,dir_name=""):
	if dir_name=="":
		dir_name=get_device_lib_path()
	archives=glob.glob(os.path.join(dir_name,"*.gpvdm"))
	for i in range(0,len(archives)):
		print("replace ",archives[i],file_name)
		archive_add_file(archives[i],file_name,"")

def device_lib_delete(file_name,dir_name=""):
	if dir_name=="":
		dir_name=get_device_lib_path()
	archives=glob.glob(os.path.join(dir_name,"*.gpvdm"))
	for i in range(0,len(archives)):
		zip_remove_file(archives[i],file_name)
