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


import os
import shutil
import re
import hashlib
import glob
from util_zip import zip_get_data_file

class dat_file():
	x_len=0
	y_len=0
	z_len=0
	x_scale=[]
	y_scale=[]
	z_scale=[]
	data=[]


def dat_file_max_min(my_data):
	my_max=my_data.data[0][0][0]
	my_min=my_data.data[0][0][0]

	for z in range(0,my_data.z_len):
		for x in range(0,my_data.x_len):
			for y in range(0,my_data.y_len):
				
				if my_data.data[z][x][y]>my_max:
					my_max=my_data.data[z][x][y]

				if my_data.data[z][x][y]<my_min:
					my_min=my_data.data[z][x][y]

	return [my_max,my_min]

def dat_file_read(out,file_name):
	if file_name==None:
		return False
	
	found,lines=zip_get_data_file(file_name)
	if found==False:
		return False

	out.x_scale=[]
	out.y_scale=[]
	out.z_scale=[]
	out.data=[]
	
	out.x_len=1
	out.y_len=1
	out.z_len=1

	data_started=False

	x=0
	y=0
	z=0
	dim=0
	for i in range(0, len(lines)):
		temp=lines[i]
		temp=re.sub(' +',' ',temp)
		temp=re.sub('\t',' ',temp)
		s=lines[i].split(" ")


		if len(s)>0:
			if s[0]=="#x":
				out.x_len=int(s[1])

			if s[0]=="#y":
				out.y_len=int(s[1])

			if s[0]=="#z":
				out.z_len=int(s[1])

				out.data=[[[0.0 for k in range(out.y_len)] for j in range(out.x_len)] for i in range(out.z_len)]
						
				x_scale= [0.0]*out.x_len
				y_scale= [0.0]*out.y_len
				z_scale= [0.0]*out.z_len

			if s[0]=="#end":
				break
			
			if data_started==True:
				line_found=False
				if len(s)==4:
					line_found=True
					out.data[z][x][y]=float(s[3])
				if len(s)==3:
					line_found=True
					out.data[z][x][y]=float(s[2])
				elif len(s)==2:
					line_found=True
					out.data[z][x][y]=float(s[1])
				else:
					print("skip")

				if line_found==True:
					y=y+1
					if y==out.y_len:
						y=0
						x=x+1
					if x==out.x_len:
						x=0
						z=z+1

			if s[0]=="#data":
				data_started=True
	return True
			
