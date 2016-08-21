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

import sys
open_gl_ok=False

try:
	from OpenGL.GL import *
	from OpenGL.GLU import *
	from PyQt5 import QtOpenGL
	from PyQt5.QtOpenGL import QGLWidget
	open_gl_ok=True
except:
	print("opengl error ",sys.exc_info()[0])

from PyQt5 import QtGui
from PyQt5.QtCore import Qt
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
import numpy as np
from inp import inp_get_token_value
from util import str2bool

from PyQt5.QtCore import QTimer

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


def draw_photon(x,z,up):
	glLineWidth(3)
	length=0.9
	if up==True:
		glColor3f(0.0, 0.0, 1.0)
	else:
		glColor3f(0.0, 1.0, 0.0)

	glBegin(GL_LINES)
	wx=np.arange(0, length , 0.01)
	wy=np.sin(wx*3.14159*8)*0.2
	
	start_x=2.7
	stop_x=2.7-length
	for i in range(1,len(wx)):
		glVertex3f(x, start_x-wx[i-1], z+wy[i-1])
		glVertex3f(x, start_x-wx[i], z+wy[i])

	glEnd()

	if up==False:
		glBegin(GL_TRIANGLES)

		glVertex3f(x-0.1, stop_x,z)
		glVertex3f(x+0.1, stop_x ,z)
		glVertex3f(x,stop_x-0.1 ,z)

		glEnd()
	else:
		glBegin(GL_TRIANGLES)

		glVertex3f(x-0.1, start_x,z)
		glVertex3f(x+0.1, start_x ,z)
		glVertex3f(x,start_x+0.1 ,z)

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
		t.reverse()
		s.reverse()		
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

if open_gl_ok==True:		
	class glWidget(QGLWidget):

		tet_rotate = 0.0
		colors=[]
		def __init__(self, parent):
			self.xRot =25.0
			self.yRot =-20.0
			self.zRot =0.0
			self.enabled=False
			self.timer=None
			QGLWidget.__init__(self, parent)
			self.lastPos=None
			#glClearDepth(1.0)              
			#glDepthFunc(GL_LESS)
			#glEnable(GL_DEPTH_TEST)
			#glShadeModel(GL_SMOOTH)
		
			self.setMinimumSize(650, 500)

		def my_timer(self):
			self.xRot =self.xRot + 10
			self.yRot =self.yRot + 10
			self.zRot =self.zRot + 10
			self.update()
			
		def keyPressEvent(self, event):
			print("r0od")

			if type(event) == QtGui.QKeyEvent:
				print("rod")
				if event.text()=="r":
					if self.timer==None:
						self.timer=QTimer()
						self.timer.timeout.connect(self.my_timer)
						self.timer.start(100)
					else:
						self.timer.stop()
						self.timer=None
						
		def mouseMoveEvent(self,event):
			if 	self.timer!=None:
				self.timer.stop()
				self.timer=None

			if self.lastPos==None:
				self.lastPos=event.pos()
			dx = event.x() - self.lastPos.x();
			dy = event.y() - self.lastPos.y();

			if event.buttons()==Qt.LeftButton:
				
				self.xRot =self.xRot + 1 * dy
				self.yRot =self.yRot + 1 * dx
			elif event.buttons()==Qt.RightButton:
				self.xRot =self.xRot + 1 * dy
				self.zRot =self.zRot + 1 * dx
				
			self.lastPos=event.pos()
			self.setFocusPolicy(Qt.StrongFocus)
			self.setFocus()
			self.update()
			
		def paintGL(self):
			if self.enabled==True:
			
				self.emission=False
				lines=[]
				for i in range(0,epitaxy_get_layers()):
					if epitaxy_get_pl_file(i)!="none":
						if inp_load_file(lines,epitaxy_get_pl_file(i)+".inp")==True:
							if str2bool(lines[1])==True:
								self.emission=True
						
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
				glLoadIdentity()

				glTranslatef(-0.5, -0.5, -7.0) # Move Into The Screen
				
				glRotatef(self.xRot, 1.0, 0.0, 0.0)
				glRotatef(self.yRot, 0.0, 1.0, 0.0)
				glRotatef(self.zRot, 0.0, 0.0, 1.0)

				glColor3f( 1.0, 1.5, 0.0 )
				glPolygonMode(GL_FRONT, GL_FILL);


				#glClearColor(0.92, 0.92, 0.92, 0.5) # Clear to black.
				glClearColor(0.0, 0.0, 0.0, 0.5)
				lines=[]

				draw_mode(2.0)

				if self.suns!=0:
					if self.suns<=0.01:
						den=1.4
					elif self.suns<=0.1:
						den=0.8
					elif self.suns<=1.0:
						den=0.6
					elif self.suns<=10.0:
						den=0.3
					else:
						den=0.2
				
					x=np.arange(0, 2.0 , den)
					y=np.arange(0, 2.0 , den)
					for i in range(0,len(x)):
						for ii in range(0,len(y)):
							draw_photon(x[i],y[ii],False)

				if self.emission==True:
					den=0.6
					x=np.arange(0, 2.0 , den)
					y=np.arange(0, 2.0 , den)
					for i in range(0,len(x)):
						for ii in range(0,len(y)):
								draw_photon(x[i]+0.1,y[ii]+0.1,True)


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

						if xpoints==1 and zpoints==1:
							box(0.0,pos,0,width,thick,depth,red,green,blue)
						else:
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
					self.renderText (width+0.1,pos+thick/2,depth, text,font)

					pos=pos+thick+0.05


			
					glRotatef(self.tet_rotate, tet_x_rate, tet_y_rate, tet_z_rate)

		def recalculate(self):
			self.colors=[]
			lines=[]

		
			val=inp_get_token_value("light.inp", "#Psun")
			self.suns=float(val)
			l=epitaxy_get_layers()-1
			for i in range(0,epitaxy_get_layers()):

				path=os.path.join(get_materials_path(),epitaxy_get_mat_file(l-i),"mat.inp")


				if inp_load_file(lines,path)==True:
					red=float(inp_search_token_value(lines, "#Red"))
					green=float(inp_search_token_value(lines, "#Green"))
					blue=float(inp_search_token_value(lines, "#Blue"))
				else:

					red=0.0
					green=0.0
					blue=0.0
				self.colors.append(color(red,green,blue))
			self.colors.reverse()
			self.update()

		def initializeGL(self):
			self.enabled=True
			self.recalculate()

			glClearDepth(1.0)              
			glDepthFunc(GL_LESS)
			glEnable(GL_DEPTH_TEST)
			glShadeModel(GL_SMOOTH)
		
			glViewport(0, 0, self.width(), self.height()+100)
			glMatrixMode(GL_PROJECTION)
			glLoadIdentity()                    
			gluPerspective(45.0,float(self.width()) / float(self.height()+100),0.1, 100.0) 
			glMatrixMode(GL_MODELVIEW)
else:
	class glWidget(QWidget):
		def __init__(self, parent):
			QWidget.__init__(self)
			self.enabled=False
			return
