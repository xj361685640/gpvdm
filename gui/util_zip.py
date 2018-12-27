#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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

## @package util_zip
#  Functions to manipulate zip files/.gpvdm files
#

import os
import shutil
from tempfile import mkstemp
import zipfile
from inp_util import inp_merge

from cal_path import subtract_paths
import time

def archive_copy_file(dest_archive,dest_file_name,src_archive,file_name,dest="archive"):
	lines=read_lines_from_archive(src_archive,file_name)
	if lines==False:
		return False

	write_lines_to_archive(dest_archive,dest_file_name,lines,dest=dest)
	return True




def archive_make_empty(archive_path):
		zf = zipfile.ZipFile(archive_path, 'w',zipfile.ZIP_DEFLATED)

		#zf.writestr("gpvdm.txt", "")
		zf.close()

def zip_lsdir(file_name,zf=None,sub_dir=None):
	"""Input: path to a .gpvdm file"""
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
		do_close=False
		if zf==None:
			do_close=True
			zf = zipfile.ZipFile(file_name, 'r')

		items=zf.namelist()
		items.extend(my_list)

#		print(items)
		my_list=list(set(items))

#		print(my_list)
		#print(my_list[0])
		#asdsa


		if sub_dir!=None:
			l=[]
			level=sub_dir.count("/")
			for i in range(0,len(my_list)):
				archive_file=my_list[i]

				if archive_file.startswith(sub_dir)==True or sub_dir=="/":
					s=archive_file.split("/")
					if len(s)>level:
						s=s[0:level]
						l.append("/".join(s))

			my_list=list(set(l))

		if do_close==True:
			zf.close()

		my_list=sorted(my_list)
		return my_list

	return False

def zip_get_data_file(file_name):
	found=False
	lines=[]
	zip_file=os.path.dirname(file_name)+".zip"
	if os.path.isfile(zip_file)==False:
		zip_file=os.path.join(os.path.dirname(file_name),"sim.gpvdm")

	name=os.path.basename(file_name)
	if os.path.isfile(file_name)==True:
		f = open(file_name, mode='rb')
		lines = f.read()
		f.close()
		found=True

	if os.path.isfile(zip_file) and found==False:
		zf = zipfile.ZipFile(zip_file, 'r')
		items=zf.namelist()
		if items.count(name)>0:
			lines = zf.read(name)
			found=True
		zf.close()
	try:
		lines=lines.decode('utf-8')
		lines=lines.split("\n")
	except:
		lines=[]
		
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


def replace_file_in_zip_archive(zip_file_name,target,lines,mode="l",delete_first=True):
	if os.path.isfile(zip_file_name)==True:
		if delete_first==True:
			zip_remove_file(zip_file_name,target)

		zf = zipfile.ZipFile(zip_file_name, 'a',zipfile.ZIP_DEFLATED)

		if mode=="l":
			build='\n'.join(lines)

		if mode=="b":
			build=lines

		zf.writestr(target, build)

		zf.close()

		#print(">>>>>>>>>>>>>>>>>>>>",abs_path, zip_file_name)
		return True
	else:
		return False

def zip_search_file(source,target):
	for file in source.filelist:
		if file.filename==target:
			return True
	return False




def zip_remove_file(zip_file_name,target):
	file_path=os.path.join(os.path.dirname(zip_file_name),target)
	#if has_handle(zip_file_name)==True:
	#	print("We are already open!",zip_file_name)
	#	exit(0)
	if os.path.isfile(file_path)==True:
		os.remove(file_path)


	if archive_isfile(zip_file_name,target)==True:
		source = zipfile.ZipFile(zip_file_name, 'r')

		found=zip_search_file(source,target)

		if found==True:
			fh, abs_path = mkstemp()
			zf = zipfile.ZipFile(abs_path, 'w',zipfile.ZIP_DEFLATED)

			for file in source.filelist:
				if not file.filename.startswith(target):
					zf.writestr(file.filename, source.read(file))

			zf.close()
			os.close(fh)

		source.close()

		if found==True:
			#I think virus killers are opening the file on windows
			i=0
			while(i<3):
				try:
					if os.path.isfile(zip_file_name):
						os.remove(zip_file_name)
					shutil.move(abs_path, zip_file_name)
					return
				except:
					print("Waiting for the file to close: ",zip_file_name)
					print("... file a bug report if gpvdm does not work because of this.")
					time.sleep(1)
				i=i+1
			#failed three times to open the file, so try again and expect to fail
			if os.path.isfile(zip_file_name):
				os.remove(zip_file_name)
			shutil.move(abs_path, zip_file_name)
			return

