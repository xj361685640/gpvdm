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


from OpenGL.GL import *
from OpenGL.GLU import *
from PyQt5 import QtGui
from PyQt5 import QtOpenGL
from PyQt5.QtOpenGL import QGLWidget
from PyQt5.QtWidgets import QWidget, QHBoxLayout

#from layer_widget import layer_widget
from util import read_xyz_data

#from util import str2bool
#from tab_base import tab_base

import os

#inp
from inp import inp_load_file
from inp_util import inp_search_token_value

#path
from cal_path import get_materials_path

#contacts
from contacts_io import contacts_get_contacts
from contacts_io import contacts_get_array

#mesh
from mesh import mesh_get_xpoints
from mesh import mesh_get_ypoints
from mesh import mesh_get_zpoints
from mesh import mesh_get_xlen

#epitaxy
from epitaxy import epitaxy_get_layers
from epitaxy import epitaxy_get_width
from epitaxy import epitaxy_get_mat_file
from epitaxy import epitaxy_get_electrical_layer
from epitaxy import epitaxy_get_pl_file
from epitaxy import epitaxy_get_name

#qt
from PyQt5.QtGui import QFont

# Rotations for cube.
cube_rotate_x_rate = 0.2
cube_rotate_y_rate = 0.2
cube_rotate_z_rate = 0.2

# Rotation rates for the tetrahedron.
tet_x_rate = 0.0
tet_y_rate = 1.0
tet_z_rate = 0.5
tet_rotate_step = 10.0

def tab(x,y,z,w,h,d):


	glBegin(GL_QUADS)
	glColor3f(0.0,0.0,1.0)
	glVertex3f(x+w+0.05,y,z)
	glVertex3f(x+w+0.2,y ,z)
	glVertex3f(x+w+0.2,y+h ,z)
	glVertex3f(x+w+0.05,y+h ,z)

	glEnd()

def draw_mode(z_size):

	glLineWidth(5)
	glColor3f(0.0, 0.0, 0.0)
	glBegin(GL_LINES)
	t=[]
	s=[]
	z=[]

	if read_xyz_data(t,s,z,os.path.join(os.getcwd(),"light_dump","light_1d_photons_tot_norm.dat"))==True:
		array_len=len(t)
		for i in range(1,array_len):
			glVertex3f(-1.2-s[i-1]*0.5, -1.0+(z_size*(i-1)/array_len), 1.0)
			glVertex3f(-1.2-s[i]*0.5, -1.0+(z_size*i/array_len), 1.0)

	glEnd()


	
def box(x,y,z,w,h,d,r,g,b):


	
	red=r
	green=g
	blue=b

	glBegin(GL_QUADS)

	#btm
	glColor3f(red,green,blue)

	glVertex3f(x+0.0,y+0.0,z+0.0)
	glVertex3f(x+w,y+ 0.0,z+0.0)
	glVertex3f(x+w,y+ 0.0,z+d)
	glVertex3f(x+ 0.0, y+0.0,z+ d) 

	#back
	red=red*0.95
	green=green*0.95
	blue=blue*0.95

	glColor3f(red,green,blue)

	glVertex3f(x+0.0,y+h,z+0.0)
	glVertex3f(x+w,y+ h,z+0.0)
	glVertex3f(x+w,y+ h,z+d)
	glVertex3f(x+ 0.0, y+h,z+ d) 

	#right
	red=red*0.95
	green=green*0.95
	blue=blue*0.95
	glColor3f(red,green,blue)

	glVertex3f(x+w,y,z)
	glVertex3f(x+w,y+ h,z)
	glVertex3f(x+w,y+ h,z+d)
	glVertex3f(x+w, y,z+d) 

	#left
	red=red*0.95
	green=green*0.95
	blue=blue*0.95
	glColor3f(red,green,blue)

	glVertex3f(x,y,z)
	glVertex3f(x,y+ h,z)
	glVertex3f(x,y+ h,z+d)
	glVertex3f(x, y,z+d) 

	#front
	red=r
	green=g
	blue=b

	glColor3f(red,green,blue)
	glVertex3f(x,y,z+d)
	glVertex3f(x+w,y,z+d)
	glVertex3f(x+w,y+h,z+d)
	glVertex3f(x, y+h,z+d) 

	red=red*0.8
	green=green*0.8
	blue=blue*0.8

	#top
	glColor3f(red,green,blue)
	glVertex3f(x,y+h,z)
	glVertex3f(x+w,y+ h,z)
	glVertex3f(x+w,y+ h,z+ d)
	glVertex3f(x, y+h,z+ d) 

	glEnd()


