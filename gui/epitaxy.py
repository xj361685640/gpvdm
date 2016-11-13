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
from inp import inp_write_lines_to_file
from inp import inp_load_file
from util import isnumber

layers=0
electrical_layers=0
width=[]
mat_file=[]
electrical_layer=[]
pl_file=[]
name=[]

def epitaxy_load():
	lines=[]
	global layers
	global electrical_layers
	global width
	global mat_file
	global electrical_layer
	global pl_file
	global name

	layers=0
	electrical_layers=0
	width=[]
	mat_file=[]
	electrical_layer=[]
	pl_file=[]
	name=[]

	if inp_load_file(lines,"epitaxy.inp")==True:
		pos=0
		pos=pos+1

		for i in range(0, int(lines[pos])):
			pos=pos+1		#token
			pos=pos+1
			name.append(lines[pos])

			pos=pos+1		#token
			pos=pos+1
			width.append(float(lines[pos]))

			pos=pos+1		#token
			pos=pos+1
			mat_file.append(lines[pos])

			pos=pos+1		#token
			pos=pos+1
			electrical_layer.append(lines[pos])		#value

			if lines[pos].startswith("dos")==True:
				electrical_layers=electrical_layers+1

			pos=pos+1		#token
			pos=pos+1
			pl_file.append(lines[pos])		#value

			layers=layers+1

def epitay_get_next_dos():
	global electrical_layer
	for i in range(0,20):
		name="dos"+str(i)
		if electrical_layer.count(name)==0:
			return name

def epitay_get_next_pl():
	global pl_file
	for i in range(0,20):
		name="pl"+str(i)
		if pl_file.count(name)==0:
			return name

def epitaxy_load_from_arrays(in_name,in_width,in_material,in_dos_layer,in_pl_file):

	for i in range(0, len(in_width)):
		if isnumber(in_width[i])==False:
			return False

	global layers
	global electrical_layers
	global width
	global mat_file
	global electrical_layer
	global pl_file
	global name

	layers=0
	electrical_layers=0
	width=[]
	mat_file=[]
	electrical_layer=[]
	pl_file=[]
	name=[]

	for i in range(0, len(in_width)):

		name.append(in_name[i])

		width.append(float(in_width[i]))

		mat_file.append(in_material[i])

		electrical_layer.append(in_dos_layer[i])		#value

		pl_file.append(in_pl_file[i])			#value

		if in_dos_layer[i].startswith("dos")==True:
			electrical_layers=electrical_layers+1


		layers=layers+1

	return True

def epitaxy_print():
	global layers
	global electrical_layers
	global width
	global mat_file
	global electrical_layer
	global pl_file
	global name
	print("Epitxy dump:")
	layer=0
	print("layers=",str(layers))
	for i in range(0,layers):
		print("#layer"+str(layer))
		print(str(name[i]))
		print(str(width[i]))
		print(mat_file[i])
		print(electrical_layer[i])
		print(pl_file[i])
		layer=layer+1
		
def epitaxy_save():
	global layers
	global electrical_layers
	global width
	global mat_file
	global electrical_layer
	global pl_file
	global name

	#dos_text=""
	lines=[]
	lines.append("#layers")
	lines.append(str(layers))

	layer=0
	for i in range(0,layers):
		lines.append("#layer_name"+str(layer))
		lines.append(str(name[i]))
		lines.append("#layer_width"+str(layer))
		lines.append(str(width[i]))
		lines.append("#layer_material_file"+str(layer))
		lines.append(mat_file[i])
		lines.append("#layer_dos_file"+str(layer))
		lines.append(electrical_layer[i])
		lines.append("#layer_pl_file"+str(layer))
		lines.append(pl_file[i])
		layer=layer+1

	lines.append("#ver")
	lines.append("1.3")
	lines.append("#end")

	inp_write_lines_to_file(os.path.join(os.getcwd(),"epitaxy.inp"),lines)

def epitaxy_get_dos_files():
	global electrical_layer
	dos_file=[]
	for i in range(0,len(electrical_layer)):
		if electrical_layer[i].startswith("dos")==True:
			dos_file.append(electrical_layer[i])

	return dos_file

def epitaxy_get_device_start():
	global layers
	global width
	global electrical_layer

	pos=0.0
	for i in range(0, layers):
		if electrical_layer[i].startswith("dos")==True:
			return pos

		pos=pos+width[i]
			
def epitaxy_get_layers():
	global layers
	return layers

def epitaxy_get_width(i):
	global width
	return width[i]

def epitaxy_get_electrical_layer(i):
	global electrical_layer
	#print electrical_layer,i
	return electrical_layer[i]

def epitaxy_get_mat_file(i):
	global mat_file
	return mat_file[i]

def epitaxy_get_pl_file(i):
	global pl_file
	return pl_file[i]

def epitaxy_get_dos_file(i):
	global electrical_layer
	return electrical_layer[i]

def epitaxy_get_name(i):
	global name
	return name[i]

