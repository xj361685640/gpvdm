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
from numpy import *
from util import read_data_2d
from plot_io import plot_load_info
from plot_export import plot_export


#matplotlib
import matplotlib
matplotlib.use('Qt5Agg')
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar
import matplotlib.pyplot as plt

from util import numbers_to_latex
from util import pygtk_to_latex_subscript
from util import fx_with_units
from plot_state import plot_state
from plot_io import plot_save_oplot_file
from util import time_with_units

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QTableWidget,QAbstractItemView, QMenuBar
from PyQt5.QtGui import QPainter,QIcon

#calpath
from cal_path import get_image_file_path
from gui_util import save_as_image

from dat_file import dat_file_read
from dat_file import dat_file_max_min
from dat_file import dat_file

class plot_widget(QWidget):

	def keyPressEvent(self, event):
		
		keyname=event.key()
		
		if keyname=="a":
			self.do_plot()

		if keyname=="g":
			if self.plot_token.grid==False:
				for i in range(0,len(self.ax)):
					self.ax[i].grid(True)
				self.plot_token.grid=True
			else:
				for i in range(0,len(self.ax)):
					self.ax[i].grid(False)
				self.plot_token.grid=False
		if keyname=="r":
			if self.lx==None:
				for i in range(0,len(self.ax)):
					self.lx = self.ax[i].axhline(color='k')
					self.ly = self.ax[i].axvline(color='k')
			self.lx.set_ydata(self.ydata)
			self.ly.set_xdata(self.xdata)

		if keyname=="l":
			if self.plot_token.logy==True:
				self.plot_token.logy=False
				for i in range(0,len(self.ax)):
					self.ax[i].set_yscale("linear")
			else:
				self.plot_token.logy=True
				for i in range(0,len(self.ax)):
					self.ax[i].set_yscale("log")

		if keyname=="L":
			if self.plot_token.logx==True:
				self.plot_token.logx=False
				for i in range(0,len(self.ax)):
					self.ax[i].set_xscale("linear")
			else:
				self.plot_token.logx=True
				for i in range(0,len(self.ax)):
					self.ax[i].set_xscale("log")

		if keyname=="q":
			self.destroy()

		if keyname == "c":
			#print "clip",event.state,event.state& gtk.gdk.CONTROL_MASK
			if event.state & gtk.gdk.CONTROL_MASK==gtk.gdk.CONTROL_MASK:
				#print "yes"
				self.do_clip()

		self.fig.canvas.draw()
		plot_save_oplot_file(self.config_file,self.plot_token)

	def do_clip(self):
		snap = self.canvas.get_snapshot()
		pixbuf = gtk.gdk.pixbuf_get_from_drawable(None, snap, snap.get_colormap(),0,0,0,0,snap.get_size()[0], snap.get_size()[1])
		clip = gtk.Clipboard()
		clip.set_image(pixbuf)


	def mouse_move(self,event):
		#print event.xdata, event.ydata
		self.xdata=event.xdata
		self.ydata=event.ydata

		#self.fig.canvas.draw()

		#except:
		#	print "Error opening file ",file_name


	def sub_zero_frame(self,data,i):
		if self.zero_frame_enable==True:
			data_zero=dat_file()
			if dat_file_read(data_zero,self.zero_frame_list[i])==True:
				for x in range(0,data.x_len):
					for y in range(0,data.y_len):
						for z in range(0,data.z_len):
							data.data[z][x][y]=data.data[z][x][y]-data_zero.data[z][x][y]

	def read_data_file(self,data,index):
		if dat_file_read(data,self.input_files[index])==True:
			self.sub_zero_frame(data,index)
			my_min=0.0;


			for x in range(0,data.x_len):
				for y in range(0,data.y_len):
					for z in range(0,data.z_len):
						data.y_scale[y]=data.y_scale[y]*self.plot_token.x_mul
						data.data[z][x][y]=data.data[z][x][y]*self.plot_token.y_mul

						if self.plot_token.invert_y==True:
							data.data[z][x][y]=-data.data[z][x][y]

			if self.plot_token.subtract_first_point==True:
				val=data.data[0][0][0]
				for x in range(0,data.x_len):
					for y in range(0,data.y_len):
						for z in range(0,data.z_len):
							data.data[z][x][y]=data.data[z][x][y]-val


			if self.plot_token.add_min==True:
				my_max,my_min=dat_file_max_min(data)

				for x in range(0,data.x_len):
					for y in range(0,data.y_len):
						for z in range(0,data.z_len):
							data.data[z][x][y]=data.data[z][x][y]-my_min

			if self.plot_token.normalize==True:
				my_max,my_min=dat_file_max_min(data)
				for x in range(0,data.x_len):
					for y in range(0,data.y_len):
						for z in range(0,data.z_len):
							if data.data[z][x][y]!=0:
								data.data[z][x][y]=data.data[z][x][y]/local_max
							else:
								data.data[z][x][y]=0.0


			if index<len(self.plot_id):
				plot_number=self.plot_id[index]
			else:
				plot_number=index
			print("YMIN=",self.plot_token.ymin)
			if self.plot_token.ymax!=-1:
				self.ax[plot_number].set_ylim((self.plot_token.ymin,self.plot_token.ymax))
			return True
		else:
			return False

	def do_plot(self):
		print("PLOT TYPE=",self.plot_token.type)
		if self.plot_token!=None and len(self.plot_id)!=0:
			plot_number=0

			self.fig.clf()
			self.fig.subplots_adjust(bottom=0.2)
			self.fig.subplots_adjust(bottom=0.2)
			self.fig.subplots_adjust(left=0.1)
			self.fig.subplots_adjust(hspace = .001)

			title=""
			if self.plot_title=="":
				title=self.plot_token.title
			else:
				title=self.plot_title

			if self.plot_token.time!=-1.0 and self.plot_token.Vexternal!=-1.0:
				mul,unit=time_with_units(self.plot_token.time)
				title=title+" V="+str(self.plot_token.Vexternal)+" time="+str(self.plot_token.time*mul)+" "+unit

			self.fig.suptitle(title)

			self.ax=[]
			number_of_plots=max(self.plot_id)+1
			if self.plot_token.type=="heat":
				number_of_plots=1




			for i in range(0,number_of_plots):
				self.ax.append(self.fig.add_subplot(number_of_plots,1,i+1, axisbg='white'))
				#Only place label on bottom plot
				if i==number_of_plots-1:
					print(self.plot_token.x_label,self.plot_token.x_units)
					self.ax[i].set_xlabel(self.plot_token.x_label+" ("+str(self.plot_token.x_units)+")")

				else:
					self.ax[i].tick_params(axis='x', which='both', bottom='off', top='off',labelbottom='off') # labels along the bottom edge are off

				#Only place y label on center plot
				if self.plot_token.normalize==True or self.plot_token.norm_to_peak_of_all_data==True:
					y_text="Normalized "+self.plot_token.y_label
					y_units="au"
				else:
					y_text=self.plot_token.y_label
					y_units=self.plot_token.y_units
				if i==math.trunc(number_of_plots/2):
					self.ax[i].set_ylabel(y_text+" ("+y_units+")")

				if self.plot_token.logx==True:
					self.ax[i].set_xscale("log")

				if self.plot_token.logy==True:
					self.ax[i].set_yscale("log")


			lines=[]
			files=[]
			data=dat_file()
			my_max=1.0
			if self.plot_token.x_len==1 and self.plot_token.z_len==1:

				all_max=1.0
				if self.plot_token.norm_to_peak_of_all_data==True:
					my_max=-1e40
					for i in range(0, len(self.input_files)):
						if self.read_data_file(data,i)==True:
							local_max,my_min=dat_file_max_min(data)
							if local_max>my_max:
								my_max=local_max
					all_max=my_max

				for i in range(0, len(self.input_files)):
					if self.read_data_file(data,i)==True:
						if all_max!=1.0:
							for x in range(0,data.x_len):
								for y in range(0,data.y_len):
									for z in range(0,data.z_len):
										data.data[z][x][y]=data.data[z][x][y]/all_max

						Ec, = self.ax[plot_number].plot(data.y_scale,data.data[0][0], linewidth=3 ,alpha=1.0,color=self.color[i],marker=self.marker[i])

						#label data if required
						#if self.plot_token.label_data==True:
						#	for ii in range(0,len(t)):
						#		if z[ii]!="":
						#			fx_unit=fx_with_units(float(z[ii]))
						#			label_text=str(float(z[ii])*fx_unit[0])+" "+fx_unit[1]
						#			self.ax[plot_number].annotate(label_text,xy = (t[ii], s[ii]), xytext = (-20, 20),textcoords = 'offset points', ha = 'right', va = 'bottom',bbox = dict(boxstyle = 'round,pad=0.5', fc = 'yellow', alpha = 0.5),arrowprops = dict(arrowstyle = '->', connectionstyle = 'arc3,rad=0'))

						#if number_of_plots>1:
						#	self.ax[plot_number].yaxis.set_major_formatter(ticker.FormatStrFormatter('%0.1e'))
						#	if min(s)!=max(s):
						#		print("TICKS=",(max(s)-min(s))/4.0)
						#		self.ax[plot_number].yaxis.set_ticks(arange(min(s), max(s), (max(s)-min(s))/4.0 ))

						#print("roderick",self.labels,i,self.labels[i])
						if self.labels[i]!="":
							#print "Rod=",self.labels[i]
							#print self.plot_token.key_units
							files.append("$"+numbers_to_latex(str(self.labels[i]))+" "+pygtk_to_latex_subscript(self.plot_token.key_units)+"$")

							lines.append(Ec)

				self.lx = None
				self.ly = None
				if self.plot_token.legend_pos=="No key":
					self.ax[plot_number].legend_ = None
				else:
					self.fig.legend(lines, files, self.plot_token.legend_pos)
			elif self.plot_token.x_len>1 and self.plot_token.y_len>1 and self.plot_token.z_len==1:		#3d plot
				data=dat_file()
				print("year2",self.input_files[0])
				if self.read_data_file(data,0)==True:
					print("year")
					self.ax[0].pcolor(data.data[0])

					#self.ax[0].plot_surface(x, y, z, rstride=1, cstride=1, cmap=cm.coolwarm,linewidth=0, antialiased=False)
					#self.ax[0].invert_yaxis()
					#self.ax[0].xaxis.tick_top()
			elif self.plot_token.type=="heat":
				x=[]
				y=[]
				z=[]

				pos=float(self.plot_token.x_start)
				x_step=(float(self.plot_token.x_stop)-float(self.plot_token.x_start))/self.plot_token.x_points
				while(pos<float(self.plot_token.x_stop)):
					x.append(pos)
					pos=pos+x_step

				pos=float(self.plot_token.y_start)
				y_step=(float(self.plot_token.y_stop)-float(self.plot_token.y_start))/self.plot_token.y_points
				while(pos<float(self.plot_token.y_stop)):
					y.append(pos)
					pos=pos+y_step

				data = zeros((len(y),len(x)))

				for ii in range(0,len(self.input_files)):
					t=[]
					s=[]
					z=[]
					if self.read_data_file(t,s,z,ii)==True:
						print(self.input_files[ii])
						for points in range(0,len(t)):
							found=0
							if t[points]>x[0]:
								for x_pos in range(0,len(x)):
									if x[x_pos]>t[points]:
										found=found+1
										break

							if s[points]>y[0]:
								for y_pos in range(0,len(y)):
									if y_pos!=0:
										if y[y_pos]>s[points]:
											found=found+1
											break
							if found==2:
								print("adding data at",x_pos,y_pos)
								if data[y_pos][x_pos]<10.0:
									data[y_pos][x_pos]=data[y_pos][x_pos]+1
							else:
								print("not adding point",t[points],s[points])

				print(x)
				print(y)
				print(data)
				x_grid, y_grid = mgrid[float(self.plot_token.y_start):float(self.plot_token.y_stop):complex(0, len(y)), float(self.plot_token.x_start):float(self.plot_token.x_stop):complex(0, len(x))]
				self.ax[0].pcolor(y_grid,x_grid,data)

			else:
				x=[]
				y=[]
				z=[]

				if read_data_2d(x,y,z,self.input_files[0])==True:
					maxx=-1
					maxy=-1
					for i in range(0,len(z)):
						if x[i]>maxx:
							maxx=x[i]

						if y[i]>maxy:
							maxy=y[i]

					maxx=maxx+1
					maxy=maxy+1

					data = zeros((maxy,maxx))


					for i in range(0,len(z)):
						data[y[i]][x[i]]= random.random()+5
						self.ax[0].text(x[i], y[i]+float(maxy)/float(len(z))+0.1,'%.1e' %  z[i], fontsize=12)

					#fig, ax = plt.subplots()
					self.ax[0].pcolor(data,cmap=plt.cm.Blues)

					self.ax[0].invert_yaxis()
					self.ax[0].xaxis.tick_top()

			#self.fig.tight_layout(pad=0.0, w_pad=0.0, h_pad=0.0)
			self.fig.canvas.draw()
			print("exit do plot")

	def callback_plot_save(self):
		response=save_as_image(self)
		if response != None:
			plot_export(response,self.input_files,self.plot_token,self.fig)

	def set_labels(self,labels):
		self.labels=labels

	def set_plot_ids(self,plot_id):
		self.plot_id=plot_id

	def load_data(self,input_files,config_file):

		self.input_files=input_files
		self.config_file=config_file

		if config_file=="":
			config_file=os.path.splitext(input_files[0])[0]+".oplot"

		loaded=False
		self.plot_token=plot_state()

		#Try and get the data from the config file
		if plot_load_info(self.plot_token,config_file)==True:
			loaded=True
			print("I HAVE LOADED THE OPLOT FILE",self.plot_token.type)

		#If that did not work get it from the data file
		if loaded==False:
			if plot_load_info(self.plot_token,input_files[0])==True:
				loaded=True

		print("the config file is",config_file)
		print(input_files,loaded)
		loaded=True
		if loaded==True:

			if len(self.plot_id)==0:
				for i in range(0,len(input_files)):
					self.plot_id.append(0)

			self.plot_token.path=os.path.dirname(config_file)
			if self.plot_token.tag0=="":
				self.plot_token.file0=os.path.basename(input_files[0])

			plot_save_oplot_file(config_file,self.plot_token)

			self.output_file=os.path.splitext(config_file)[0]+".png"

			#ret=plot_populate_plot_token(plot_token,self.input_files[0])
			#if ret==True:
			#print "Rod",input_files
			title=self.plot_token.title
			self.setWindowTitle(title+" - www.gpvdm.com")

			print("Loaded OK",self.config_file)

			test_file=self.input_files[0]
			for i in range(0,len(self.input_files)):
				if os.path.isfile(self.input_files[i]):
					test_file=self.input_files[i]

			print("test_file=",test_file)
			print("Exit here")


	def gen_colors_black(self,repeat_lines):
		#make 100 black colors
		marker_base=["","x","o"]
