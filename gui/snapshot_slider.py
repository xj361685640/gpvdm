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
from tab import tab_class
from window_list import windows
from cal_path import get_image_file_path

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QSlider,QHBoxLayout,QLabel
from PyQt5.QtGui import QPainter,QIcon
from PyQt5.QtCore import pyqtSignal

from help import help_window


class snapshot_slider(QWidget):

	changed = pyqtSignal()

	def cal_min_max(self, data, widget):

		#path0=self.entry0.get_active_text()
		my_max=-1e40
		my_min=1e40
		#print "Rod",self.file_names
		for ii in range(0,len(self.file_names)):
			for i in range(0,self.dumps):
				self.update(i)
				t=[]
				s=[]
				z=[]
				if self.plot.read_data_file(t,s,z,ii) == True:
					temp_max=max(s)
					temp_min=min(s)

					if temp_max>my_max:
						my_max=temp_max

					if temp_min<my_min:
						my_min=temp_min

		self.plot.ymax=my_max
		self.plot.ymin=my_min
		
	def update(self):
		self.dirs=[]
		if os.path.isdir(self.path)==True:
			for name in os.listdir(self.path):
				if name!="." and name!= "..":
					full_path=os.path.join(self.path, name)
					if os.path.isdir(full_path):
						self.dirs.append(full_path)

		self.slider0.setMaximum(len(self.dirs))		

	def slider0_change(self):
		value = self.slider0.value()
		self.label0.setText(str(value))
		self.file_name=os.path.join(self.path,str(value),"Jn.dat")
		self.changed.emit()
		
	def __init__(self,path):
		QWidget.__init__(self)
		self.path=path
		self.file_name=""
		
		self.setWindowTitle(_("Snapshot slider")) 
		
		#self.main_vbox = QVBoxLayout()

		self.slider_hbox0= QHBoxLayout()

		self.slider0 = QSlider(Qt.Horizontal)
		self.slider0.setMinimum(10)
		self.slider0.setMaximum(30)
		self.slider0.setTickPosition(QSlider.TicksBelow)
		self.slider0.setTickInterval(5)
		self.slider0.valueChanged.connect(self.slider0_change)
		self.slider0.setMinimumSize(300, 80)

		self.slider_hbox0.addWidget(self.slider0)

		self.label0 = QLabel()
		self.label0.setText("")

		self.slider0.setValue(20)

		self.slider_hbox0.addWidget(self.label0)

		self.update()
		self.setLayout(self.slider_hbox0)



