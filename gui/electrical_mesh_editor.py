#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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
from code_ctrl import enable_betafeatures
from scan_item import scan_item_add
from icon_lib import icon_get

#qt
from PyQt5.QtWidgets import QMainWindow, QTextEdit, QAction, QApplication
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QSizePolicy,QHBoxLayout,QPushButton,QDialog,QFileDialog,QToolBar, QMessageBox, QVBoxLayout, QGroupBox, QTableWidget,QAbstractItemView, QTableWidgetItem
from PyQt5.QtCore import pyqtSignal

#matplotlib
import matplotlib
matplotlib.use('Qt5Agg')
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar
import matplotlib.cm as cm
from matplotlib import rcParams

from mesh import mesh_get_xlayers
from mesh import mesh_get_ylayers
from mesh import mesh_get_zlayers
from mesh import mesh_get_xmesh
from mesh import mesh_get_ymesh
from mesh import mesh_get_zmesh
from mesh import mesh_save_all
from mesh import mesh_add

from mesh import mesh_clear_xlist
from mesh import mesh_clear_ylist
from mesh import mesh_clear_zlist
from mesh import mesh_save_y
from mesh import mesh_save_x
from mesh import mesh_save_z

import numpy as np

from util import distance_with_units

from leftright import leftright
from util import str2bool
from gui_util import tab_get_value

