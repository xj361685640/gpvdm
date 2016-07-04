#!/usr/bin/env python2.7
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



from distutils.core import setup
import py2exe
import os
import sys
import matplotlib
import shutil
import glob

#run with python setup.py py2exe
dest_path=os.path.join(os.getcwd(),"dist")

if os.path.isdir(dest_path)==False:
	os.makedirs(dest_path)
#	shutil.rmtree(dest_path)

print "copying from z:\\auto_dep\\"
for file in glob.glob("z:\\auto_dep\\*"):
	dest=os.path.join(os.getcwd(),os.path.basename(file))
	print file,dest
	if os.path.isfile(dest)==False:
		shutil.copy(file, dest)


	dest=os.path.join(dest_path,os.path.basename(file))
	if os.path.isfile(dest)==False:
		print file,dest
		shutil.copy(file, dest)

includes =['cairo', 'pango', 'pangocairo','atk', 'gobject', 'gio',"matplotlib.backends",  "matplotlib.backends.backend_tkagg"]

dll_excludes = []
#[ "OLEAUT32.dll","USER32.dll","IMM32.DLL","SHELL32.DLL","OLE32.dll","SHLWAPI.DLL","MSVCP90.dll","MSVCP90.dll","COMDLG32.dll","ADVAPI32.dll","ADVAPI32.dll","msvcrt.dll","WS2_32.dll","WINSPOOL.DRV","GDI32.dll","VERSION.dll","KERNEL32.dll","COMCTL32.dll","gdiplus.dll","api-ms-win-core-processthreads-l1-1-2.dll","api-ms-win-core-sysinfo-l1-2-1.dll","api-ms-win-core-errorhandling-l1-1-1.dll","api-ms-win-core-profile-l1-1-0.dll","api-ms-win-core-libraryloader-l1-2-0.dll"]
pack  =[  'gtk','gtk.keysyms']
setup(
		console=['gpvdm.py'],
        options={
				"py2exe":{
				'packages': pack,
				"includes": includes,
				"dll_excludes": dll_excludes}
				},
		data_files=matplotlib.get_py2exe_datafiles()
)

dist=os.path.join(os.getcwd(),"dist","etc")
if os.path.isdir(dist)==False:
	print "copying",dist
	shutil.copytree("c:\\Python27\\Lib\\site-packages\\gtk-2.0\\runtime\\etc", dist)

dist=os.path.join(dest_path,"lib")
if os.path.isdir(dist)==False:
	print "copying",dist
	shutil.copytree("c:\\Python27\\Lib\\site-packages\\gtk-2.0\\runtime\\lib", dist)

dist=os.path.join(dest_path,"share")
if os.path.isdir(dist)==False:
	print "copying",dist
	shutil.copytree("c:\\Python27\\Lib\\site-packages\\gtk-2.0\\runtime\\share", dist)


path_to_del=os.path.join(dest_path,"share","gtk-doc")
if os.path.isdir(path_to_del)==True:
	print "Delete",path_to_del
	shutil.rmtree(path_to_del)

path_to_del=os.path.join(dest_path,"share","locale")
if os.path.isdir(path_to_del)==True:
	print "Delete",path_to_del
	shutil.rmtree(path_to_del)

path_to_del=os.path.join(dest_path,"share","man")
if os.path.isdir(path_to_del)==True:
	print "Delete",path_to_del
	shutil.rmtree(path_to_del)


path_to_del=os.path.join(dest_path,"share","doc")
if os.path.isdir(path_to_del)==True:
	print "Delete",path_to_del
	shutil.rmtree(path_to_del)

path_to_del=os.path.join(dest_path,"share","gtk-2.0")		#only contains a demo folder
if os.path.isdir(path_to_del)==True:
	print "Delete",path_to_del
	shutil.rmtree(path_to_del)

path_to_del=os.path.join(dest_path,"share","pub","gui","dist","tcl","tk8.5","demos")
if os.path.isdir(path_to_del)==True:
	print "Delete",path_to_del
	shutil.rmtree(path_to_del)

path_to_del=os.path.join(dest_path,"share","pub","gui","dist","tcl","tk8.5","images")
if os.path.isdir(path_to_del)==True:
	print "Delete",path_to_del
	shutil.rmtree(path_to_del)

path_to_del=os.path.join(dest_path,"share","mpl-data","sample_data")
if os.path.isdir(path_to_del)==True:
	print "Delete",path_to_del
	shutil.rmtree(path_to_del)

path_to_del=os.path.join(dest_path,"share","icons","Tango","scalable")
if os.path.isdir(path_to_del)==True:
	print "Delete",path_to_del
	shutil.rmtree(path_to_del)
