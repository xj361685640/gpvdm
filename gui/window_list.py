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


from inp import inp_load_file
from inp import inp_write_lines_to_file

from PyQt5.QtWidgets import QWidget, QDesktopWidget

def get_main_window_size():
	shape=QDesktopWidget().screenGeometry()
	desktop_w=shape.width()*0.7
	desktop_h=shape.height()*0.7
	return desktop_w,desktop_h

class window_item:
	name=""
	x=0.0
	y=0.0

wlist=[]

class windows():
	def update(self,window,name):
		global wlist
		pos=-1
		x=window.x()
		y=window.y()

		for i in range(0,len(wlist)):
			if wlist[i].name==name:
				pos=i
				wlist[i].x=x
				wlist[i].y=y
				break

		if pos==-1:
			a=window_item()
			a.name=name
			a.x=x
			a.y=y
			wlist.append(a)
			pos=len(wlist)-1


		lines=[]
		lines.append(str(len(wlist)))

		for i in range(0,len(wlist)):
			lines.append(wlist[i].name)
			lines.append(str(wlist[i].x))
			lines.append(str(wlist[i].y))

		return inp_write_lines_to_file("window_list.inp",lines)

	def set_window(self,window,name):
		global wlist
		for i in range(0,len(wlist)):
			if wlist[i].name==name:
				shape=QDesktopWidget().screenGeometry()

				desktop_w=shape.width()
				desktop_h=shape.height()

				w=window.width()
				h=window.height()

				x=int(wlist[i].x)
				y=int(wlist[i].y)
				if (x+w>desktop_w):
					x=0
					#print("Reset with")
				if (y+h>desktop_h):
					y=0
					#print("Reset height")

				window.move(x,y)
				break

	def load(self):
		global wlist
		wlist=[]
		lines=[]
		if inp_load_file(lines,"window_list.inp")==True:
			number=int(lines[0])
			for i in range(0,number):
				a=window_item()
				a.name=lines[i*3+1]
				a.x=lines[i*3+2]
				a.y=lines[i*3+3]
				wlist.append(a)


