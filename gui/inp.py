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
from util_zip import replace_file_in_zip_archive
#import subprocess
from tempfile import mkstemp
#import logging
#import zipfile
from util_zip import zip_remove_file
from util_zip import write_lines_to_archive
from util_zip import read_lines_from_archive
from util_zip import archive_isfile
from util_zip import zip_lsdir


def inp_issequential_file(data,search):
	if data.startswith(search) and data.endswith("inp"):
		cut=data[len(search):-4]
		#print(cut)
		return cut.isdigit()
	else:
		return False

def inp_lsdir():
	return zip_lsdir("sim.gpvdm")

def inp_remove_file(file_name):
	zip_remove_file("sim.gpvdm",file_name)

def inp_read_next_item(lines,pos):
	token=lines[pos]
	pos=pos+1
	value=lines[pos]
	pos=pos+1
	return token,value,pos

def inp_replace_multi_line_token(lines,token,replace):
	new_lines=[]
	pos=0
	for i in range(0, len(lines)):
		new_lines.append(lines[i])
		if lines[i]==token:
			new_lines.append(replace)
			pos=i+1
			break

	start=False
	for i in range(pos, len(lines)):
		if len(lines[i])>0:
			if lines[i][0]=="#":
				start=True
				
		if start==True:
			new_lines.append(lines[i])
	return new_lines

def inp_update(file_path, token, replace):
	inp_update_token_value(file_path, token, replace,1)

def inp_update_token_value(file_path, token, replace,line_number):
	lines=[]
	if token=="#Tll":
		inp_update_token_value("thermal.inp", "#Tlr", replace,1)
		files=inp_lsdir(file_path)
		for i in range(0,len(files)):
			if files[i].startswith("dos") and files[i].endswith(".inp"):

				inp_update_token_value(files[i], "#Tstart", replace,1)
				try:
					upper_temp=str(float(replace)+5)
				except:
					upper_temp="300.0"
				inp_update_token_value(files[i], "#Tstop", upper_temp,1)

	path=os.path.dirname(file_path)

	zip_file_name=os.path.join(path,"sim.gpvdm")

	read_lines_from_archive(lines,zip_file_name,os.path.basename(file_path))


	new_lines=inp_replace_multi_line_token(lines,token,replace)


	if os.path.isfile(file_path):
		fh, abs_path = mkstemp()

		dump='\n'.join(new_lines)

		dump=dump.rstrip("\n")
		dump=dump.encode('ascii')
		f=open(abs_path, mode='wb')
		written = f.write(dump)
		f.close()

		os.close(fh)
		shutil.move(abs_path, file_path)
	else:
		replace_file_in_zip_archive(zip_file_name,os.path.basename(file_path),new_lines)

def inp_isfile(file_path):

	zip_file_name=os.path.join(os.path.dirname(file_path),"sim.gpvdm")
	return archive_isfile(zip_file_name,os.path.basename(file_path))

def inp_copy_file(dest,src):
	lines=[]
	if inp_load_file(lines,src)==True:
		inp_write_lines_to_file(dest,lines)
		return True
	else:
		return False

def inp_load_file(lines,file_path):
	zip_file_path=os.path.join(os.path.dirname(file_path),"sim.gpvdm")
	file_name=os.path.basename(file_path)
	return read_lines_from_archive(lines,zip_file_path,file_name)

def inp_write_lines_to_file(file_path,lines):
	archive_path=os.path.join(os.path.dirname(file_path),"sim.gpvdm")
	file_name=os.path.basename(file_path)
	return write_lines_to_archive(archive_path,file_name,lines)

def inp_save_lines(file_path,lines):
	dump=""
	for item in lines:
		#print(type(dump),type(item),item)
		dump=dump+item+"\n"

	dump=dump.rstrip("\n")
	dump=dump.encode('ascii')
	try:
		f=open(file_path, mode='wb')
	except:
		return False

	lines = f.write(dump)
	f.close()
	return True

def inp_new_file():
	ret=[]
	ret.append("#ver")
	ret.append("1.0")
	ret.append("#end")
	return ret


def inp_get_next_token_array(lines,pos):

	ret=[]
	ret.append(lines[pos])
	pos=pos+1
	while (lines[pos][0]!="#"):
		ret.append(lines[pos])
		pos=pos+1

	return ret,pos

def inp_get_token_array(file_path, token):

	lines=[]
	ret=[]
	inp_load_file(lines,file_path)

	for i in range(0, len(lines)):
		if lines[i]==token:
			pos=i+1
			for ii in range(pos,len(lines)):
				if len(lines[ii])>0:
					if lines[ii][0]=="#":
						return ret
				
				ret.append(lines[ii])

	return False

def inp_get_token_value(file_path, token):

	lines=[]
	inp_load_file(lines,file_path)

	for i in range(0, len(lines)):
		if lines[i]==token:
			return lines[i+1]

	return None

def inp_sum_items(lines,token):
	my_sum=0.0
	for i in range(0, len(lines)):
		if lines[i].startswith(token)==True:
			my_sum=my_sum+float(lines[i+1])

	return my_sum

