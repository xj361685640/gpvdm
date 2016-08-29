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
from util import read_data_2d

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
from mesh import mesh_get_ylen
from mesh import mesh_get_zlen

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

import random
from math import pi,acos,sin,cos

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

stars=[]

def draw_stars():
	global stars
	if len(stars)==0:
		
		for i in range(0,5000):
			phi = random.uniform(0,2*pi)
			costheta = random.uniform(-1,1)
			theta = acos( costheta )
			r=70+random.uniform(0,300)
			x = r * sin( theta) * cos( phi )
			y = r * sin( theta) * sin( phi )
			z = r * cos( theta )
			color=random.uniform(0,1.0)
			r=color
			g=color
			b=color
			s=random.uniform(1,3)	
			stars.append([x,y,z,r,g,b,s])
	
		stars.append([x,4,z,0.5,0.0,0.0,5])
		
	for i in range(0,len(stars)):
		glPointSize(stars[i][6])
		glBegin(GL_POINTS)
		glColor3f(stars[i][3],stars[i][4],stars[i][5])
		glVertex3f(stars[i][0],stars[i][1],stars[i][2])
		#glVertex3f(-1.0,-1.0,0.0)
		glEnd()


def draw_grid():
	glLineWidth(1)


	glColor3f(0.5, 0.5, 0.5)

	start_x=-18.0
	stop_x=20.0
	n=int(stop_x-start_x)
	dx=1.0#(stop_x-start_x)/n
	pos=start_x
	glBegin(GL_LINES)
	for i in range(0,n+1):
		glVertex3f(start_x, 0.0, pos)
		glVertex3f(stop_x, 0.0, pos)
		pos=pos+dx


	start_z=-18.0
	stop_z=20.0
	dz=1.0#(stop_z-start_z)/n
	pos=start_z
	for i in range(0,n+1):
		glVertex3f(pos, 0, start_z)
		glVertex3f(pos, 0, stop_z)
		pos=pos+dz

	glEnd()



def draw_photon(x,z,up):
	glLineWidth(3)
	length=0.9
	if up==True:
		glColor3f(0.0, 0.0, 1.0)
	else:
		glColor3f(0.0, 1.0, 0.0)

	glBegin(GL_LINES)
	wx=np.arange(0, length , 0.025)
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

def draw_mode(z_size,depth):

	glLineWidth(5)
	glColor3f(1.0, 1.0, 1.0)
	glBegin(GL_LINES)
	t=[]
	s=[]
	z=[]
	start=0.0
	if read_xyz_data(t,s,z,os.path.join(os.getcwd(),"light_dump","light_1d_photons_tot_norm.dat"))==True:
		array_len=len(t)
		t.reverse()
		s.reverse()		
		for i in range(1,array_len):
			glVertex3f(0.0, start+(z_size*(i-1)/array_len), depth+s[i-1]*0.5)
			glVertex3f(0.0, start+(z_size*i/array_len), depth+s[i]*0.5)

	glEnd()

def box_lines(x,y,z,w,h,d):

	glLineWidth(10)
	
	glBegin(GL_LINES)

	glColor3f(1.0,1.0,1.0)

	#btm

	glVertex3f(x+0.0,y+0.0,z+0.0)
	glVertex3f(x+w,y+ 0.0,z+0.0)

	glVertex3f(x+w,y+ 0.0,z+0.0)
	glVertex3f(x+w,y+ 0.0,z+d)

	glVertex3f(x+w,y+ 0.0,z+d)
	glVertex3f(x+ 0.0, y+0.0,z+ d) 


	#
	glVertex3f(x+0.0,y+h,z+0.0)
	glVertex3f(x+w,y+ h,z+0.0)


	glVertex3f(x+w,y+ h,z+0.0)
	glVertex3f(x+w,y+ h,z+d)
	
	glVertex3f(x+w,y+ h,z+d)	
	glVertex3f(x+ 0.0, y+h,z+ d) 

	#right

	glVertex3f(x+w,y,z)
	glVertex3f(x+w,y+ h,z)

	glVertex3f(x+w,y+ h,z)
	glVertex3f(x+w,y+ h,z+d)

	glVertex3f(x+w,y+ h,z+d)	
	glVertex3f(x+w, y,z+d) 

	#left

	glVertex3f(x,y,z)
	glVertex3f(x,y+ h,z)

	glVertex3f(x,y+ h,z)
	glVertex3f(x,y+ h,z+d)
	
	glVertex3f(x,y+ h,z+d)
	glVertex3f(x, y,z+d) 