def write_lines_to_archive(archive_path,file_name,lines,mode="l",dest="archive"):

	file_path=os.path.join(os.path.dirname(archive_path),file_name)

	if os.path.isfile(file_path)==True or os.path.isfile(archive_path)==False or dest=="file":
		zip_remove_file(archive_path,file_name)

		if mode=="l":
			dump=""
			for item in lines:
				dump=dump+item+"\n"

			dump=dump.rstrip("\n")
		if mode=="b":
			dump=lines
			
		f=open(file_path, mode='wb')
		lines = f.write(str.encode(dump))
		f.close()
		return True
	else:
		return replace_file_in_zip_archive(archive_path,file_name,lines,mode=mode)

def archive_compress(archive_path):
	if os.path.isfile(archive_path)==False:
		archive_make_empty(archive_path)

	if os.path.isfile(archive_path)==True:
		lines=[]
		dir_name=os.path.dirname(archive_path)
		for file_name in os.listdir(dir_name):
			full_name=os.path.join(dir_name,file_name)
			if file_name.endswith(".inp")==True:
				lines=read_lines_from_archive(archive_path,file_name)
				os.remove(full_name)
				replace_file_in_zip_archive(archive_path,file_name,lines,delete_first=False)


def archiv_move_file_to_archive(archive_path,file_name,base_dir,dont_delete=False):
	archive_add_file(archive_path,file_name,base_dir,dont_delete)
	os.remove(file_name)

def archive_add_file(archive_path,file_name,base_dir,dont_delete=False):
		lines=[]
		name_of_file_in_archive=subtract_paths(base_dir,file_name)#file_name[len(base_dir):]

		if dont_delete==False:
			zip_remove_file(archive_path,name_of_file_in_archive)

		if os.path.isfile(file_name):
			f=open(file_name, mode='rb')
			lines = f.read()
			f.close()
		else:
			return False

		zf = zipfile.ZipFile(archive_path, 'a',zipfile.ZIP_DEFLATED)

		zf.writestr(name_of_file_in_archive, lines)
		zf.close()
		return True

def archive_add_dir(archive_path,dir_name,base_dir, remove_src_dir=False,zf=None):

	close_file=False

	if zf==None:
		close_file=True
		zf = zipfile.ZipFile(archive_path, 'a',zipfile.ZIP_DEFLATED)

	for root, dirs, files in os.walk(dir_name):
		for name in files:
			file_name=os.path.join(root, name)

			if os.path.isfile(file_name):
				f=open(file_name, mode='rb')
				lines = f.read()
				f.close()

				name_of_file_in_archive=subtract_paths(base_dir,file_name)
				zf.writestr(name_of_file_in_archive, lines)


	if close_file==True:
		zf.close()

	if remove_src_dir==True: 
		if base_dir=="" or base_dir==dir_name or dir_name=="/" or dir_name=="/home/rod" or dir_name=="/home/rod/" or dir_name=="c:\\":	#paranoia
			return

		shutil.rmtree(dir_name)


