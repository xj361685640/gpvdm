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
open_gl_ok=False

try:
	from OpenGL.GLU import *
	from OpenGL.GL import *
	from PyQt5 import QtOpenGL
	from PyQt5.QtOpenGL import QGLWidget
	open_gl_ok=True
except:
	print("opengl error ",sys.exc_info()[0])

from PyQt5 import QtGui
from PyQt5.QtCore import Qt
from PyQt5.QtGui import QScreen
from PyQt5.QtWidgets import QWidget, QHBoxLayout,QMenu

import os

#inp
from inp import inp_load_file
from inp_util import inp_search_token_value

#path
from cal_path import get_materials_path
from cal_path import get_sim_path

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
from epitaxy import epitaxy_get_y_len
from epitaxy import epitaxy_get_layer
from epitaxy import epitaxy_get_epi

#qt
from PyQt5.QtGui import QFont
import numpy as np
from inp import inp_get_token_value
from util import str2bool

from PyQt5.QtCore import QTimer

import random

from dat_file import dat_file
from dat_file import dat_file_read
from dat_file_math import dat_file_max_min

import glob

from gl_lib import draw_stars
from gl_lib import draw_grid
from gl_lib import draw_photon
from gl_lib import box_lines
from gl_lib import box

from gl_lib_ray import draw_rays
from gl_lib_ray import fast_data

from global_objects import global_object_register

from inp import inp_search_token_array
from math import fabs
from gl_color import color
# Rotations for cube.
cube_rotate_x_rate = 0.2
cube_rotate_y_rate = 0.2
cube_rotate_z_rate = 0.2


from gl_color import set_color
from gl_color import clear_color
from gl_color import set_color_alpha
from gl_color import print_color
from gl_color import set_false_color
from gl_color import search_color

from gl_save import gl_save_clear
from gl_save import gl_save_print
from gl_save import gl_save_save
from gl_lib import gl_save_draw
from gl_save import gl_save_load

from open_save_dlg import save_as_filter
import time

def tab(x,y,z,w,h,d):
	glBegin(GL_QUADS)
	set_color(0.0,0.0,1.0,"tab")
	glVertex3f(x+w+0.05,y,z)
	glVertex3f(x+w+0.2,y ,z)
	glVertex3f(x+w+0.2,y+h ,z)
	glVertex3f(x+w+0.05,y+h ,z)

	glEnd()


	
def draw_mode(z_size,depth):

	glLineWidth(5)
	set_color(1.0, 1.0, 1.0,"mode")
	glBegin(GL_LINES)
	t=[]
	s=[]
	z=[]
	start=0.0
	data=dat_file()
			
	path=os.path.join(get_sim_path(),"light_dump","light_1d_photons_tot_norm.dat")
	if dat_file_read(data,path)==True:
		array_len=data.y_len
		s=data.data[0][0]
		s.reverse()
		#print(path)
		#print(data.data)
		for i in range(1,array_len):
			glVertex3f(0.0, start+(z_size*(i-1)/array_len), depth+s[i-1]*0.5)
			glVertex3f(0.0, start+(z_size*i/array_len), depth+s[i]*0.5)

	glEnd()



def val_to_rgb(v):

	dx=1.0
	r=0*v/dx
	g=0*v/dx
	b=1*v/dx	
	return r,g,b

	dx=1/6.0

	if v<dx:
		r=0*v/dx
		g=0*v/dx
		b=1*v/dx
	elif v<dx*2:
		r=0*(v-dx)/dx
		g=1*(v-dx)/dx
		b=1*(v-dx)/dx
	elif v<dx*3:
		r=0*(v-2*dx)/dx
		g=1*(v-2*dx)/dx
		b=0*(v-2*dx)/dx
	elif v<dx*4:
		r=1*(v-3*dx)/dx
		g=1*(v-3*dx)/dx
		b=0*(v-3*dx)/dx
	elif v<dx*5:
		r=1*(v-4*dx)/dx
		g=0*(v-4*dx)/dx
		b=0*(v-4*dx)/dx
	else:
		r=1*(v-5*dx)/dx
		g=1*(v-5*dx)/dx
		b=1*(v-5*dx)/dx
		
	return r,g,b

		
		
