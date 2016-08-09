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
#import signal
#import subprocess
from tempfile import mkstemp
#import logging
import zipfile
#import re
#from numpy import zeros
#import hashlib
#import glob
#from win_lin import running_on_linux
#from cal_path import get_inp_file_path
from inp_util import inp_merge
from inp_util import inp_search_token_value

def archive_copy_file(dest_archive,dest_file_name,src_archive,file_name):
	lines=[]
	if read_lines_from_archive(lines,src_archive,file_name)==False:
		return False

	write_lines_to_archive(dest_archive,dest_file_name,lines)
	return True




def archive_make_empty(archive_path):
		zf = zipfile.ZipFile(archive_path, 'w')

		zf.writestr("gpvdm.txt", "")
		zf.close()

def zip_lsdir(file_name):
	my_list=[]
	my_dir=os.path.dirname(file_name)

	if my_dir=="":
		my_dir=os.getcwd()
		file_name=os.path.join(os.getcwd(),file_name)

	if os.path.isdir(my_dir):
		my_list=os.listdir(my_dir)
	else:
		return False

	if os.path.isfile(file_name):
		zf = zipfile.ZipFile(file_name, 'r')
		items=zf.namelist()
		for i in range(0,len(items)):
			if my_list.count(items[i])==0:
				my_list.append(items[i])
		zf.close()
		return my_list

	return False

def zip_get_data_file(file_name):
	found=False
	lines=[]
	zip_file=os.path.dirname(file_name)+".zip"
	name=os.path.basename(file_name)
	if os.path.isfile(file_name)==True:
		f = open(file_name)
		lines = f.readlines()
		f.close()
		found=True

	if os.path.isfile(zip_file) and found==False:
		zf = zipfile.ZipFile(zip_file, 'r')
		items=zf.namelist()
		if items.count(name)>0:
			lines = zf.read(name).split("\n")
			found=True
		zf.close()

	return [found,lines]

def check_is_config_file(name):
	found="none"
	if os.path.isfile(name)==True:
		found=True
		return "file"
	if os.path.isfile("sim.gpvdm"):
		zf = zipfile.ZipFile('sim.gpvdm', 'r')
		items=zf.namelist()
		if items.count(name)>0:
			found="archive"
		zf.close()

	return found


def replace_file_in_zip_archive(zip_file_name,target,lines):
	fh, abs_path = mkstemp()
	source = zipfile.ZipFile(zip_file_name, 'r')

	zf = zipfile.ZipFile(abs_path, 'w')

	for file in source.filelist:
		if not file.filename.startswith(target):
			zf.writestr(file.filename, source.read(file))

	source.close()

	build='\n'.join(lines)

	#for i in range(0,len(lines)):
	#	build=build+lines[i]+"\n"

	zf.writestr(target, build)

	zf.close()
	os.close(fh)
	shutil.move(abs_path, zip_file_name)

def zip_search_file(source,target):
	for file in source.filelist:
		if file.filename==target:
			return True
	return False

def zip_remove_file(zip_file_name,target):
	file_path=os.path.join(os.path.dirname(zip_file_name),target)

	if os.path.isfile(file_path)==True:
		os.remove(file_path)

	if os.path.isfile(zip_file_name):
		source = zipfile.ZipFile(zip_file_name, 'r')

		found=zip_search_file(source,target)

		if found==True:
			fh, abs_path = mkstemp()
			zf = zipfile.ZipFile(abs_path, 'w')

			for file in source.filelist:
				if not file.filename.startswith(target):
					zf.writestr(file.filename, source.read(file))

			zf.close()
			os.close(fh)

		source.close()

		if found==True:
			shutil.move(abs_path, zip_file_name)


def write_lines_to_archive(archive_path,file_name,lines):

	file_path=os.path.join(os.path.dirname(archive_path),file_name)

	if os.path.isfile(file_path)==True:  # or os.path.isfile(zip_file_name)==False
		zip_remove_file(archive_path,file_name)

		dump=""
		for item in lines:
			dump=dump+item+"\n"

		dump=dump.rstrip("\n")

		f=open(file_path, mode='wb')
		lines = f.write(dump)
		f.close()

	else:
		replace_file_in_zip_archive(archive_path,file_name,lines)

def archive_add_file(archive_path,file_name,base_dir):
		lines=[]
		name_of_file_in_archive=file_name[len(base_dir):]

		zip_remove_file(archive_path,name_of_file_in_archive)

		if os.path.isfile(file_name):
			f=open(file_name, mode='rb')
			lines = f.read()
			f.close()
		else:
			return False

		zf = zipfile.ZipFile(archive_path, 'a')

		zf.writestr(name_of_file_in_archive, lines)
		zf.close()
		return True

def read_lines_from_archive(lines,zip_file_path,file_name):

	file_path=os.path.join(os.path.dirname(zip_file_path),file_name)

	read_lines=[]

	if os.path.isfile(file_path):
		f=open(file_path, mode='rb')
		read_lines = f.read()
		f.close()
	else:
		if os.path.isfile(zip_file_path):
			zf = zipfile.ZipFile(zip_file_path, 'r')
			if zip_search_file(zf,os.path.basename(file_path))==True:
				read_lines = zf.read(file_name)
				zf.close()
			else:
				zf.close()
				return False
		else:
			return False
	#print(">",file_path,"<",read_lines)
	read_lines=read_lines.decode('ascii')#.decode("utf-8") 
	read_lines=read_lines.split("\n")

	del lines[:]

	for i in range(0, len(read_lines)):
		lines.append(read_lines[i].rstrip())

	if lines[len(lines)-1]=='\n':
		del lines[len(lines)-1]

	return True

def archive_isfile(zip_file_name,file_name):
	ret=False

	file_path=os.path.join(os.path.dirname(zip_file_name),file_name)

	if os.path.isfile(file_path):
		ret=True
	else:
		if os.path.isfile(zip_file_name):
			zf = zipfile.ZipFile(zip_file_name, 'r')
			if file_name in zf.namelist():
				ret=True
			zf.close()
		else:
			ret=False

	return ret

def archive_merge_file(dest_archive,src_archive,file_name):
	if dest_archive==src_archive:
		print("I can't opperate on the same .gpvdm file")
		return

	src_lines=[]
	dest_lines=[]

	orig_exists=read_lines_from_archive(src_lines,src_archive,file_name)

	if orig_exists==False:
		print("Warning: ",src_archive,file_name," no origonal file to copy")
		return False

	dest_exists=read_lines_from_archive(dest_lines,dest_archive,file_name)

	if dest_exists==False:
		print("Warning: ",dest_archive,file_name," no final copy found")
		return False

	errors=inp_merge(dest_lines,src_lines)
	if len(errors)!=0:
		print("File ",file_name,errors)

	write_lines_to_archive(dest_archive,file_name,dest_lines)

	return True



def archive_get_file_ver(archive,file_name):
	lines=[]
	exists=read_lines_from_archive(lines,archive,file_name)

	if exists==True:
		ver=inp_search_token_value(lines, "#ver")
	else:
		return ""

	return ver
