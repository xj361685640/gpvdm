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


#import sys
import os
#import shutil
import gobject
from inp import inp_write_lines_to_file
from inp import inp_load_file
from code_ctrl import enable_betafeatures
from scan_item import scan_item_add
from cal_path import get_image_file_path

xlist=[]
ylist=[]
zlist=[]

class mlayer():
	def __init__(self):
		self.thick=0.0
		self.points=0

def mesh_add(my_list,thick,points):
	a=mlayer()
	a.thick=float(thick)
	a.points=float(points)
	my_list.append(a)

def mesh_load(file_name):
	global xlist
	my_list=[]
	pos=0
	lines=[]
	print "loading",os.path.join(os.getcwd(),file_name)
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
			print "adding",thick,points
			mesh_add(my_list,thick,points)
			print "adding",thick,points,len(my_list)
	return my_list

def mesh_save(file_name,my_list):
	lines=[]
	lines.append("#mesh_layers")
	lines.append(str(len(self.mesh_model)))
	i=0
	for item in my_list:
		lines.append("#mesh_layer_length"+str(i))
		lines.append(item.thick)
		lines.append("#mesh_layer_points"+str(i))
		lines.append(item.points)
		i=i+1
	lines.append("#ver")
	lines.append("1.0")
	lines.append("#end")
	inp_write_lines_to_file(os.path.join(os.getcwd(),file_name),lines)

def mesh_load_all():
	mesh_load_x()
	mesh_load_y()
	mesh_load_z()	

def mesh_load_x():
	global xlist
	xlist=mesh_load("mesh_x.inp")

def mesh_load_y():
	global ylist
	ylist=mesh_load("mesh_y.inp")

def mesh_load_z():
	global zlist
	zlist=mesh_load("mesh_z.inp")

def mesh_get_xlen():
	global xlist
	tot=0.0
	for a in xlist:
		print a.thick
		tot=tot+a.thick
	return tot

def mesh_get_xpoints():
	global xlist
	tot=0.0
	for a in xlist:
		print a.points
		tot=tot+a.points
	return tot

def mesh_get_ypoints():
	global ylist
	tot=0.0
	for a in ylist:
		print a.points
		tot=tot+a.points
	return tot

def mesh_get_zpoints():
	global zlist
	tot=0.0
	for a in zlist:
		print a.points
		tot=tot+a.points
	return tot