#
	glVertex3f(x,y,z+d)
	glVertex3f(x+w,y,z+d)

	glVertex3f(x+w,y,z+d)
	glVertex3f(x+w,y+h,z+d)

	glVertex3f(x+w,y+h,z+d)	
	glVertex3f(x, y+h,z+d) 


	#top
	glVertex3f(x,y+h,z)
	glVertex3f(x+w,y+ h,z)

	glVertex3f(x+w,y+ h,z)
	glVertex3f(x+w,y+ h,z+ d)
	
	glVertex3f(x+w,y+ h,z+ d)
	glVertex3f(x, y+h,z+ d) 

	glEnd()

def graph(xstart,ystart,z,w,h,x_scale,y_scale,z_data):
	xpoints=len(x_scale)
	ypoints=len(y_scale)
	
	dx=w/xpoints
	dy=h/ypoints

	glBegin(GL_QUADS)

	my_max=z_data[0][0]
	for x in range(0,xpoints):
		for y in range(0,ypoints):
			if z_data[x][y]>my_max:
				my_max=z_data[x][y]



	for x in range(0,xpoints):
		for y in range(0,ypoints):

			glColor4f(z_data[x][y]/my_max,0.0,0.0, 0.7)
			#glColor3f(z_data[x][y]/my_max,0.0,0.0)
			glVertex3f(xstart+dx*x,ystart+y*dy, z)
			glVertex3f(xstart+dx*(x+1),ystart+y*dy, z)
			glVertex3f(xstart+dx*(x+1),ystart+dy*(y+1), z)
			glVertex3f(xstart+dx*x, ystart+dy*(y+1), z) 


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
			self.graph_path="./snapshots/159/Jn.dat"
			self.xRot =25.0
			self.yRot =-20.0
			self.zRot =0.0
			self.zoom=-8.0
			self.enabled=False
			self.timer=None
			self.zoom_timer=None
			
			self.selected_layer=-1
			QGLWidget.__init__(self, parent)
			self.lastPos=None
			#glClearDepth(1.0)              
			#glDepthFunc(GL_LESS)
			#glEnable(GL_DEPTH_TEST)
			#glShadeModel(GL_SMOOTH)
		
			self.setMinimumSize(650, 500)

		def my_timer(self):
			#self.xRot =self.xRot + 2
			self.yRot =self.yRot + 2
			#self.zRot =self.zRot + 5
			
			self.update()

		def fzoom_timer(self):
			self.zoom =self.zoom+4.0
			if self.zoom>-12.0:
				self.zoom_timer.stop()
			self.update()

		def start_rotate(self):
			self.timer=QTimer()
			self.timer.timeout.connect(self.my_timer)
			self.timer.start(50)

		def keyPressEvent(self, event):

			if type(event) == QtGui.QKeyEvent:
				if event.text()=="f":
					self.showFullScreen()
				if event.text()=="r":
					if self.timer==None:
						self.start_rotate()
					else:
						self.timer.stop()
						self.timer=None
				if event.text()=="z":
					if self.timer==None:
						self.start_rotate()
						
						self.zoom =-400
						self.zoom_timer=QTimer()
						self.zoom_timer.timeout.connect(self.fzoom_timer)
						self.zoom_timer.start(50)
					else:
						self.zoom_timer.stop()
						self.zoom_timer=None
						
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
				
			self.lastPos=event.pos()
			self.setFocusPolicy(Qt.StrongFocus)
			self.setFocus()
			self.update()

		def mouseReleaseEvent(self,event):
			self.lastPos=None
			
		def wheelEvent(self,event):
			p=event.angleDelta()
			self.zoom =self.zoom + p.y()/120
			self.update()

		def paintGL(self):
			if self.enabled==True:

				width=mesh_get_xlen()/1e-3
				depth=mesh_get_zlen()/1e-3
			
				l=epitaxy_get_layers()-1

				xpoints=int(mesh_get_xpoints())
				ypoints=int(mesh_get_ypoints())
				zpoints=int(mesh_get_zpoints())

				x_len=mesh_get_xlen()

				self.emission=False
				lines=[]
			#	for i in range(0,epitaxy_get_layers()):
			#		if epitaxy_get_pl_file(i)!="none":
			#			if inp_load_file(lines,epitaxy_get_pl_file(i)+".inp")==True:
			#				if str2bool(lines[1])==True:
			#					self.emission=True
						
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
				glLoadIdentity()

				glTranslatef(-0.5, -0.5, self.zoom) # Move Into The Screen
				
				glRotatef(self.xRot, 1.0, 0.0, 0.0)
				glRotatef(self.yRot, 0.0, 1.0, 0.0)
				glRotatef(self.zRot, 0.0, 0.0, 1.0)

				glColor3f( 1.0, 1.5, 0.0 )
				glPolygonMode(GL_FRONT, GL_FILL);


				#glClearColor(0.92, 0.92, 0.92, 0.5) # Clear to black.
				glClearColor(0.0, 0.0, 0.0, 0.5)
				lines=[]


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
				
					x=np.arange(0, width , den)
					z=np.arange(0, depth , den)
					for i in range(0,len(x)):
						for ii in range(0,len(z)):
							draw_photon(x[i],z[ii],False)

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
				
				for i in range(0,epitaxy_get_layers()):

					thick=epitaxy_get_width(l-i)*mul

					red=self.colors[l-i].r
					green=self.colors[l-i].g
					blue=self.colors[l-i].b

					if i==l-self.selected_layer:
						box_lines(0.0,pos,0,width,thick,depth)

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
					
					elif epitaxy_get_electrical_layer(l-i).lower()=="contact" and i==l:
						if xpoints==1 and zpoints==1:
							box(0.0,pos,0,width,thick,depth,red,green,blue)
						else:
							for c in contacts_get_array():
								xstart=width*(c.start/x_len)
								xwidth=width*(c.width/x_len)
								print("contacts",xstart,xwidth,c.width,x_len)
								if (c.start+c.width)>x_len:
									xwidth=width-xstart
								if c.active==True:
									box(xstart,pos,0,xwidth,thick,depth,0.0,1.0,0.0)
								else:
									box(xstart,pos,0,xwidth,thick,depth,red,green,blue)


					else:
						box(0.0,pos,0,width,thick,depth,red,green,blue)
					

					if epitaxy_get_electrical_layer(l-i).startswith("dos")==True:
						text=epitaxy_get_name(l-i)+" (active)"
					else:
						text=epitaxy_get_name(l-i)

					glColor3f(1.0,1.0,1.0)
					font = QFont("Arial")
					font.setPointSize(18)
					if self.zoom>-10:
						self.renderText (width+0.1,pos+thick/2,depth, text,font)

					pos=pos+thick+0.05


			
					glRotatef(self.tet_rotate, tet_x_rate, tet_y_rate, tet_z_rate)

				draw_mode(pos-0.05,depth)
				print(self.graph_path)

				graph(0.0,0.0,depth+0.5,width,pos,self.x_scale,self.y_scale,self.z_data)
				draw_grid()
				if self.zoom<-60:
					draw_stars()
					
		def recalculate(self):
			self.colors=[]
			lines=[]

			self.x_scale=[]
			self.y_scale=[]
			self.z_data=[]

			read_data_2d(self.x_scale,self.y_scale,self.z_data,self.graph_path)


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
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);
			#glEnable(GL_PROGRAM_POINT_SIZE_EXT);
			glShadeModel(GL_SMOOTH)
		
			glViewport(0, 0, self.width(), self.height()+100)
			glMatrixMode(GL_PROJECTION)
			glLoadIdentity()                    
			gluPerspective(45.0,float(self.width()) / float(self.height()+100),0.1, 1000.0) 
			glMatrixMode(GL_MODELVIEW)

else:
	class glWidget(QWidget):
		def __init__(self, parent):
			QWidget.__init__(self)
			self.enabled=False
			return
