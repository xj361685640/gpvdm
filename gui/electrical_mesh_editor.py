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

class electrical_mesh_editor(QGroupBox):

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

	def on_remove_click(self, button, treeview):
		self.tab.blockSignals(True)
		index = self.tab.selectionModel().selectedRows()

		print(index)
		if len(index)>0:
			pos=index[0].row()
			self.tab.removeRow(pos)

		self.save()
		self.tab.blockSignals(False)

	def save(self):
		lines=[]
		lines.append("#mesh_layers")
		lines.append(str(self.tab.rowCount()))
		i=0
		for i in range(0,self.tab.rowCount()):
			print(i)
			lines.append("#mesh_layer_length"+str(i))
			lines.append(str(self.tab.item(i, 0).text()))
			lines.append("#mesh_layer_points"+str(i))
			lines.append(str(self.tab.item(i, 1).text()))

		lines.append("#ver")
		lines.append("1.0")
		lines.append("#end")

		inp_write_lines_to_file(os.path.join(os.getcwd(),"mesh_"+self.xyz+".inp"),lines)

	def refresh(self):
		self.load()

	def disable_dim(self):
		self.tab.setItem(0,1,QTableWidgetItem("1"))
		self.mesh_points=1
		self.save()

	def enable_dim(self):
		if int(self.tab.rowCount())==1:
			self.tab.setItem(0,1,QTableWidgetItem("10"))
			self.mesh_points=10
			self.save()


	def load(self):
		self.tab.clear()
		self.mesh_points=0
		self.tab.clear()
		lines=[]
		pos=0
		if inp_load_file(lines,os.path.join(os.getcwd(),"mesh_"+self.xyz+".inp"))==True:
			pos=pos+1	#first comment
			mesh_layers=int(lines[pos])
			self.tab.setRowCount(mesh_layers)

			for i in range(0, mesh_layers):
				pos=pos+1					#token
				token=lines[pos]
				scan_item_add("mesh_"+self.xyz+".inp",token,self.xyz+"mesh width"+str(i),1)
				pos=pos+1
				thicknes=lines[pos]	#read value

				pos=pos+1					#token
				token=lines[pos]
				scan_item_add("mesh_"+self.xyz+".inp",token,self.xyz+"mesh points"+str(i),1)

				pos=pos+1
				points=lines[pos] 		#read value

				value = QTableWidgetItem(str(thicknes))
				self.tab.setItem(i,0,value)

				value = QTableWidgetItem(str(points))
				self.tab.setItem(i,1,value)

				self.mesh_points=self.mesh_points+int(points)

	def tab_changed(self, x,y):
		print(x,y)
		self.save()
		#self.refresh(True)


	def __init__(self,xyz):
		self.xyz=xyz
		self.mesh_points=0
		QGroupBox.__init__(self)
		self.setTitle(self.xyz)
		self.setStyleSheet("QGroupBox {  border: 1px solid gray;}")
		vbox=QVBoxLayout()
		self.setLayout(vbox)

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))

		add = QAction(QIcon(os.path.join(get_image_file_path(),"16_add.png")),  _("Add "+self.xyz+" mesh layer"), self)
		add.triggered.connect(self.on_add_mesh_clicked)
		toolbar.addAction(add)

		remove = QAction(QIcon(os.path.join(get_image_file_path(),"16_minus.png")),  _("Remove "+self.xyz+" mesh layer"), self)
		remove.triggered.connect(self.on_remove_click)
		toolbar.addAction(remove)

		vbox.addWidget(toolbar)

		self.tab = QTableWidget()
		self.tab.resizeColumnsToContents()

		self.tab.verticalHeader().setVisible(False)

		self.tab.clear()
		self.tab.setColumnCount(2)
		self.tab.setSelectionBehavior(QAbstractItemView.SelectRows)
		self.tab.setHorizontalHeaderLabels([_("Thicknes"), _("Mesh points")])

		self.load()

		self.tab.cellChanged.connect(self.tab_changed)

		vbox.addWidget(self.tab)


		return




#gobject.type_register(electrical_mesh_editor)
#gobject.signal_new("refresh", electrical_mesh_editor, gobject.SIGNAL_RUN_FIRST,gobject.TYPE_NONE, ())
