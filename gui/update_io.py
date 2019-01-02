#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
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


## @package update_io
#  Back end for getting updates
#

import sys
import os
from urllib.parse import urlparse
from cal_path import get_web_cache_path

from util_zip import archive_extract

import re
import urllib.request
#import os


class http_get():
	def __init__(self,address):
		self.address=address

	def go(self):
		print("getting: "+self.address)
		try:
			response=urllib.request.urlopen(self.address)
		except urllib.error.HTTPError as e:
			print("fail")
			return False

		data=b""
		while True:
			chunk = response.read(1024)
			if not chunk:
				break
			data=data+chunk
			sys.stdout.write('.')

		return data



class file_info():
	def __init__(self):
		self.file_name=""
		self.size=-1
		self.md5=""
		self.status="?"
		self.description="none"


class update_cache():
	def __init__(self,web_directory):
		self.file_list=[]
		self.web_directory=web_directory
		self.web_cache_dir=get_web_cache_path(self.web_directory)
		self.load_cache_status()

	def write_cache_status(self):
		a = open(os.path.join(self.web_cache_dir,"info.dat"), "w")
		for f in self.file_list:
			a.write("#"+f.file_name+"\n")
			a.write(f.description+"\n")
			a.write(f.md5+"\n")
			a.write(str(f.size)+"\n")
			a.write(f.status+"\n")

		a.write("#ver\n")
		a.write("1.0\n")
		a.write("#end\n")
		a.close()

	def load_cache_status(self):
		file_path=os.path.join(self.web_cache_dir,"info.dat")
		if os.path.isfile(file_path)==False:
			return False

		f = open(file_path)
		lines = f.readlines()
		f.close()

		self.file_list=[]
		
		for i in range(0, len(lines)):
			lines[i]=lines[i].rstrip()

		pos=0
		while(True):
			if lines[pos]=="#ver":
				break

			a=file_info()
			a.file_name=lines[pos][1:]
			pos=pos+1

			a.description=lines[pos]
			pos=pos+1

			a.md5="hello"#lines[pos]
			pos=pos+1

			a.size=int(lines[pos])
			pos=pos+1

			a.status=lines[pos]
			pos=pos+1

			self.file_list.append(a)

	def to_local_path(self,f):
		return os.path.join(self.web_cache_dir,f.file_name)

	def to_server_path(self,f):
		return "https://www.gpvdm.com/"+self.web_directory+"/"+f.file_name

	def add_to_cache(self,f_in):
		for f in self.file_list:
			print(f.file_name,f_in.file_name,len(self.file_list))
			if f_in.file_name==f.file_name:
				if os.path.isfile(self.to_local_path(f))==False:
					f.status="updata-avaliable"
					return
				if f.md5 != f_in.md5:
					f.status="updata-avaliable"
					return
				
				return

		f_in.status="updata-avaliable"
		self.file_list.append(f_in)
		
	def updates_get(self):
		print("here")
		data=http_get("https://www.gpvdm.com/"+self.web_directory+"/info.dat")
		ret=data.go()

		lines=ret.decode("utf-8").split("\n")

		pos=0
		while(True):
			if lines[pos]=="#ver":
				break

			a=file_info()
			a.file_name=lines[pos][1:]
			pos=pos+1

			a.description=lines[pos]
			pos=pos+1

			a.size=int(lines[pos])
			pos=pos+1

			a.md5=lines[pos]
			pos=pos+1

			self.add_to_cache(a)

		self.write_cache_status()




	def updates_download(self):

		for f in self.file_list:
			if f.status=="updata-avaliable":
				data=http_get(self.to_server_path(f))
				ret=data.go()
				if ret!=False:
					print(len(ret),f.size)

					file_han=open(self.to_local_path(f), mode='wb')
					lines = file_han.write(ret)
					file_han.close()

					f.status="on-disk"

		self.write_cache_status()

	def updates_install(self,target):

		for f in self.file_list:
			if f.status=="on-disk":
				out_sub_dir=f.file_name
				if out_sub_dir.endswith(".zip"):
					out_sub_dir=out_sub_dir[:-4]

				archive_extract(os.path.join(target,out_sub_dir),self.to_local_path(f))
				f.status="up-to-date"

		#self.write_cache_status()
