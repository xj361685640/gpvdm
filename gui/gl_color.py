#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2016 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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

import sys
from OpenGL.GL import *

class color():
	def __init__(self,r,g,b,alpha):
		self.r=r
		self.g=g
		self.b=b
		self.alpha=alpha
		self.name=""

color_list=[]
false_color=False

def search_color(r,g,b):
	global color_list
	n=0
	my_min=1000.0
	for i in range(0,len(color_list)):
		val=abs(color_list[i].r-r)+abs(color_list[i].g-g)+abs(color_list[i].b-b)
		if val<my_min:
			my_min=val
			n=i
	return color_list[n].name

def set_false_color(value):
	global false_color
	false_color=value

def set_color(r,g,b,name,alpha=-1):
	global color_list
	global false_color
	if false_color==True:
		rgb_int=len(color_list)
		r =  rgb_int & 255
		g = (rgb_int >> 8) & 255
		b =   (rgb_int >> 16) & 255
		r=r/255
		g=g/255
		b=b/255

	a=color(r,g,b,alpha)
	a.name=name
	color_list.append(a)
	glColor3f(r,g,b)

def clear_color():
	global color_list
	color_list=[]

def set_color_alpha(r,g,b,alpha,name):
	global color_list
	a=color(r,g,b,alpha)
	a.name=name
	color_list.append(a)
	glColor4f(r,g,b, alpha)

def print_color():
	global color_list
	for i in range(0,len(color_list)):
		print(color_list[i].r,color_list[i].g,color_list[i].b,color_list[i].alpha,color_list[i].name)

