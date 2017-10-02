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
from inp import inp_save
from inp import inp_load_file
from code_ctrl import enable_betafeatures
from scan_item import scan_item_add

from cal_path import get_sim_path
from util import str2bool

class mesh:
	layers=[]
	remesh=False

xlist=mesh()
ylist=mesh()
zlist=mesh()

class mlayer():
	def __init__(self):
		self.thick=0.0
		self.points=0
		self.mul=1.0
		self.left_right="left"
		

def mesh_add(vector,thick,points,mul,left_right):
	a=mlayer()
	a.thick=float(thick)
	a.points=float(points)
	a.mul=float(mul)
	a.left_right=left_right
	if vector=="x":
		global xlist
		xlist.layers.append(a)
	elif vector=="y":
		global ylist
		ylist.layers.append(a)
	elif vector=="z":
		global zlist
		zlist.layers.append(a)
		
	
def mesh_load(vector):
	file_name="mesh_"+vector+".inp"

	if vector=="x":
		mesh_clear_xlist()
	elif vector=="y":
		mesh_clear_ylist()
	elif vector=="z":
		mesh_clear_zlist()

	my_list=[]
	pos=0
	lines=inp_load_file(file_name)

	if lines!=False:
		pos=pos+1	#first comment
		remesh=str2bool(lines[pos])
		pos=pos+1	#remesh

		if vector=="x":
			global xlist
			xlist.remesh=remesh
		elif vector=="y":
			global ylist
			ylist.remesh=remesh
		elif vector=="z":
			global zlist
			zlist.remesh=remesh

		pos=pos+1	#first comment
		mesh_layers=int(lines[pos])
		for i in range(0, mesh_layers):
			#thick
			pos=pos+1			#token
			token=lines[pos]
			pos=pos+1
			thick=lines[pos]	#length

			pos=pos+1			#token
			token=lines[pos]
			pos=pos+1
			points=lines[pos] 	#points
			
			pos=pos+1			#token
			token=lines[pos]
			pos=pos+1
			mul=lines[pos] 		#mul

			pos=pos+1			#token
			token=lines[pos]
			pos=pos+1
			left_right=lines[pos] 		#left_right

			mesh_add(vector,thick,points,mul,left_right)


def mesh_save(file_name,mesh_class):
	lines=[]
	lines.append("#remesh_enable")
	lines.append(str(mesh_class.remesh))	
	lines.append("#mesh_layers")
	lines.append(str(len(mesh_class.layers)))
	i=0
	for item in mesh_class.layers:
		lines.append("#mesh_layer_length"+str(i))
		lines.append(str(item.thick))
		lines.append("#mesh_layer_points"+str(i))
		lines.append(str(item.points))
		lines.append("#mesh_layer_mul"+str(i))
		lines.append(str(item.mul))
		lines.append("#mesh_layer_left_right"+str(i))
		lines.append(str(item.left_right))
		i=i+1
	lines.append("#ver")
	lines.append("1.0")
	lines.append("#end")
	print("save as",file_name,lines)
	inp_save(file_name,lines)

def mesh_save_x():
	global xlist
	mesh_save("mesh_x.inp",xlist)

def mesh_save_y():
	global ylist
	mesh_save("mesh_y.inp",ylist)

def mesh_save_z():
	global zlist
	mesh_save("mesh_z.inp",zlist)

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
	for a in xlist.layers:
		tot=tot+a.thick
	return tot

def mesh_get_ylen():
	global ylist
	tot=0.0
	for a in ylist.layers:
		tot=tot+a.thick
	return tot

def mesh_get_zlen():
	global zlist
	tot=0.0
	for a in zlist.layers:
		tot=tot+a.thick
	return tot

def mesh_get_xpoints():
	global xlist
	tot=0.0
	for a in xlist.layers:
		tot=tot+a.points
	return tot

def mesh_get_ypoints():
	global ylist
	tot=0.0
	for a in ylist.layers:
		tot=tot+a.points
	return tot

def mesh_get_zpoints():
	global zlist
	tot=0.0
	for a in zlist.layers:
		tot=tot+a.points
	return tot

def mesh_get_xlayers():
	global xlist
	return len(xlist.layers)

def mesh_get_ylayers():
	global ylist
	return len(ylist.layers)

def mesh_get_zlayers():
	global zlist
	return len(zlist.layers)

def mesh_get_xmesh():
	global xlist
	return xlist

def mesh_get_ymesh():
	global ylist
	return ylist

def mesh_get_zmesh():
	global zlist
	return zlist

def mesh_clear_xlist():
	global xlist
	zlist.remesh=True
	xlist.layers=[]

def mesh_clear_ylist():
	global ylist
	zlist.remesh=True
	ylist.layers=[]

def mesh_clear_zlist():
	global zlist
	zlist.remesh=True
	zlist.layers=[]
	
def mesh_clear():
	mesh_clear_xlist()
	mesh_clear_ylist()
	mesh_clear_zlist()
