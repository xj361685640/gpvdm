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


import os
from cal_path import get_image_file_path
from search import find_fit_log
from search import find_fit_speed_log
from window_list import windows
from inp import inp_load_file
from inp_util import inp_search_token_value
from status_icon import status_icon_stop
from jobs import jobs_view

#qt
from PyQt5.QtWidgets import QMainWindow, QTextEdit, QAction, QApplication
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QSizePolicy,QHBoxLayout,QPushButton,QDialog,QFileDialog,QToolBar,QVBoxLayout,QTabWidget,QLabel,QSlider,QWidgetItem
from about import about_dlg
from gui_util import error_dlg

from progress import progress_class
from server import server_get

class node_indicator(QWidget):
	def __init__(self):
		QWidget.__init__(self)
		self.hbox=QHBoxLayout()
		
		self.bar=progress_class()
		self.bar.spinner.stop()
		self.bar.spinner.hide()
		self.label=QLabel()
		
		self.slider = QSlider(Qt.Horizontal)


		self.slider.setTickPosition(QSlider.TicksBelow)
		self.slider.setTickInterval(1)
		#self.slider.valueChanged.connect(self.slider0_change)
		self.slider.setMinimumSize(300, 80)
		self.slider.valueChanged.connect(self.slider_changed)
		self.slider.setTickPosition(QSlider.TicksBelow)
		self.hbox.addWidget(self.label)
		self.hbox.addWidget(self.bar)
		self.hbox.addWidget(self.slider)

		self.setLayout(self.hbox)

		self.name=""
		self.ip=""
		self.cpus=-1
		self.load=-1
		self.max_cpus=-1
		self.last_seen=-1

	def block_signals(self,val):
		self.slider.blockSignals(val)
		
	def slider_changed(self):
		self.max_cpus=self.slider.value()
		self.update()
		
	def set_cpus(self,cpus):
		self.cpus=cpus
		self.slider.setMinimum(0)
		self.slider.setMaximum(cpus)

	def set_text(self,text):
		self.label.setText(text)

	def set_name(self,name):
		self.name=name		

	def set_ip(self,ip):
		self.ip=ip		

	def set_load(self,load):
		self.load=load

	def set_max_cpus(self,max_cpus):
		self.slider.setValue(max_cpus)
		self.max_cpus=max_cpus

	def set_last_seen(self,last_seen):
		self.last_seen=last_seen
		
	def update(self):
		self.set_text(self.name)

		self.bar.set_text(self.ip+" "+str(self.load)+":"+str(self.max_cpus)+"/"+str(self.cpus)+":seen="+self.last_seen)

		if float(self.cpus)!=0.0:
			prog=float(self.load)/float(self.cpus)
		else:
			prog=0.0

		print("setting CPUs",self.load,self.cpus,prog)
		
		if prog>1.0:
			prog=1.0

		self.bar.set_fraction(prog)

class hpc_class(QWidget):

	name=""
	cpus=[]


	def slider_changed(self, widget):
		ip=[]
		loads=[]

		for i in range(0, self.node_view_vbox.count()):
			item=self.node_view_vbox.itemAt(i).widget()
			ip.append(item.ip)
			loads.append(item.max_cpus)
		self.myserver.set_cluster_loads(ip,loads)

	def callback_cluster_get_info(self):
		self.myserver.cluster_get_info()

		if len(self.myserver.nodes)>self.node_view_vbox.count():
			needed=len(self.myserver.nodes)-self.node_view_vbox.count()
			for i in range(0,needed):
				self.node_widget=node_indicator()
				self.node_widget.show()
				self.node_widget.slider.valueChanged.connect(self.slider_changed)
				self.node_view_vbox.addWidget(self.node_widget)
				print("Add widget",i)

		if self.node_view_vbox.count()>len(self.myserver.nodes):
			for i in range(len(self.myserver.nodes), len(self.button)):
				item=self.node_view_vbox.itemAt(i).widget()
				item.deleteLater()

		for i in range(0, self.node_view_vbox.count()):
			item=self.node_view_vbox.itemAt(i).widget()
			item.block_signals(True)
			item.set_name(self.myserver.nodes[i][0])
			item.set_ip(self.myserver.nodes[i][1])
			item.set_cpus(int(self.myserver.nodes[i][2]))
			item.set_load(self.myserver.nodes[i][4])
			item.set_max_cpus(int(self.myserver.nodes[i][5]))
			item.set_last_seen(self.myserver.nodes[i][6])

			item.update()
			item.block_signals(False)

	def closeEvent(self, event):
		self.win_list.update(self,"hpc_window")
		self.hide()
		event.accept()


	def __init__(self):
		QWidget.__init__(self)

		self.setMinimumSize(900, 600)
		self.setWindowIcon(QIcon(os.path.join(get_image_file_path(),"connected.png")))
		self.setWindowTitle(_("Cluster status (www.gpvdm.com)")) 

		self.myserver=server_get()
		self.win_list=windows()
		self.win_list.load()
		self.win_list.set_window(self,"hpc_window")

		self.node_view=QWidget()
		self.node_view_vbox=QVBoxLayout()
		self.node_view.setLayout(self.node_view_vbox)

		
		self.main_vbox = QVBoxLayout()

		self.tool_bar=QToolBar()

		self.tool_bar.setIconSize(QSize(42, 42))
		
		self.main_vbox.addWidget(self.tool_bar)
		
		self.notebook = QTabWidget()

		self.notebook.setMovable(True)

		self.main_vbox.addWidget(self.notebook)

		self.notebook.addTab(self.node_view,"Nodes")
		self.node_view.show()
		
		self.jview=jobs_view()
		self.jview.load_data(self.myserver.cluster_jobs)
		self.notebook.addTab(self.jview,"Jobs list")

		self.setLayout(self.main_vbox)

		self.win_list.set_window(self,"hpc_window")
		