def graph(xstart,ystart,z,w,h,z_range,graph_data):
	xpoints=graph_data.x_len
	ypoints=graph_data.y_len
	
	if xpoints>0 and ypoints>0:
		
		dx=w/xpoints
		dy=h/ypoints

		glBegin(GL_QUADS)


		if z_range==0.0:
			z_range=1.0

		for x in range(0,xpoints):
			for y in range(0,ypoints):
				r,g,b=val_to_rgb(graph_data.data[0][x][y]/z_range)
				glColor4f(r,g,b, 0.7)
				glVertex3f(xstart+dx*x,ystart+y*dy, z)
				glVertex3f(xstart+dx*(x+1),ystart+y*dy, z)
				glVertex3f(xstart+dx*(x+1),ystart+dy*(y+1), z)
				glVertex3f(xstart+dx*x, ystart+dy*(y+1), z) 


		glEnd()
	

class view_point():
	def __init__(self):
		self.xRot =25.0
		self.yRot =-20.0
		self.zRot =0.0
		self.x_pos=-0.5
		self.y_pos=-0.5
		self.zoom=-12.0


	def shift(self,target):
		stop=False
		move=0.0
		max_angle_shift=4.0
		max_xy_shift=0.2
		delta=(target.xRot-self.xRot)
		if fabs(delta)>max_angle_shift:
			delta=max_angle_shift*delta/fabs(delta)

		self.xRot=self.xRot+delta
		move=move+fabs(delta)

		delta=(target.yRot-self.yRot)
		if fabs(delta)>max_angle_shift:
			delta=max_angle_shift*delta/fabs(delta)

		self.yRot=self.yRot+delta
		move=move+fabs(delta)

		delta=(target.zRot-self.zRot)
		if fabs(delta)>max_angle_shift:
			delta=max_angle_shift*delta/fabs(delta)

		self.zRot=self.zRot+delta
		move=move+fabs(delta)
		
		delta=(target.x_pos-self.x_pos)
		if fabs(delta)>max_xy_shift:
			delta=max_xy_shift*delta/fabs(delta)

		self.x_pos=self.x_pos+delta
		move=move+fabs(delta)
		
		delta=(target.y_pos-self.y_pos)
		if fabs(delta)>max_xy_shift:
			delta=max_xy_shift*delta/fabs(delta)

		self.y_pos=self.y_pos+delta
		move=move+fabs(delta)

		delta=(target.zoom-self.zoom)
		if fabs(delta)>1.0:
			delta=1.0*delta/fabs(delta)

		self.zoom=self.zoom+delta
		move=move+fabs(delta)
		
		if move==0.0:
			stop=True

		return stop

	def set_value(self,data):
		self.xRot=data.xRot
		self.yRot=data.yRot
		#self.zRot=data.zRot
		self.x_pos=data.x_pos
		self.y_pos=data.y_pos
		self.zoom=data.zoom
			
if open_gl_ok==True:		
	class glWidget(QGLWidget):

		def __init__(self, parent):
			QGLWidget.__init__(self, parent)
			self.setAutoBufferSwap(False)

			self.failed=True
			self.graph_path="./snapshots/159/Jn.dat"
			self.graph_z_max=1.0
			self.graph_z_min=1.0
			#view pos
			self.viewpoint=view_point()
			self.viewpoint.xRot =25.0
			self.viewpoint.yRot =-20.0
			self.viewpoint.zRot =0.0
			self.viewpoint.x_pos=-0.5
			self.viewpoint.y_pos=-0.5
			self.viewpoint.zoom=-12.0

			self.viewtarget=view_point()
			#self.viewtarget.set_value(self.viewpoint)
			self.viewtarget.xRot=0.0
			self.viewtarget.yRot=0.0
			self.viewtarget.zRot=0.0
			self.viewtarget.x_pos=-2.0
			self.viewtarget.y_pos=-1.7
			self.viewtarget.zoom=-8.0



			self.render_grid=True
			self.render_photons=True
			self.render_text=True

			self.timer=None
			
			self.suns=0.0
			self.selected_layer=-1
			self.graph_data=dat_file()
			self.lastPos=None
			self.ray_file=""
			self.mouse_click_time=0.0


		def my_timer(self):
			#self.xRot =self.xRot + 2
			self.viewpoint.yRot =self.viewpoint.yRot + 2
			#self.zRot =self.zRot + 5
			
			self.update()

		def ftimer_target(self):
			stop=self.viewpoint.shift(self.viewtarget)
			
			self.update()
			if stop==True:
				self.timer.stop()

		def fzoom_timer(self):
			self.viewpoint.zoom =self.viewpoint.zoom+4.0
			if self.viewpoint.zoom>-12.0:
				self.timer.stop()
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
						if self.viewpoint.zoom>-40:
							self.viewpoint.zoom =-400
						self.timer=QTimer()
						self.timer.timeout.connect(self.fzoom_timer)
						self.timer.start(50)
					else:
						self.timer.stop()
						self.timer=None
		def xy(self):
			self.viewtarget.xRot=0.0
			self.viewtarget.yRot=0.0
			self.viewtarget.zRot=0.0
			self.viewtarget.x_pos=-2.0
			self.viewtarget.y_pos=-1.7
			self.viewtarget.zoom=-8.0
			self.timer=QTimer()
			self.timer.timeout.connect(self.ftimer_target)
			self.timer.start(25)

		def yz(self):
			self.viewtarget.xRot=0.0
			self.viewtarget.yRot=-90
			self.viewtarget.zRot=0.0
			self.viewtarget.x_pos=2.0
			self.viewtarget.y_pos=-1.7
			self.viewtarget.zoom=-8.0
			self.timer=QTimer()
			self.timer.timeout.connect(self.ftimer_target)
			self.timer.start(25)

		def xz(self):
			self.viewtarget.xRot=90.0
			self.viewtarget.yRot=0.0
			self.viewtarget.zRot=0.0
			self.viewtarget.x_pos=-2.0
			self.viewtarget.y_pos=1.2
			self.viewtarget.zoom=-9.0
			self.timer=QTimer()
			self.timer.timeout.connect(self.ftimer_target)
			self.timer.start(25)

		def mouseDoubleClickEvent(self,event):
			x = event.x()
			y = self.height()-event.y()
			data=glReadPixelsub(x, y, 1, 1, GL_RGBA,GL_FLOAT)
#glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, pixelf);
			set_false_color(True)
			self.render()
			r=float(data[0][0][0])
			g=float(data[0][0][1])
			b=float(data[0][0][2])

			print(x,y,r,g,b,search_color(r,g,b))
			set_false_color(False)
			

		def mouseMoveEvent(self,event):
			if 	self.timer!=None:
				self.timer.stop()
				self.timer=None

			if self.lastPos==None:
				self.lastPos=event.pos()
			dx = event.x() - self.lastPos.x();
			dy = event.y() - self.lastPos.y();

			if event.buttons()==Qt.LeftButton:
				
				self.viewpoint.xRot =self.viewpoint.xRot + 1 * dy
				self.viewpoint.yRot =self.viewpoint.yRot + 1 * dx

			if event.buttons()==Qt.RightButton:
				self.viewpoint.x_pos =self.viewpoint.x_pos + 0.1 * dx
				self.viewpoint.y_pos =self.viewpoint.y_pos - 0.1 * dy


			self.lastPos=event.pos()
			self.setFocusPolicy(Qt.StrongFocus)
			self.setFocus()
			self.update()

		def mousePressEvent(self,event):
			self.mouse_click_time=time.time()

		def mouseReleaseEvent(self,event):
			if (time.time() - self.mouse_click_time)<0.2:
				self.menu(event)

			self.lastPos=None
		
		def save_as(self):
			self.random_device()
			return
			ret=save_as_filter(self,"3d (*.3d)")
			print(ret)
			if ret!=False:
				gl_save_save(ret)

		def menu(self,event):
			menu = QMenu(self)
			action=menu.addAction(_("Save image"))
			action.triggered.connect(self.save_as)

			#menu.exec_(self.emailbtn.mapToGlobal(QtCore.QPoint(0,0)))
			menu.exec_(event.globalPos())

		def wheelEvent(self,event):
			p=event.angleDelta()
			self.viewpoint.zoom =self.viewpoint.zoom + p.y()/120
			self.update()

		def draw_photons(self,max_gui_device_x,max_gui_device_z):
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
			
				x=np.arange(0, max_gui_device_x , den)
				z=np.arange(0, max_gui_device_z , den)
				for i in range(0,len(x)):
					for ii in range(0,len(z)):
						draw_photon(x[i],z[ii],False)

			if self.emission==True and self.ray_model==False:
				den=0.6
				x=np.arange(0, max_gui_device_x , den)
				y=np.arange(0, max_gui_device_z , den)
				for i in range(0,len(x)):
					for ii in range(0,len(y)):
						draw_photon(x[i]+0.1,y[ii]+0.1,True)

		def render(self):
			#print("do draw")
			clear_color()
			gl_save_clear()


			dos_start=-1
			dos_stop=-1
			epi_y_len=epitaxy_get_y_len()
			dy_layer_offset=0.05
			
			if epi_y_len<=0:
				return

			self.x_mul=1e3
			self.y_mul=1.4/epi_y_len
			self.z_mul=1e3


			x_len=mesh_get_xlen()

			max_gui_device_x=x_len*self.x_mul
			max_gui_device_y=1.0
			max_gui_device_z=mesh_get_zlen()*self.z_mul

			l=epitaxy_get_layers()-1

			xpoints=int(mesh_get_xpoints())
			ypoints=int(mesh_get_ypoints())
			zpoints=int(mesh_get_zpoints())

			if ypoints>10:
				ypoints=10
			
			if xpoints>10:
				xpoints=10
				
			if ypoints>10:
				ypoints=10

			self.emission=False
			self.ray_model=False
			
			lines=inp_load_file(os.path.join(get_sim_path(),"led.inp"))
			if lines!=False:
				self.ray_model=val=str2bool(inp_search_token_value(lines, "#led_on"))
				
			lines=[]

			for i in range(0,epitaxy_get_layers()):
				if epitaxy_get_pl_file(i)!="none":
					lines=inp_load_file(os.path.join(get_sim_path(),epitaxy_get_pl_file(i)+".inp"))
					if lines!=False:
						if str2bool(lines[1])==True:
							self.emission=True
					
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
			glLoadIdentity()

			glTranslatef(self.viewpoint.x_pos, self.viewpoint.y_pos, self.viewpoint.zoom) # Move Into The Screen
			
			glRotatef(self.viewpoint.xRot, 1.0, 0.0, 0.0)
			glRotatef(self.viewpoint.yRot, 0.0, 1.0, 0.0)
			glRotatef(self.viewpoint.zRot, 0.0, 0.0, 1.0)

			glColor3f( 1.0, 1.5, 0.0 )
			glPolygonMode(GL_FRONT, GL_FILL);

			threed_files=glob.glob("*.3d")
			if len(threed_files)>0:
				gl_save_load()
				gl_save_draw()
				draw_grid()
				return

			#glClearColor(0.92, 0.92, 0.92, 0.5) # Clear to black.
			glClearColor(0.0, 0.0, 0.0, 0.5)
			lines=[]

			if self.render_photons==True:
				self.draw_photons(max_gui_device_x,max_gui_device_z)

			pos=0.0
				
			for i in range(0,epitaxy_get_layers()):

				thick=epitaxy_get_width(l-i)*self.y_mul
				obj=epitaxy_get_layer(l-i)
				red=obj.r
				green=obj.g
				blue=obj.b
				alpha=obj.alpha
				if i==l-self.selected_layer:
					box_lines(0.0,pos,0,max_gui_device_x,thick,max_gui_device_z)

				if epitaxy_get_electrical_layer(l-i).startswith("dos")==True and ypoints!=0 and xpoints!=0 and zpoints!=0:
					dy=thick/float(ypoints)
					dx=max_gui_device_x/float(xpoints)
					dz=max_gui_device_z/float(zpoints)
					xshrink=0.8
					zshrink=0.8
					
					if dos_start==-1:
						dos_start=pos
					
					dos_stop=pos+thick
			
					if xpoints==1:
						xshrink=1.0

					if zpoints==1:
						zshrink=1.0

					if xpoints==1 and zpoints==1:
						box(0.0,pos,0,max_gui_device_x,thick,max_gui_device_z,red,green,blue,alpha)
					else:
						for y in range(0,ypoints):
							for x in range(0,xpoints):
								for z in range(0,zpoints):
									box(dx*x,pos+y*(dy),z*dz,dx*xshrink,dy*0.8,dz*zshrink,red,green,blue,alpha)
					tab(0.0,pos,max_gui_device_z,max_gui_device_x,thick,max_gui_device_z)
				
				elif epitaxy_get_electrical_layer(l-i).lower()=="contact" and (i==l or i==0):
					for c in contacts_get_array():
						if (c.position=="top" and i==l) or (c.position=="bottom" and i==0):
							if xpoints==1 and zpoints==1:
								xstart=0.0
								xwidth=max_gui_device_x
							else:
								xstart=max_gui_device_x*(c.start/x_len)
								xwidth=max_gui_device_x*(c.width/x_len)
								#print("contacts",xstart,xwidth,c.width,x_len)
								if (c.start+c.width)>x_len:
									xwidth=max_gui_device_x-xstart
								
							if c.depth>0.0:
								etch_depth=c.depth*self.y_mul
								if c.position=="top":
									box(xstart,pos-etch_depth-dy_layer_offset,0,xwidth,etch_depth,max_gui_device_z,0.0,0.0,1.0,1.0)
								else:
									box(xstart,pos+dy_layer_offset+thick,0,xwidth,etch_depth,max_gui_device_z,0.0,0.0,1.0,1.0)
									
							if c.active==True:
								box(xstart,pos,0,xwidth,thick,max_gui_device_z,0.0,1.0,0.0,alpha)
							else:
								box(xstart,pos,0,xwidth,thick,max_gui_device_z,red,green,blue,alpha)


				else:
					box(0.0,pos,0,max_gui_device_x,thick,max_gui_device_z,red,green,blue,alpha)
				

				if self.render_text==True:
					if epitaxy_get_electrical_layer(l-i).startswith("dos")==True:
						text=epitaxy_get_name(l-i)+" ("+_("active")+")"
					else:
						text=epitaxy_get_name(l-i)

					set_color(1.0,1.0,1.0,"text")

					font = QFont("Arial")
					font.setPointSize(18)
					if self.viewpoint.zoom>-20:
						self.renderText (max_gui_device_x+0.1,pos+thick/2,max_gui_device_z, text,font)

				pos=pos+thick+dy_layer_offset

			draw_mode(pos-dy_layer_offset,max_gui_device_z)
			draw_rays(self.ray_file,pos-dy_layer_offset,max_gui_device_x,self.y_mul,max_gui_device_z*1.05)
			#print(self.graph_path)

			full_data_range=self.graph_z_max-self.graph_z_min
			graph(0.0,dos_start,max_gui_device_z+0.5,max_gui_device_x,dos_stop-dos_start,full_data_range,self.graph_data)

			if self.render_grid==True:
				draw_grid()

			if self.viewpoint.zoom<-60:
				draw_stars()


		def do_draw(self):
			self.render()
			self.swapBuffers()
			#gl_save_print()

		def paintGL(self):
			if self.failed==False:
				self.do_draw()
					
		def load_data(self):
			lines=[]

			if dat_file_read(self.graph_data,self.graph_path)==True:
				#print(self.graph_path)
				self.graph_z_max,self.graph_z_min=dat_file_max_min(self.graph_data)
				#print(self.graph_z_max,self.graph_z_min)
			val=inp_get_token_value(os.path.join(get_sim_path(),"light.inp"), "#Psun")
			try:
				self.suns=float(val)
			except:
				self.suns=0.0

		def random_device(self):
			self.render_grid=True
			self.render_photons=True
			self.render_text=False

			for i in range(0,100):
				r=random.randint(0,epitaxy_get_layers()-1)
				w=float(random.randint(20,100))*1e-9
				e=epitaxy_get_epi()
				e[r].width=w
				levels = range(32,256,32)
				color=tuple(random.choice(levels) for _ in range(3))
				if r!=2:
					e[r].r=color[0]/256.0
					e[r].g=color[1]/256.0
					e[r].b=color[2]/256.0

				self.do_draw()
				self.grabFrameBuffer().save("./one/a"+str(i)+".png")
			
		def force_redraw(self):
			self.load_data()
			self.update()
			self.do_draw()

		def resizeEvent(self,event):
			if self.failed==False:
				#glClearDepth(1.0)              
				#glDepthFunc(GL_LESS)
				#glEnable(GL_DEPTH_TEST)
				#glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				#glEnable(GL_BLEND);
				#glShadeModel(GL_SMOOTH)
				glViewport(0, 0, self.width(), self.height()+100)
				glMatrixMode(GL_PROJECTION)
				glLoadIdentity()                    
				gluPerspective(45.0,float(self.width()) / float(self.height()+100),0.1, 1000.0) 
				glMatrixMode(GL_MODELVIEW)


		def initializeGL(self):
			self.load_data()
			try:
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
				#self.resizeEvent.connect(self.resize)
				
				self.failed=False
				global_object_register("gl_force_redraw",self.force_redraw)

			except:
				print("OpenGL failed to load falling back to 2D rendering.",sys.exc_info()[0])

else:
	class glWidget(QWidget):

		def __init__(self, parent):
			QWidget.__init__(self)
			self.failed=True
