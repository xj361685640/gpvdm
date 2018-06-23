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
from inp import inp_save
from inp import inp_load_file
from util import isnumber
from scan_item import scan_item_add
from scan_item import scan_remove_file
from cal_path import get_materials_path

from inp import inp_load_file
from inp import inp_search_token_array
from inp_util import inp_search_token_value


class epi_layer():
	def __init__(self):
		self.width=0
		self.mat_file=""
		self.name=""
		self.pl_file=""
		self.r=0
		self.g=0
		self.b=0
		self.alpha=0

epi=[]

def epitaxy_update_scan():
	global epi
	scan_remove_file("epitaxy.inp")
	for i in range(0,len(epi)):
		scan_item_add("epitaxy.inp","#layer_material_file"+str(i),_("Material for ")+str(epi[i].name),2)
		scan_item_add("epitaxy.inp","#layer_width"+str(i),_("Layer width ")+str(epi[i].name),1)

def epitaxy_populate_rgb():
	global epi
	path=os.path.join(get_materials_path(),epi[-1].mat_file,"mat.inp")
	mat_lines=inp_load_file(path)
	ret=inp_search_token_array(mat_lines, "#red_green_blue")

	if ret!=False:
		epi[-1].r=float(ret[0])
		epi[-1].g=float(ret[1])
		epi[-1].b=float(ret[2])
		epi[-1].alpha=float(inp_search_token_value(mat_lines, "#mat_alpha"))

def epitaxy_load(path):
	lines=[]
	global epi

	epi=[]
	
	lines=inp_load_file(os.path.join(path,"epitaxy.inp"))
	if lines!=False:
		pos=0
		pos=pos+1

		for i in range(0, int(lines[pos])):
			a=epi_layer()
			pos=pos+1		#token
			pos=pos+1
			a.name=lines[pos]

			pos=pos+1		#token
			pos=pos+1
			a.width=float(lines[pos])

			pos=pos+1		#token
			pos=pos+1
			lines[pos]=lines[pos].replace("\\", "/")
			a.mat_file=lines[pos]

			pos=pos+1		#token
			pos=pos+1
			a.electrical_layer=lines[pos]		#value

			pos=pos+1		#token
			pos=pos+1
			a.pl_file=lines[pos]		#value


			epi.append(a)
			epitaxy_populate_rgb()

	epitaxy_update_scan()

def epitay_get_next_dos():
	global epi
	for i in range(0,20):
		name="dos"+str(i)
		found=False
		for a in epi:
			if a.electrical_layer==name:
				found=True

		if found==False:
			return name

def epitay_get_next_pl():
	global epi
	for i in range(0,20):
		name="pl"+str(i)
		found=False
		for a in epi:
			if a.pl_file==name:
				found=True

		if found==False:
			return name

def epitaxy_get_layer(i):
	global epi
	return epi[i]

def epitaxy_get_epi():
	global epi
	return epi

def epitaxy_load_from_arrays(in_name,in_width,in_material,in_dos_layer,in_pl_file):

	for i in range(0, len(in_width)):
		if isnumber(in_width[i])==False:
			return False

	global epi

	epi=[]

	for i in range(0, len(in_width)):

		a=epi_layer()
		
		a.name=in_name[i]

		a.width=float(in_width[i])
		a.mat_file=in_material[i]

		a.electrical_layer=in_dos_layer[i]		#value

		a.pl_file=in_pl_file[i]							#value

		epi.append(a)
		epitaxy_populate_rgb()

	epitaxy_update_scan()

	return True

def epitaxy_print():
	global epi
	print("Epitxy dump:")

	print("layers=",str(len(epi)))
	for i in range(0,len(epi)):
		print("#layer"+str(layer))
		print(str(epi[i].name))
		print(str(epi[i].width))
		print(epi[i].mat_file)
		print(epi[i].electrical_layer)
		print(epi[i].pl_file)
		
def epitaxy_save(path):
	global epi

	#dos_text=""
	lines=[]
	lines.append("#layers")
	lines.append(str(len(epi)))

	layer=0
	for i in range(0,len(epi)):
		lines.append("#layer_name"+str(layer))
		lines.append(str(epi[i].name))
		lines.append("#layer_width"+str(layer))
		lines.append(str(epi[i].width))
		lines.append("#layer_material_file"+str(layer))
		lines.append(epi[i].mat_file)
		lines.append("#layer_dos_file"+str(layer))
		lines.append(epi[i].electrical_layer)
		lines.append("#layer_pl_file"+str(layer))
		lines.append(epi[i].pl_file)
		layer=layer+1

	lines.append("#ver")
	lines.append("1.3")
	lines.append("#end")

	inp_save(os.path.join(path,"epitaxy.inp"),lines)

def epitaxy_get_dos_files():
	global epi
	dos_file=[]
	for i in range(0,len(epi)):
		if epi[i].electrical_layer.startswith("dos")==True:
			dos_file.append(epi[i].electrical_layer)

	return dos_file

def epitaxy_get_device_start():
	global epi

	pos=0.0
	for i in range(0, len(epi)):
		if epi[i].electrical_layer.startswith("dos")==True:
			return pos

		pos=pos+width[i]
			
def epitaxy_get_layers():
	global epi
	return len(epi)

def epitaxy_get_width(i):
	global epi
	return epi[i].width

def epitaxy_get_electrical_layer(i):
	global epi
	return epi[i].electrical_layer

def epitaxy_get_mat_file(i):
	global epi
	return epi[i].mat_file

def epitaxy_get_pl_file(i):
	global epi
	return epi[i].pl_file

def epitaxy_get_dos_file(i):
	global epi
	return epi[i].electrical_layer

def epitaxy_get_name(i):
	global epi
	return epi[i].name

def epitaxy_get_y_len():
	tot=0

	for i in range(0,epitaxy_get_layers()):
		tot=tot+epitaxy_get_width(i)

	return tot