class color():
	def __init__(self,r,g,b):
		self.r=r
		self.g=g
		self.b=b

class glWidget(QGLWidget):

	x_rot = 0.0
	y_rot = 0.0
	z_rot = 0.0
	tet_rotate = 0.0
	colors=[]
	def __init__(self, parent):
		QGLWidget.__init__(self, parent)
		self.setMinimumSize(600, 480)

	def paintGL(self):
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
		glLoadIdentity()

		glTranslatef(-0.5, 0.0, -6.0) # Move Into The Screen
		glRotatef(25.0, 1.0, 0.0, 0.0)
		glRotatef(-20.0, 0.0, 1.0, 0.0)

		glColor3f( 1.0, 1.5, 0.0 )
		glPolygonMode(GL_FRONT, GL_FILL);


		glClearColor(0.92, 0.92, 0.92, 0.5) # Clear to black.

		lines=[]

		draw_mode(2.0)

		tot=0
		for i in range(0,epitaxy_get_layers()):
			tot=tot+epitaxy_get_width(i)

		mul=1.5/tot
		pos=0.0
		width=2.0
		depth=2.0

		l=epitaxy_get_layers()-1
		#lines=[]
			#pos=0.0
		xpoints=int(mesh_get_xpoints())
		ypoints=int(mesh_get_ypoints())
		zpoints=int(mesh_get_zpoints())
		for i in range(0,epitaxy_get_layers()):

			thick=epitaxy_get_width(l-i)*mul
			print(thick)
			red=self.colors[l-i].r
			green=self.colors[l-i].g
			blue=self.colors[l-i].b

			if epitaxy_get_electrical_layer(l-i).startswith("dos")==True:
				dy=thick/float(ypoints)
				dx=width/float(xpoints)
				dz=depth/float(zpoints)
				xshrink=0.8
				zshrink=0.8
				if xpoints==1:
					xshrink=1.0

				if zpoints==1:
					zshrink=1.0


				for y in range(0,ypoints):
					for x in range(0,xpoints):
						for z in range(0,zpoints):
							box(dx*x,pos+y*(dy),z*dz,dx*xshrink,dy*0.8,dz*zshrink,red,green,blue)
				tab(0.0,pos,depth,width,thick,depth)
				
			elif epitaxy_get_electrical_layer(l-i)=="Contact" and i==l:
				if xpoints==1 and zpoints==1:
					box(0.0,pos,0,width,thick,depth,red,green,blue)
				else:
					for c in contacts_get_array():
						x_len=mesh_get_xlen()
						xstart=width*(c.start/x_len)
						xwidth=width*(c.width/x_len)
						if (c.start+c.width)>x_len:
							xwidth=width-xstart

						box(xstart,pos,0,xwidth,thick,depth,red,green,blue)
			else:
				box(0.0,pos,0,width,thick,depth,red,green,blue)
				

			if epitaxy_get_electrical_layer(l-i).startswith("dos")==True:
				text=epitaxy_get_name(l-i)+" (active)"
			else:
				text=epitaxy_get_name(l-i)

			glColor3f(0.0,0.0,0.0)
			font = QFont("Arial")
			font.setPointSize(18)
			self.renderText (width+0.1,pos,depth, text,font)

			pos=pos+thick+0.05


			glRotatef(self.tet_rotate, tet_x_rate, tet_y_rate, tet_z_rate)


	def recalculate(self):
		self.colors=[]
		l=epitaxy_get_layers()-1
		for i in range(0,epitaxy_get_layers()):
			print("order",epitaxy_get_mat_file(l-i))
			path=os.path.join(get_materials_path(),epitaxy_get_mat_file(l-i),"mat.inp")
			lines=[]

			if inp_load_file(lines,path)==True:
				red=float(inp_search_token_value(lines, "#Red"))
				green=float(inp_search_token_value(lines, "#Green"))
				blue=float(inp_search_token_value(lines, "#Blue"))
			else:
				print("Could not load",path)
				red=0.0
				green=0.0
				blue=0.0
			self.colors.append(color(red,green,blue))

		self.update()

	def initializeGL(self):

		self.recalculate()

		glClearDepth(1.0)              
		glDepthFunc(GL_LESS)
		glEnable(GL_DEPTH_TEST)
		glShadeModel(GL_SMOOTH)
		
		glViewport(0, 0, self.width(), self.height())
		glMatrixMode(GL_PROJECTION)
		glLoadIdentity()                    
		gluPerspective(45.0,float(self.width()) / float(self.height()),0.1, 100.0) 
		glMatrixMode(GL_MODELVIEW)




