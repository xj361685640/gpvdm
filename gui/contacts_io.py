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
from inp import inp_load_file
from inp_util import inp_search_token_value
from inp import inp_write_lines_to_file
from cal_path import get_image_file_path
from inp import inp_update

from util import str2bool

class segment():
	def __init__(self):
		self.start=0.0
		self.depth=0.0
		self.voltage=0.0
		self.width=0.0
		self.active=False

store=[]

def contacts_print():
	global store
	for s in store:
		print(s.start, s.width,s.depth,s.voltage,s.active)

def contacts_get_contacts():
	global store
	return len(store)

def contacts_get_array():
	global store
	return store

def contacts_clear():
	global store
	store=[]

def contacts_append(start,depth,voltage,width,active):
	global store
	s=segment()
	s.start=start
	s.depth=depth
	s.voltage=voltage
	s.width=width
	s.active=active
	store.append(s)

def contacts_save():
	global store
	lines=[]
	lines.append("#contacts")
	lines.append(str(len(store)))
	i=0
	for s in store:
		lines.append("#contact_start"+str(i))
		lines.append(str(s.start))
		lines.append("#contact_width"+str(i))
		lines.append(str(s.width))
		lines.append("#contact_depth"+str(i))
		lines.append(str(s.depth))
		lines.append("#contact_voltage"+str(i))
		lines.append(str(s.voltage))
		lines.append("#contact_active"+str(i))
		lines.append(str(s.active))
		i=i+1
	lines.append("#ver")
	lines.append("1.0")
	lines.append("#end")
	
	inp_write_lines_to_file(os.path.join(os.getcwd(),"contacts.inp"),lines)


def contacts_load():
	global store
	store=[]
	lines=[]
	pos=0
	if inp_load_file(lines,os.path.join(os.getcwd(),"contacts.inp"))==True:
		pos=pos+1	#first comment
		layers=int(lines[pos])

		for i in range(0, layers):
			#start
			pos=pos+1					#token
			token=lines[pos]

			pos=pos+1
			start=lines[pos]			#read value

			#width
			pos=pos+1					#token
			token=lines[pos]

			pos=pos+1
			width=lines[pos]			#read value

			#depth
			pos=pos+1					#token
			token=lines[pos]

			pos=pos+1
			depth=lines[pos]			#read value

			#voltage
			pos=pos+1					#token
			token=lines[pos]

			pos=pos+1
			voltage=lines[pos]			#read value

			#active
			pos=pos+1					#token
			token=lines[pos]
			
			pos=pos+1
			active=lines[pos]			#read value
			
			contacts_append(float(start),float(depth),float(voltage),float(width),str2bool(active))

		print("store=",store)

