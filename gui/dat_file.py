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
	my_max=False
	my_min=False
	
	if my_data.x_len>0 and my_data.y_len>0 and my_data.z_len>0:
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

#search first 40 lines for dims
def find_dim(lines):
	x_len=-1
	y_len=-1
	z_len=-1

	my_max=len(lines)
	if my_max>40:
		my_max=40
		
	for i in range(0, my_max):
		temp=lines[i]
		temp=re.sub(' +',' ',temp)
		temp=re.sub('\t',' ',temp)
		s=lines[i].split(" ")

		if s[0]=="#x":
			x_len=int(s[1])

		if s[0]=="#y":
			y_len=int(s[1])

		if s[0]=="#z":
			z_len=int(s[1])

		if x_len!=-1 and y_len!=-1 and z_len!=-1:
			return x_len,y_len,z_len
	return False,False,False

def guess_dim(lines):
	x=0
	y=0
	z=0
	data_started=False
	for i in range(0, len(lines)):
		temp=lines[i]
		temp=re.sub(' +',' ',temp)
		temp=re.sub("\t"," ",temp)
		temp=re.sub("\r","",temp)

		if len(temp)>0:
			if is_number(temp)==True:
				s=temp.split(" ")
				if len(s)==1:
					print("I can't do this file type yet")
					return False,False,False
				if len(s)==2:
					y=y+1
				if len(s)==3:
					print("I can't do this file type yet")
					return False,False,False
	return 1,y,1

def is_number(data_in):
	if type(data_in)==str:
		if len(data_in)>0:
			s=data_in
			s=re.sub(' ','',s)
			s=re.sub("\+",'',s)
			s=re.sub('-','',s)
			s=re.sub('\t','',s)
	
			if len(s)>0:
				if s[0].isdigit()==True:
					return True
				else:
					return False

	return False

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
	

	out.x_len, out.y_len, out.z_len = find_dim(lines)

	if out.x_len==False:
		out.x_len, out.y_len, out.z_len = guess_dim(lines)
		if out.x_len==False:
			#print(file_name)
			return False

	out.data=[[[0.0 for k in range(out.y_len)] for j in range(out.x_len)] for i in range(out.z_len)]
			
	out.x_scale= [0.0]*out.x_len
	out.y_scale= [0.0]*out.y_len
	out.z_scale= [0.0]*out.z_len

	data_started=False

	x=0
	y=0
	z=0
	dim=0
	for i in range(0, len(lines)):
		temp=lines[i]
		temp=re.sub(' +',' ',temp)
		temp=re.sub('\t',' ',temp)
		s=temp.split(" ")

		if len(s)>0:

			#if 
			if data_started==False:
				if is_number(s[0])==True:
					data_started=True

			if s[0]=="#end":
				break

			if data_started==True:
				line_found=False
				if len(s)==4:
					line_found=True
					out.data[z][x][y]=float(s[3])
					a0=s[0]
					a1=s[1]
					a2=s[2]

				if len(s)==3:
					line_found=True
					out.data[z][x][y]=float(s[2])
					a0=s[0]
					a1=s[1]
					a2=0.0
				elif len(s)==2:
					line_found=True
					out.data[z][x][y]=float(s[1])
					a0=s[0]
					a1=0.0
					a2=0.0
#				else:
#					print("skip")

				if line_found==True:
					if len(s)==2:
						if x==0 and z==0:
							out.y_scale[y]=float(a0)

					if len(s)==3:
						if x==0 and z==0:
							out.y_scale[y]=float(a1)
							
						if z==0 and y==0:
							out.x_scale[x]=float(a0)

					#if z==y:
					#	out.z_scale[y]=float(a0)

					y=y+1
					if y==out.y_len:
						y=0
						x=x+1
					if x==out.x_len:
						x=0
						z=z+1

			if s[0]=="#data":
				data_started=True

	if data_started==False:
		return False
	return True
			

def read_data_2d(x_scale,y_scale,z,file_name):
	if file_name==None:
		return False
	
	found,lines=zip_get_data_file(file_name)
	if found==True:
		x_max=0
		y_max=0
		y_pos=0
		z_store=[]
		for i in range(0, len(lines)):
			if len(lines[i])>0:
				if lines[i][0]!="#" and lines[i]!="\n":
					temp=lines[i]
					temp=re.sub(' +',' ',temp)
					temp=re.sub('\t',' ',temp)
					temp=temp.rstrip()
					sline=temp.split(" ")

					if len(sline)==3:
						if x_max==0:
							y_scale.append(float(lines[i].split(" ")[1]))
						if y_pos==0:
							x_scale.append(float(lines[i].split(" ")[0]))

						z_store.append(float(lines[i].split(" ")[2]))
					y_pos=y_pos+1

					if x_max==0:
						y_max=y_max+1

			if lines[i]=="":
				x_max=x_max+1
				y_pos=0

		if  lines[len(lines)-1]!="\n":
			x_max=x_max+1

		x_max=len(x_scale)
		y_max=len(y_scale)

		pos=0
		for x in range(0, x_max):
			z.append([])
			for y in range(0, y_max):
				z[x].append(z_store[pos])
				pos=pos+1
		return True
	else:
		return False
