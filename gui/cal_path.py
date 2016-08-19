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


import sys
import os
#import shutil
from win_lin import running_on_linux

if running_on_linux()==False:
	import winreg

materials_path=None
plugins_path=None
exe_command=None
share_path=None
device_lib_path=None
bin_path=None
lib_path=None
image_path=None
lang_path=None
inp_file_path=None
src_path=None

def remove_cwdfrompath(path):
	tmp=path
	if tmp.startswith(os.getcwd()):
		tmp=tmp[len(os.getcwd())+1:]
	return tmp

def join_path(one,two):
	output_file=os.path.join(one,two)

	if two[0]=='/':
		if one!="" :
			output_file=os.path.join(one,two[1:])

	return output_file


def cal_share_path():
	global share_path

	if os.path.isfile("configure.ac"):
		share_path=os.getcwd()
		return

	if os.path.isfile("ver.py"):
		share_path=os.path.abspath(os.path.join(os.getcwd(), os.pardir))
		return

	if running_on_linux()==False:
		try:
			registry_key = winreg.OpenKey(winreg.HKEY_CURRENT_USER, "Software\\gpvdm", 0, winreg.KEY_READ)
			value, regtype = winreg.QueryValueEx(registry_key, "installpath")
			winreg.CloseKey(registry_key)
			share_path=value
		except WindowsError:
			share_path="c:\\gpvdm"
			print("No registry key found using default",share_path)
	else:
		if os.path.isdir("/usr/lib64/gpvdm"):
			share_path="/usr/lib64/gpvdm/"
		elif os.path.isdir("/usr/lib/gpvdm"):
			share_path="/usr/lib/gpvdm/"
		else:
			share_path="/usr/lib64/gpvdm/"
			print("I don't know where the shared files are assuming ",share_path)

def search_known_paths(file_or_dir_to_find,ext,key_file):
	global share_path
	global bin_path
	#check cwd
	paths=[]
	for ex in ext:
		paths.append(os.path.join(os.getcwd(),file_or_dir_to_find)+ex)
		paths.append(os.path.join(os.path.abspath(os.path.join(os.getcwd(), os.pardir)),file_or_dir_to_find)+ex)
		paths.append(os.path.join(share_path,file_or_dir_to_find)+ex)
		paths.append(os.path.join(bin_path,file_or_dir_to_find)+ex)
		if running_on_linux()==True:
			paths.append(os.path.join("/usr/share/gpvdm/",file_or_dir_to_find)+ex)
			paths.append(os.path.join("/usr/local/bin/",file_or_dir_to_find)+ex)
			paths.append(os.path.join("/usr/bin/",file_or_dir_to_find)+ex)

	for item in paths:
		if key_file==None:
			if os.path.isdir(item) or os.path.isfile(item):
				#print "found",item
				return item
		else:
			if os.path.isfile(os.path.join(item,key_file)):
				return item
	#print "Can't find",file_or_dir_to_find, "setting it to",paths[0]
	return paths[2]

def cal_bin_path():
	global bin_path
	if running_on_linux()==True:
			bin_path="/bin/"
	else:
			bin_path=share_path


def calculate_paths_init():
	cal_share_path()
	cal_bin_path()

def calculate_paths():
	global share_path
	global lib_path
	global exe_command
	global device_lib_path
	global materials_path
	global image_path
	global plugins_path
	global inp_file_path
	global src_path
	global ui_path
	materials_path=search_known_paths("materials",[""],None)
	device_lib_path=search_known_paths("device_lib",[""],None)
	plugins_path=search_known_paths("plugins",[""],None)
	image_path=search_known_paths("images",[""],"image.jpg")
	lang_path=search_known_paths("lang",[""],None)
	exe_command=search_known_paths("gpvdm_core",["",".exe",".o"],None)
	inp_file_path=os.path.dirname(search_known_paths("sim",[".gpvdm"],None))
	src_path=os.path.dirname(search_known_paths("Makefile",[".am"],None))
	ui_path=search_known_paths("ui",[""],None)

def get_share_path():
	global share_path
	return share_path

def get_src_path():
	global src_path
	return src_path

def get_materials_path():
	global materials_path
	return materials_path

def get_device_lib_path():
	global device_lib_path
	return device_lib_path

def get_bin_path():
	global bin_path
	return bin_path

def get_plugins_path():
	global plugins_path
	return plugins_path

def get_exe_command():
	global exe_command
	return exe_command

def get_exe_name():
	global exe_command
	return os.path.basename(exe_command)

def get_inp_file_path():
	global inp_file_path
	return inp_file_path

def get_image_file_path():
	global image_path
	return image_path

def get_lang_path():
	global lang_path
	return lang_path

def get_ui_path():
	global ui_path
	return ui_path