#		c_tot=[]
		base=[[0.0,0.0,0.0]]
		self.marker=[]
		self.color=[]
		for i in range(0,100):
			for n in range(0,repeat_lines):
				self.color.append([base[0][0],base[0][1],base[0][2]])
				self.marker.append(marker_base[n])

	def gen_colors(self,repeat_lines):
		base=[[0.0,0.0,1.0],[0.0,1.0,0.0],[1.0,0.0,0.0],[0.0,1.0,1.0],[1.0,1.0,0.0],[1.0,0.0,1.0]]
		c_tot=[]
		self.marker=[]
		marker_base=["","x","o"]
		mul=1.0
		self.color=[]
		for rounds in range(0,20):
			for i in range(0,len(base)):
				for n in range(0,repeat_lines):
					c_tot.append([base[i][0]*mul,base[i][1]*mul,base[i][2]*mul])
					self.marker.append(marker_base[n])
			mul=mul*0.5

		self.color=c_tot

	def callback_black(self):
		self.gen_colors_black(1)
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_rainbow(self):
		self.gen_colors(1)
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_save(self):
		plot_export(self.output_file,self.input_files,self.plot_token,self.fig)

	def callback_key(self):
		self.plot_token.legend_pos=widget.get_label()
		print(self.config_file,self.plot_token)
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_units(self):
		units=dlg_get_text( "Units:", self.plot_token.key_units)
		if units!=None:
			self.plot_token.key_units=units
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()


	def callback_autoscale_y(self):
		if self.plot_token.ymax==-1:
			xmin, xmax, ymin, ymax = self.ax[0].axis()
			self.plot_token.ymax=ymax
			self.plot_token.ymin=ymin
		else:
			self.plot_token.ymax=-1
			self.plot_token.ymin=-1

	def callback_normtoone_y(self):
		self.plot_token.normalize= not self.plot_token.normalize
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_norm_to_peak_of_all_data(self):
		self.plot_token.norm_to_peak_of_all_data=not self.plot_token.norm_to_peak_of_all_data
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_toggle_log_scale_y(self):
		self.plot_token.logy=not self.plot_token.logy
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_toggle_log_scale_x(self):
		self.plot_token.logx=not self.plot_token.logx
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_toggle_label_data(self):
		self.plot_token.label_data=not self.plot_token.label_data
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_set_heat_map(self):
		self.plot_token.type="heat"
		plot_save_oplot_file(self.config_file,self.plot_token)
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_heat_map_edit(self):
		[a,b,c,d,e,f] = dlg_get_multi_text("2D plot editor", [["x start",str(self.plot_token.x_start)],["x stop",str(self.plot_token.x_stop)],["x points",str(self.plot_token.x_points)],["y start",str(self.plot_token.y_start)],["y stop",str(self.plot_token.y_stop)],["y points",str(self.plot_token.y_points)]])
		print("---------",a,b,c,d,e,f)
		self.plot_token.x_start=float(a)
		self.plot_token.x_stop=float(b)
		self.plot_token.x_points=float(c)

		self.plot_token.y_start=float(d)
		self.plot_token.y_stop=float(e)
		self.plot_token.y_points=float(f)

		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_set_xy_plot(self):
		self.plot_token.type="xy"
		plot_save_oplot_file(self.config_file,self.plot_token)
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_toggle_invert_y(self):
		self.plot_token.invert_y=not self.plot_token.invert_y
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_toggle_subtract_first_point(self):
		self.plot_token.subtract_first_point=not self.plot_token.subtract_first_point
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def callback_toggle_add_min(self):
		self.plot_token.add_min=not self.plot_token.add_min
		plot_save_oplot_file(self.config_file,self.plot_token)
		self.do_plot()

	def update(self):
		self.load_data(self.input_files,self.config_file)
		self.do_plot()

	def callback_refresh(self):
		self.update()

	def init(self):
		self.main_vbox = QVBoxLayout()

		self.config_file=""
		self.plot_token=None
		self.labels=[]
		self.fig = Figure(figsize=(2.5,2), dpi=100)
		self.plot_id=[]
		self.canvas = FigureCanvas(self.fig)  # a gtk.DrawingArea

		self.zero_frame_enable=False
		self.zero_frame_list=[]
		self.plot_title=""
		self.gen_colors(1)

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))


		self.tb_save = QAction(QIcon(os.path.join(get_image_file_path(),"save.png")), _("Save graph"), self)
		self.tb_save.triggered.connect(self.callback_plot_save)
		toolbar.addAction(self.tb_save)

		self.tb_refresh = QAction(QIcon(os.path.join(get_image_file_path(),"refresh.png")), _("Refresh graph"), self)
		self.tb_refresh .triggered.connect(self.callback_refresh)
		toolbar.addAction(self.tb_refresh )

		nav_bar=NavigationToolbar(self.canvas,self)
		toolbar.addWidget(nav_bar)


		

		self.fig.canvas.mpl_connect('motion_notify_event', self.mouse_move)

		menubar = QMenuBar()

		file_menu = menubar.addMenu("File")

		self.menu_save=file_menu.addAction(_("&Save"))
		self.menu_save.triggered.connect(self.callback_save)

		self.menu_save_as=file_menu.addAction(_("&Save as"))
		self.menu_save_as.triggered.connect(self.callback_plot_save)


		key_menu = menubar.addMenu('&Key')

		key_menu = menubar.addMenu('&Color')
		self.menu_black=key_menu.addAction(_("&Black"))
		self.menu_black.triggered.connect(self.callback_black)

		self.menu_rainbow=key_menu.addAction(_("&Rainbow"))
		self.menu_rainbow.triggered.connect(self.callback_rainbow)

		axis_menu = menubar.addMenu('&Color')
		menu=axis_menu.addAction(_("&Autoscale"))
		menu.triggered.connect(self.callback_autoscale_y)

		menu=axis_menu.addAction(_("&Set log scale y"))
		menu.triggered.connect(self.callback_toggle_log_scale_y)

		menu=axis_menu.addAction(_("&Set log scale x"))
		menu.triggered.connect(self.callback_toggle_log_scale_x)

		menu=axis_menu.addAction(_("&Set log scale x"))
		menu.triggered.connect(self.callback_toggle_log_scale_x)

		self.menu_rainbow=key_menu.addAction(_("&Label data"))
		self.menu_rainbow.triggered.connect(self.callback_toggle_label_data)

		math_menu = menubar.addMenu('&Math')

		menu=math_menu.addAction(_("&Subtract first point"))
		menu.triggered.connect(self.callback_toggle_subtract_first_point)

		menu=math_menu.addAction(_("&Add min point"))
		menu.triggered.connect(self.callback_toggle_add_min)

		menu=math_menu.addAction(_("&Invert y-axis"))
		menu.triggered.connect(self.callback_toggle_invert_y)
		
		menu=math_menu.addAction(_("&Norm to 1.0 y"))
		menu.triggered.connect(self.callback_normtoone_y)
		
		menu=math_menu.addAction(_("&Norm to peak of all data"))
		menu.triggered.connect(self.callback_norm_to_peak_of_all_data)
		
		menu=math_menu.addAction(_("&Heat map"))
		menu.triggered.connect(self.callback_set_heat_map)

		menu=math_menu.addAction(_("&Heat map edit"))
		menu.triggered.connect(self.callback_heat_map_edit)

		menu=math_menu.addAction(_("&xy plot"))
		menu.triggered.connect(self.callback_set_xy_plot)

		self.main_vbox.addWidget(menubar)
		
		self.main_vbox.addWidget(toolbar)

		#    ( "/_Key/No key",  None,         self.callback_key , 0, "<RadioItem>", "gtk-save" ),
		#    ( "/_Key/upper right",  None,         self.callback_key , 0, "<RadioItem>", "gtk-save" ),
		#   ( "/_Key/upper left",  None,         self.callback_key , 0, "<RadioItem>", "gtk-save" ),
		#    ( "/_Key/lower left",  None,         self.callback_key , 0, "<RadioItem>", "gtk-save" ),
		#    ( "/_Key/lower right",  None,         self.callback_key , 0, "<RadioItem>", "gtk-save" ),
		#    ( "/_Key/right",  None,         self.callback_key , 0, "<RadioItem>", "gtk-save" ),
		#    ( "/_Key/center right",  None,         self.callback_key , 0, "<RadioItem>", "gtk-save" ),
		#    ( "/_Key/lower center",  None,         self.callback_key , 0, "<RadioItem>", "gtk-save" ),
		#    ( "/_Key/upper center",  None,         self.callback_key , 0, "<RadioItem>", "gtk-save" ),
		#    ( "/_Key/center",  None,         self.callback_key , 0, "<RadioItem>", "gtk-save" ),
		#    ( "/_Key/Units",  None,         self.callback_units , 0, None ),
		




		self.canvas.figure.patch.set_facecolor('white')
		self.canvas.setMinimumSize(800, 350)
		self.main_vbox.addWidget(self.canvas)

		self.setLayout(self.main_vbox)
		#self.win.connect('key_press_event', self.on_key_press_event)
