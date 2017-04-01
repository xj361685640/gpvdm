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
from code_ctrl import enable_betafeatures
from scan_item import scan_item_add
from cal_path import get_image_file_path

#inp
from inp import inp_write_lines_to_file
from inp import inp_load_file

#qt
from PyQt5.QtWidgets import QMainWindow, QTextEdit, QAction, QApplication
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QSizePolicy,QHBoxLayout,QPushButton,QDialog,QFileDialog,QToolBar, QMessageBox, QVBoxLayout, QGroupBox, QTableWidget,QAbstractItemView, QTableWidgetItem
from PyQt5.QtCore import pyqtSignal

from mesh import mesh_get_xlayers
from mesh import mesh_get_ylayers
from mesh import mesh_get_zlayers
from mesh import mesh_get_xlist
from mesh import mesh_get_ylist
from mesh import mesh_get_zlist
from mesh import mesh_save_all
from mesh import mesh_add

from mesh import mesh_clear_xlist
from mesh import mesh_clear_ylist
from mesh import mesh_clear_zlist

class electrical_mesh_editor(QGroupBox):

	changed = pyqtSignal()
	
	def on_add_mesh_clicked(self):
		self.tab.blockSignals(True)
		index = self.tab.selectionModel().selectedRows()

		print(index)
		if len(index)>0:
			pos=index[0].row()+1
		else:
			pos = self.tab.rowCount()

		self.tab.insertRow(pos)

		self.tab.setItem(pos,0,QTableWidgetItem("100e-9"))

		self.tab.setItem(pos,1,QTableWidgetItem("20"))

		self.save()
		self.tab.blockSignals(False)
		self.changed.emit()

	def on_remove_click(self):
		self.tab.blockSignals(True)
		index = self.tab.selectionModel().selectedRows()

		print(index)
		if len(index)>0:
			pos=index[0].row()
			self.tab.removeRow(pos)

		self.save()
		self.tab.blockSignals(False)
		self.changed.emit()

	def save(self):
		if self.xyz=="y":
			mesh_clear_ylist()
		elif self.xyz=="x":
			print("cleared")
			mesh_clear_xlist()
		elif self.xyz=="z":
			mesh_clear_zlist()

		for i in range(0,self.tab.rowCount()):
			mesh_add(self.xyz,float(self.tab.item(i, 0).text()),float(self.tab.item(i, 1).text()))

		mesh_save_all()

	def update(self):
		self.load()

	def disable_dim(self):
		self.tab.setItem(0,1,QTableWidgetItem("1"))
		self.save()

	def enable_dim(self):
		if int(self.tab.rowCount())==1:
			self.tab.setItem(0,1,QTableWidgetItem("10"))
			self.save()


	def load(self):
		self.tab.blockSignals(True)
		self.tab.clear()
		self.tab.setHorizontalHeaderLabels([_("Thicknes"), _("Mesh points")])
		lines=[]
		pos=0
		
		if self.xyz=="y":
			mesh_layers=mesh_get_ylayers()
			layer_list=mesh_get_ylist()
		elif self.xyz=="x":
			mesh_layers=mesh_get_xlayers()
			layer_list=mesh_get_xlist()
		elif self.xyz=="z":
			mesh_layers=mesh_get_zlayers()
			layer_list=mesh_get_zlist()

		self.tab.setRowCount(mesh_layers)
		for i in range(0, mesh_layers):
			value = QTableWidgetItem(str(layer_list[i].thick))
			self.tab.setItem(i,0,value)

			value = QTableWidgetItem(str(int(layer_list[i].points)))
			self.tab.setItem(i,1,value)
		self.tab.blockSignals(False)

	def tab_changed(self, x,y):
		print(x,y)
		self.save()
		self.changed.emit()


	def __init__(self,xyz):
		self.xyz=xyz
		QGroupBox.__init__(self)
		self.setTitle(self.xyz)
		self.setStyleSheet("QGroupBox {  border: 1px solid gray;}")
		vbox=QVBoxLayout()
		self.setLayout(vbox)

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))

		add = QAction(QIcon(os.path.join(get_image_file_path(),"16_list-add.png")),  _("Add "+self.xyz+" mesh layer"), self)
		add.triggered.connect(self.on_add_mesh_clicked)
		toolbar.addAction(add)

		remove = QAction(QIcon(os.path.join(get_image_file_path(),"16_list-remove.png")),  _("Remove "+self.xyz+" mesh layer"), self)
		remove.triggered.connect(self.on_remove_click)
		toolbar.addAction(remove)

		vbox.addWidget(toolbar)

		self.tab = QTableWidget()

		self.tab.resizeColumnsToContents()

		self.tab.verticalHeader().setVisible(False)

		self.tab.clear()
		self.tab.setColumnCount(2)
		self.tab.setSelectionBehavior(QAbstractItemView.SelectRows)

		self.load()

		self.tab.cellChanged.connect(self.tab_changed)

		vbox.addWidget(self.tab)