class electrical_mesh_editor(QGroupBox):

	changed = pyqtSignal()

	def insert_row(self,pos,thick,points,mul,left_right):

		item=QTableWidgetItem(str(thick))
		self.tab.setItem(pos,0,item)

		item=QTableWidgetItem(str(points))
		self.tab.setItem(pos,1,item)

		item=QTableWidgetItem(str(mul))
		self.tab.setItem(pos,2,item)

		self.item = leftright()
		self.item.set_value(str2bool(left_right))
		self.item.changed.connect(self.tab_changed)
		self.tab.setCellWidget(pos,3,self.item)
	
	def on_add_mesh_clicked(self):
		self.tab.blockSignals(True)
		index = self.tab.selectionModel().selectedRows()

		if len(index)>0:
			pos=index[0].row()+1
		else:
			pos = self.tab.rowCount()

		self.tab.insertRow(pos)
		self.insert_row(pos,100e-9,20,1.1,"left")

		#self.tab.setItem(pos,3,QTableWidgetItem("left"))

		self.save()
		self.redraw()
		self.tab.blockSignals(False)
		self.changed.emit()

	def on_remove_click(self):
		self.tab.blockSignals(True)
		index = self.tab.selectionModel().selectedRows()

		if len(index)>0:
			pos=index[0].row()
			self.tab.removeRow(pos)

		self.save()
		self.redraw()
		self.tab.blockSignals(False)
		self.changed.emit()

	def save(self):
		if self.xyz=="y":
			mesh_clear_ylist()
		elif self.xyz=="x":
			mesh_clear_xlist()
		elif self.xyz=="z":
			mesh_clear_zlist()

		for i in range(0,self.tab.rowCount()):
			mesh_add(self.xyz,float(self.tab.item(i, 0).text()),float(self.tab.item(i, 1).text()),float(self.tab.item(i, 2).text()),tab_get_value(self.tab,i, 3))
		
		if self.xyz=="y":
			mesh_save_y()
		elif self.xyz=="x":
			mesh_save_x()
		elif self.xyz=="z":
			mesh_save_z()


	def redraw(self):
		self.ax1.clear()
		self.x=[]
		self.mag=[]
		pos=0
		ii=0
		tot=0.0
		for i in range(0,self.tab.rowCount()):
			tot=tot+float(self.tab.item(i, 0).text())
			
		mul,unit=distance_with_units(tot)
		total_pos=0.0
		for i in range(0,self.tab.rowCount()):
			layer_length=float(self.tab.item(i, 0).text())
			layer_points=float(self.tab.item(i, 1).text())
			layer_mul=float(self.tab.item(i, 2).text())
			layer_left_right=tab_get_value(self.tab,i, 3)
			dx=layer_length/layer_points
			pos=dx/2
			ii=0
			temp_x=[]
			temp_mag=[]
			while(pos<layer_length):
				temp_x.append(pos)
				temp_mag.append(1.0)

				pos=pos+dx*pow(layer_mul,ii)

				ii=ii+1
				#dx=dx*layer_mul
			for i in range(0,len(temp_x)):
				if layer_left_right=="left":
					self.x.append((temp_x[i]+total_pos)*mul)
				else:
					self.x.append((layer_length-temp_x[i]+total_pos)*mul)

				self.mag.append(temp_mag[i])
			total_pos=total_pos+layer_length

		c = np.linspace(0, 10, len(self.x))
		
		self.ax1.set_ylabel(_("Magnitude"))
		cmap = cm.jet
		self.ax1.clear()
		self.ax1.scatter(self.x,self.mag ,c=c, cmap=cmap)
		self.fig.canvas.draw()
		self.ax1.set_xlabel(_("Thickness")+" ("+unit+")")
		self.ax1.get_yaxis().set_visible(False)
		self.ax1.spines['top'].set_visible(False)
		self.ax1.spines['right'].set_visible(False)
		#self.ax1.spines['bottom'].set_visible(False)
		self.ax1.spines['left'].set_visible(False)

	def update(self):
		self.load()

	def disable_dim(self):
		self.tab.setItem(0,1,QTableWidgetItem("1"))
		self.redraw()
		self.save()

	def enable_dim(self):
		if int(self.tab.rowCount())==1:
			self.tab.setItem(0,1,QTableWidgetItem("10"))
			self.redraw()
			self.save()


	def load(self):
		self.tab.blockSignals(True)
		self.tab.clear()
		self.tab.setHorizontalHeaderLabels([_("Thicknes"), _("Mesh points"), _("Step multiply"), _("Left/Right")])
		lines=[]
		pos=0

		if self.xyz=="y":
			mesh_layers=mesh_get_ylayers()
			layer_list=mesh_get_ymesh().layers
		elif self.xyz=="x":
			mesh_layers=mesh_get_xlayers()
			layer_list=mesh_get_xmesh().layers
		elif self.xyz=="z":
			mesh_layers=mesh_get_zlayers()
			layer_list=mesh_get_zmesh().layers

		self.tab.setRowCount(mesh_layers)
		for i in range(0, mesh_layers):
			self.insert_row(i,layer_list[i].thick,layer_list[i].points,layer_list[i].mul,layer_list[i].left_right)

		self.redraw()
		self.tab.blockSignals(False)
		
	def tab_changed(self):
		self.save()
		self.redraw()
		self.changed.emit()


	def __init__(self,xyz):
		self.xyz=xyz
		QGroupBox.__init__(self)
		rcParams.update({'figure.autolayout': True})
		self.setTitle(self.xyz)
		self.setStyleSheet("QGroupBox {  border: 1px solid gray;}")
		vbox=QVBoxLayout()
		self.setLayout(vbox)

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(32, 32))

		add = QAction(icon_get("list-add",size=32),  _("Add "+self.xyz+" mesh layer"), self)
		add.triggered.connect(self.on_add_mesh_clicked)
		toolbar.addAction(add)

		remove = QAction(icon_get("list-remove",size=32),  _("Remove "+self.xyz+" mesh layer"), self)
		remove.triggered.connect(self.on_remove_click)
		toolbar.addAction(remove)

		vbox.addWidget(toolbar)

		self.tab = QTableWidget()

		self.tab.resizeColumnsToContents()

		self.tab.verticalHeader().setVisible(False)

		self.tab.clear()
		self.tab.setColumnCount(4)
		self.tab.setSelectionBehavior(QAbstractItemView.SelectRows)

		self.tab.cellChanged.connect(self.tab_changed)

		vbox.addWidget(self.tab)

		self.fig = Figure(figsize=(5,2), dpi=100)
		self.canvas = FigureCanvas(self.fig)
		self.canvas.figure.patch.set_facecolor('white')
		
		self.fig.subplots_adjust(bottom=0.2)
		self.fig.subplots_adjust(left=0.1)
		self.ax1 = self.fig.add_subplot(111)
		self.ax1.ticklabel_format(useOffset=False)

		vbox.addWidget(self.canvas)

		self.load()
