#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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


#import sys
import os
#import shutil
from inp import inp_write_lines_to_file
from inp import inp_load_file
from code_ctrl import enable_betafeatures
from scan_item import scan_item_add

xlist=[]
ylist=[]
zlist=[]

class mlayer():
	def __init__(self):
		self.thick=0.0
		self.points=0

def mesh_add(vector,thick,points):
	a=mlayer()
	a.thick=float(thick)
	a.points=float(points)
	if vector=="x":
		global xlist
		xlist.append(a)
	elif vector=="y":
		global ylist
		ylist.append(a)
	elif vector=="z":
		global zlist
		zlist.append(a)
		
	
def mesh_load(vector):
	file_name="mesh_"+vector+".inp"

	if vector=="x":
		mesh_clear_xlist()
	elif vector=="y":
		mesh_clear_ylist()
	elif vector=="z":
		mesh_clear_zlist()

	global xlist
	my_list=[]
	pos=0
	lines=[]

	if inp_load_file(lines,os.path.join(os.getcwd(),file_name))==True:
		pos=pos+1	#first comment
		mesh_layers=int(lines[pos])
		for i in range(0, mesh_layers):
			#thick
			pos=pos+1					#token
			token=lines[pos]
			pos=pos+1
			thick=lines[pos]	#read value


			#points
			pos=pos+1					#token
			token=lines[pos]
			pos=pos+1
			points=lines[pos] 		#read value
			mesh_add(vector,thick,points)


def mesh_save(file_name,my_list):
	lines=[]
	lines.append("#mesh_layers")
	lines.append(str(len(my_list)))
	i=0
	for item in my_list:
		lines.append("#mesh_layer_length"+str(i))
		lines.append(str(item.thick))
		lines.append("#mesh_layer_points"+str(i))
		lines.append(str(item.points))
		i=i+1
	lines.append("#ver")
	lines.append("1.0")
	lines.append("#end")

	inp_write_lines_to_file(os.path.join(os.getcwd(),file_name),lines)

def mesh_save_all():
	global xlist
	global ylist
	global zlist
	mesh_save("mesh_x.inp",xlist)
	mesh_save("mesh_y.inp",ylist)
	mesh_save("mesh_z.inp",zlist)
	

def mesh_load_all():
	mesh_load_x()
	mesh_load_y()
	mesh_load_z()	

	
def mesh_load_x():
	mesh_load("x")

def mesh_load_y():
	mesh_load("y")

def mesh_load_z():
	mesh_load("z")

def mesh_get_xlen():
	global xlist
	tot=0.0
	for a in xlist:
		tot=tot+a.thick
	return tot

def mesh_get_ylen():
	global ylist
	tot=0.0
	for a in ylist:
		tot=tot+a.thick
	return tot

def mesh_get_zlen():
	global zlist
	tot=0.0
	for a in zlist:
		tot=tot+a.thick
	return tot

def mesh_get_xpoints():
	global xlist
	tot=0.0
	for a in xlist:
		tot=tot+a.points
	return tot

def mesh_get_ypoints():
	global ylist
	tot=0.0
	for a in ylist:
		tot=tot+a.points
	return tot

def mesh_get_zpoints():
	global zlist
	tot=0.0
	for a in zlist:
		tot=tot+a.points
	return tot

def mesh_get_xlayers():
	global xlist
	return len(xlist)

def mesh_get_ylayers():
	global ylist
	return len(ylist)

def mesh_get_zlayers():
	global zlist
	return len(zlist)

def mesh_get_xlist():
	global xlist
	return xlist

def mesh_get_ylist():
	global ylist
	return ylist

def mesh_get_zlist():
	global zlist
	return zlist

def mesh_clear_xlist():
	global xlist
	xlist=[]

def mesh_clear_ylist():
	global ylist
	ylist=[]

def mesh_clear_zlist():
	global zlist
	zlist=[]
	
def mesh_clear():
	mesh_clear_xlist()
	mesh_clear_ylist()
	mesh_clear_zlist()
