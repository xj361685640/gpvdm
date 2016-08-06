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
from scan_item import scan_item_add
from gui_util import dlg_get_text
import webbrowser
from util import time_with_units
from cal_path import get_image_file_path
from scan_item import scan_remove_file
from code_ctrl import enable_betafeatures
from tb_lasers import tb_lasers

#inp
from inp import inp_load_file
from inp import inp_read_next_item
from inp_util import inp_search_token_value
from inp import inp_get_token_value
from inp import inp_write_lines_to_file

#matplotlib
import matplotlib
matplotlib.use('Qt5Agg')
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QTableWidget,QAbstractItemView
from PyQt5.QtGui import QPainter,QIcon

import i18n
_ = i18n.language.gettext

(
SEG_LENGTH,
SEG_DT,
SEG_VOLTAGE_START,
SEG_VOLTAGE_STOP,
SEG_MUL,
SEG_SUN,
SEG_LASER
) = range(7)

mesh_articles = []


class tab_time_mesh(QWidget):
	lines=[]
	edit_list=[]

	line_number=[]
	save_file_name=""

	file_name=""
	name=""
	visible=1

	def save_data(self):
		file_name="time_mesh_config"+str(self.index)+".inp"
		scan_remove_file(file_name)

		out_text=[]
		out_text.append("#start_time")
		out_text.append(str(float(self.start_time)))
		out_text.append("#fs_laser_time")
		out_text.append(str(float(self.fs_laser_time)))
		out_text.append("#time_segments")
		out_text.append(str(int(len(self.store))))
		i=0
		for line in self.store:
			out_text.append("#time_segment"+str(i)+"_len")
			scan_item_add(file_name,out_text[len(out_text)-1],_("Part ")+str(i)+_(" period"),1)
			out_text.append(str(line[SEG_LENGTH]))

			out_text.append("#time_segment"+str(i)+"_dt")
			scan_item_add(file_name,out_text[len(out_text)-1],_("Part ")+str(i)+_(" dt"),1)
			out_text.append(str(line[SEG_DT]))

			out_text.append("#time_segment"+str(i)+"_voltage_start")
			scan_item_add(file_name,out_text[len(out_text)-1],_("Part ")+str(i)+_(" start voltage"),1)
			out_text.append(str(line[SEG_VOLTAGE_START]))

			out_text.append("#time_segment"+str(i)+"_voltage_stop")
			scan_item_add(file_name,out_text[len(out_text)-1],_("Part ")+str(i)+_(" stop voltage"),1)
			out_text.append(str(line[SEG_VOLTAGE_STOP]))

			out_text.append("#time_segment"+str(i)+"_mul")
			scan_item_add(file_name,out_text[len(out_text)-1],_("Part ")+str(i)+_(" mul"),1)
			out_text.append(str(line[SEG_MUL]))

			out_text.append("#time_segment"+str(i)+"_sun")
			scan_item_add(file_name,out_text[len(out_text)-1],_("Part ")+str(i)+_(" sun"),1)
			out_text.append(str(line[SEG_SUN]))

			out_text.append("#time_segment"+str(i)+"_laser")
			scan_item_add(file_name,out_text[len(out_text)-1],_("Part ")+str(i)+_(" laser"),1)
			out_text.append(str(line[SEG_LASER]))
			i=i+1

		out_text.append("#ver")
		out_text.append("1.1")
		out_text.append("#end")

		inp_write_lines_to_file(os.path.join(os.getcwd(),file_name),out_text)
		self.update_scan_tokens()

	def update_scan_tokens(self):
		file_name="time_mesh_config"+str(self.index)+".inp"
		scan_remove_file(file_name)

		for i in range(0,len(self.list)):
			scan_item_add(file_name,"#time_segment"+str(i)+"_len",_("Part ")+str(i)+_(" period"),1)
			scan_item_add(file_name,"#time_segment"+str(i)+"_dt",_("Part ")+str(i)+_(" dt"),1)
			scan_item_add(file_name,"#time_segment"+str(i)+"_voltage_start",_("Part ")+str(i)+_(" start voltage"),1)
			scan_item_add(file_name,"#time_segment"+str(i)+"_voltage_stop",_("Part ")+str(i)+_(" stop voltage"),1)
			scan_item_add(file_name,"#time_segment"+str(i)+"_mul",_("Part ")+str(i)+_(" mul"),1)
			scan_item_add(file_name,"#time_segment"+str(i)+"_sun",_("Part ")+str(i)+_(" Sun"),1)
			scan_item_add(file_name,"#time_segment"+str(i)+"_laser",_("Part ")+str(i)+_(" CW laser"),1)


	def callback_add_section(self, widget, treeview):
		data=["0.0", "0.0", "0.0", "0.0", "1.0", "0.0", "0.0"]
		selection = treeview.get_selection()
		model, iter = selection.get_selected()

		if iter:
			path = model.get_path(iter)[0]
			self.store.insert(path+1,data)
		else:
			self.store.append(data)

		self.build_mesh()
		self.draw_graph()
		self.fig.canvas.draw()
		self.save_data()

	def callback_remove_item(self, button, treeview):

		selection = treeview.get_selection()
		model, iter = selection.get_selected()

		if iter:
			#path = model.get_path(iter)[0]
			model.remove(iter)

		self.build_mesh()

		self.draw_graph()
		self.fig.canvas.draw()
		self.save_data()

	def callback_move_down(self, widget, treeview):

		selection = treeview.get_selection()
		model, iter = selection.get_selected()

		if iter:
			#path = model.get_path(iter)[0]
 			self.store.move_after( iter,self.store.iter_next(iter))

		self.build_mesh()
		self.draw_graph()
		self.fig.canvas.draw()
		self.save_data()

	def callback_start_time(self, widget, treeview):
		new_time=dlg_get_text( _("Enter the start time of the simulation"), str(self.start_time))

		if new_time!=None:
			self.start_time=float(new_time)
			self.build_mesh()
			self.draw_graph()
			self.fig.canvas.draw()
			self.save_data()


	def callback_laser(self, widget, treeview):
		new_time=dlg_get_text( _("Enter the time at which the laser pulse will fire (-1) to turn it off"), str(self.fs_laser_time))

		if new_time!=None:
			self.fs_laser_time=float(new_time)
			self.build_mesh()
			self.draw_graph()
			self.fig.canvas.draw()
			self.save_data()

	def on_cell_edited_length(self, cell, path, new_text, model):
		model[path][SEG_LENGTH] = new_text
		self.build_mesh()
		self.draw_graph()
		self.fig.canvas.draw()
		self.save_data()

	def update(self):
		self.build_mesh()
		self.draw_graph()
		self.fig.canvas.draw()

	def on_cell_edited(self, x,y):
		print "Cell edited",x,y
		return 
		model[path][SEG_DT] = new_text
		self.build_mesh()
		self.draw_graph()
		self.fig.canvas.draw()
		self.save_data()

	def gaussian(self,x, mu, sig):
		return exp(-power(x - mu, 2.) / (2 * power(sig, 2.)))

	def draw_graph(self):
		if (len(self.time)>0):
			mul,unit=time_with_units(float(self.time[len(self.time)-1]-self.time[0]))
		else:
			mul=1.0
			unit="s"

		time=[]
		for i in range(0,len(self.time)):
			time.append(self.time[i]*mul)
		self.fig.clf()
		self.fig.subplots_adjust(bottom=0.2)
		self.fig.subplots_adjust(left=0.1)
		self.ax1 = self.fig.add_subplot(111)
		self.ax1.ticklabel_format(useOffset=False)
		#ax2 = ax1.twinx()
		#x_pos=0.0
		#layer=0
		#color =['r','g','b','y','o','r','g','b','y','o']

		self.ax1.set_ylabel(_("Voltage (Volts)"))

		voltage, = self.ax1.plot(time,self.voltage, 'ro-', linewidth=3 ,alpha=1.0)
		self.ax1.set_xlabel(_("Time (")+unit+')')

		self.ax2 = self.ax1.twinx()
		self.ax2.set_ylabel(_("Magnitude (au)"))
		#ax2.set_ylabel('Energy (eV)')

		sun, = self.ax2.plot(time,self.sun, 'go-', linewidth=3 ,alpha=1.0)
		if enable_betafeatures()==True:
			laser, = self.ax2.plot(time,self.laser, 'bo-', linewidth=3 ,alpha=1.0)

		fs_laser_enabled=False
		if self.fs_laser_time!=-1:
			if len(self.time)>2:
				dt=(self.time[len(time)-1]-self.time[0])/100
				start=self.fs_laser_time-dt*5
				stop=self.fs_laser_time+dt*5
				x = linspace(start,stop,100)
				y=self.gaussian(x,self.fs_laser_time,dt)
				#print y

				fs_laser, = self.ax2.plot(x*mul,y, 'g-', linewidth=3 ,alpha=1.0)
				fs_laser_enabled=True
				self.ax2.ticklabel_format(style='sci', axis='x', scilimits=(0,0))

		if enable_betafeatures()==True:
			if fs_laser_enabled==True:
				self.fig.legend((voltage, sun, laser,fs_laser), (_("Voltage"), _("Sun"), _("CW laser"), _("fs laser")), 'upper right')
			else:
				self.fig.legend((voltage, sun, laser), (_("Voltage"), _("Sun"), _("CW laser")), 'upper right')
		else:
			if fs_laser_enabled==True:
				self.fig.legend((voltage, sun, fs_laser), (_("Voltage"), _("Sun"), _("fs laser")), 'upper right')
			else:
				self.fig.legend((voltage, sun), (_("Voltage"), _("Sun")), 'upper right')



	def save_image(self,file_name):
		self.fig.savefig(file_name)

	def callback_save(self, widget, data=None):
		dialog = gtk.FileChooserDialog(_("Save as.."),
                               None,
                               gtk.FILE_CHOOSER_ACTION_SAVE,
                               (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                                gtk.STOCK_SAVE, gtk.RESPONSE_OK))
		dialog.set_default_response(gtk.RESPONSE_OK)

		filter = gtk.FileFilter()
		filter.set_name(".jpg")
		filter.add_pattern("*.jpg")
		dialog.add_filter(filter)

		response = dialog.run()
		if response == gtk.RESPONSE_OK:
			file_name=dialog.get_filename()

			if os.path.splitext(file_name)[1]:
				self.save_image(file_name)
			else:
				filter=dialog.get_filter()
				self.save_image(file_name+filter.get_name())

		elif response == gtk.RESPONSE_CANCEL:
		    print _("Closed, no files selected")
		dialog.destroy()

	def create_model(self):
		store = gtk.ListStore(str, str, str, str, str, str, str)

		for line in self.list:
			store.append([str(line[SEG_LENGTH]), str(line[SEG_DT]), str(line[SEG_VOLTAGE_START]), str(line[SEG_VOLTAGE_STOP]), str(line[SEG_MUL]), str(line[SEG_SUN]), str(line[SEG_LASER])])

		return store

	def create_columns(self):
		self.tab.clear()
		self.tab.setColumnCount(7)
		self.tab.setSelectionBehavior(QAbstractItemView.SelectRows)
		self.tab.setHorizontalHeaderLabels([_("Length"), _("dt"), _("Start Voltage"), _("Stop Voltage"), _("Multiply"),_("Sun"),_("CW Laser")])


	def load_data(self):

		lines=[]
		self.start_time=0.0
		self.fs_laser_time=0.0
		self.list=[]

		file_name="time_mesh_config"+str(self.index)+".inp"
		print "loading",file_name
		ret=inp_load_file(lines,file_name)
		if ret==True:
			if inp_search_token_value(lines, "#ver")=="1.1":
				pos=0
				token,value,pos=inp_read_next_item(lines,pos)
				self.start_time=float(value)

				token,value,pos=inp_read_next_item(lines,pos)
				self.fs_laser_time=float(value)

				token,value,pos=inp_read_next_item(lines,pos)
				segments=int(value)

				for i in range(0, segments):
					token,length,pos=inp_read_next_item(lines,pos)
					token,dt,pos=inp_read_next_item(lines,pos)
					token,voltage_start,pos=inp_read_next_item(lines,pos)
					token,voltage_stop,pos=inp_read_next_item(lines,pos)
					token,mul,pos=inp_read_next_item(lines,pos)
					token,sun,pos=inp_read_next_item(lines,pos)
					token,laser,pos=inp_read_next_item(lines,pos)
					self.list.append((length,dt,voltage_start,voltage_stop,mul,sun,laser))

				print self.list
				return True
			else:
				print "file "+file_name+"wrong version"
				exit("")
				return False
		else:
			print "file "+file_name+" not found"
			return False

		return False

	def build_mesh(self):
		self.laser=[]
		self.sun=[]
		self.voltage=[]
		self.time=[]
		self.fs_laser=[]
		pos=self.start_time
		fired=False

		laser_pulse_width=0.0


		sun_steady_state=float(inp_get_token_value("light.inp", "#Psun"))

		voltage_bias=float(inp_get_token_value("pulse"+str(self.index)+".inp", "#pulse_bias"))


		seg=0
		for line in self.store:
			end_time=pos+float(line[SEG_LENGTH])
			dt=float(line[SEG_DT])
			voltage_start=float(line[SEG_VOLTAGE_START])
			voltage_stop=float(line[SEG_VOLTAGE_STOP])
			mul=float(line[SEG_MUL])
			sun=float(line[SEG_SUN])
			laser=float(line[SEG_LASER])
			#print "VOLTAGE=",line[SEG_VOLTAGE],end_time,pos

			if dt!=0.0 and mul!=0.0:
				voltage=voltage_start
				while(pos<end_time):
					dv=(voltage_stop-voltage_start)*(dt/float(line[SEG_LENGTH]))
					self.time.append(pos)
					self.laser.append(laser)
					self.sun.append(sun+sun_steady_state)
					self.voltage.append(voltage+voltage_bias)
					#print seg,voltage
					self.fs_laser.append(0.0)
					pos=pos+dt
					voltage=voltage+dv

					if fired==False:
						if pos>self.fs_laser_time and self.fs_laser_time!=-1:
							fired=True
							self.fs_laser[len(self.fs_laser)-1]=laser_pulse_width/dt

					dt=dt*mul

			seg=seg+1

		#print self.voltage

		self.statusbar.push(0, str(len(self.time))+_(" mesh points"))


	def __init__(self,index):
		self.index=index
		print "index=",index

		QWidget.__init__(self)
		self.main_vbox = QVBoxLayout()
		self.time=[]
		self.voltage=[]
		self.sun=[]
		self.laser=[]

		self.edit_list=[]
		self.line_number=[]
		self.list=[]


		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))

		self.tb_save = QAction(QIcon(os.path.join(get_image_file_path(),"32_save.png")), _("Save image"), self)
		self.tb_save.triggered.connect(self.callback_save)
		toolbar.addAction(self.tb_save)

		self.tb_save = QAction(QIcon(os.path.join(get_image_file_path(),"laser.png")), _("Laser start time"), self)
		self.tb_save.triggered.connect(self.callback_save)
		toolbar.addAction(self.tb_save)


		self.lasers=tb_lasers("pulse"+str(self.index)+".inp")
		toolbar.addWidget(self.lasers)

		self.tb_start = QAction(QIcon(os.path.join(get_image_file_path(),"start.png")), _("Simulation start time"), self)
		self.tb_start.triggered.connect(self.callback_start_time)
		toolbar.addAction(self.tb_start)

		self.main_vbox.addWidget(toolbar)


		gui_pos=0



		self.fig = Figure(figsize=(5,4), dpi=100)
		self.canvas = FigureCanvas(self.fig)  # a gtk.DrawingArea
		#self.canvas.figure.patch.set_facecolor('white')
		#canvas.set_size_request(500, 150)

		self.ax1=None
		self.show_key=True

		self.load_data()
		self.update_scan_tokens()

		#self.build_mesh()
		self.draw_graph()

		self.main_vbox.addWidget(self.canvas)

		self.canvas.show()

		#toolbar 2

		toolbar2=QToolBar()
		toolbar2.setIconSize(QSize(48, 48))

		self.tb_add = QAction(QIcon(os.path.join(get_image_file_path(),"add.png")), _("Add section"), self)
		self.tb_add.triggered.connect(self.callback_add_section)
		toolbar2.addAction(self.tb_add)

		self.tb_remove = QAction(QIcon(os.path.join(get_image_file_path(),"minus.png")), _("Delete section"), self)
		self.tb_remove.triggered.connect(self.callback_remove_item)
		toolbar2.addAction(self.tb_remove)

		self.tb_move = QAction(QIcon(os.path.join(get_image_file_path(),"down.png")), _("Move down"), self)
		self.tb_move.triggered.connect(self.callback_move_down)
		toolbar2.addAction(self.tb_move)

		self.main_vbox.addWidget(toolbar2)



		self.tab = QTableWidget()
		self.tab.resizeColumnsToContents()

		self.tab.verticalHeader().setVisible(False)

		self.create_columns()

		self.tab.cellChanged.connect(self.on_cell_edited)

		self.main_vbox.addWidget(self.tab)


		self.setLayout(self.main_vbox)

		return