def read_lines_from_archive(zip_file_path,file_name,mode="l"):
	file_path=os.path.join(os.path.dirname(zip_file_path),file_name)

	read_lines=[]
	found=False

	if os.path.isfile(file_path):	#for /a/b/c/sim.gpvdm a.dat, check /a/b/c/a.dat
		f=open(file_path, mode='rb')
		read_lines = f.read()
		f.close()
		found=True
	
	if found==False:					#for /a/b/c/sim.gpvdm a.dat, check /a/b/c/sim/a.dat
		file_path=os.path.join(zip_file_path[:-4],file_name)
		if os.path.isfile(file_path):
			f=open(file_path, mode='rb')
			read_lines = f.read()
			f.close()
			found=True

	if found==False:
		if os.path.isfile(zip_file_path):
			zip_file_open_ok=True
			try:
				zf = zipfile.ZipFile(zip_file_path, 'r')
			except:
				zip_file_open_ok=False

			if zip_file_open_ok==True:
				if zip_search_file(zf,os.path.basename(file_path))==True:
					read_lines = zf.read(file_name)
					found=True
				elif zip_search_file(zf,file_name)==True:
					read_lines = zf.read(file_name)
					found=True

				zf.close()

	if found==False:
		return False

	#print(">",file_path,"<",read_lines)
	if mode=="l":
		read_lines=read_lines.decode('utf-8')#.decode("utf-8") 
		read_lines=read_lines.split("\n")

		lines=[]

		for i in range(0, len(read_lines)):
			lines.append(read_lines[i].rstrip())

		if lines[len(lines)-1]=='\n':
			del lines[len(lines)-1]
	elif mode=="b":
		lines=read_lines


	return lines

## This will unpack an archive leaving a .gpvdm file with a ver.inp file inside it
def archive_unpack(zip_file_path):
	dir_name=os.path.dirname(zip_file_path)
	ver_file=os.path.join(dir_name,"ver.inp")
	archive_decompress(zip_file_path)
	archive_make_empty(zip_file_path)
	#archiv_move_file_to_archive(zip_file_path,ver_file,dir_name)

## This will unpack an acrhive removing the .gpvdm file
def archive_decompress(zip_file_path):

	if os.path.isfile(zip_file_path):
		zf = zipfile.ZipFile(zip_file_path, 'r')
		for file_name in zf.filelist:
			read_lines = zf.read(file_name)

			f=open(os.path.join(os.path.dirname(zip_file_path),file_name.filename), mode='wb')
			f.write(read_lines)
			f.close()

		zf.close()

		os.remove(zip_file_path)

	return

def extract_file_from_archive(dest,zip_file_path,file_name):

	file_path=os.path.join(os.path.dirname(zip_file_path),file_name)

	read_lines=[]

	if os.path.isfile(file_path):
		f=open(file_path, mode='rb')
		read_lines = f.read()
		f.close()
	else:
		found=False

		if os.path.isfile(zip_file_path):
			zf = zipfile.ZipFile(zip_file_path, 'r')
			if zip_search_file(zf,os.path.basename(file_path))==True:
				read_lines = zf.read(file_name)
				found=True
			elif zip_search_file(zf,file_name)==True:
				read_lines = zf.read(file_name)
				found=True

			zf.close()

		if found==False:
			print("not found",file_name)
			return False

	if file_name.endswith("/")==True:
		if os.path.isdir(os.path.join(dest,file_name))==False:
			os.makedirs(os.path.join(dest,file_name))
		return True
	else:
		if os.path.isdir(os.path.join(dest,os.path.dirname(file_name)))==False:
			os.makedirs(os.path.join(dest,os.path.dirname(file_name)))

		f=open(os.path.join(dest,file_name), mode='wb')
		lines = f.write(read_lines)
		f.close()

	return True


def extract_dir_from_archive(dest,zip_file_path,dir_name,zf=None):
	items=zf.namelist()
	for i in range(0,len(items)):
		if items[i].startswith(dir_name)==True:
			read_lines = zf.read(items[i])
			output_file=os.path.join(dest,subtract_paths(dir_name,items[i]))
			output_dir=os.path.dirname(output_file)

			if os.path.isdir(output_dir)==False:
				os.makedirs(output_dir)
#			print(output_file,"'"+items[i]+"'",zip_file_path)

			if items[i].endswith('/')==False:	#test if it is not a dir
				f=open(output_file, mode='wb')
				lines = f.write(read_lines)
				f.close()

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

	src_lines=read_lines_from_archive(src_archive,file_name)

	if src_lines==False:
		print("Warning: ",src_archive,file_name," no origonal file to copy")
		return False

	dest_lines=read_lines_from_archive(dest_archive,file_name)

	if dest_lines==False:
		print("Warning: ",dest_archive,file_name," no final copy found")
		return False

	errors=inp_merge(dest_lines,src_lines)
	if len(errors)!=0:
		print("File ",file_name,errors)

	write_lines_to_archive(dest_archive,file_name,dest_lines)

	return True


