#!/usr/bin/env python2.7
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
#from optics import find_materials
#from inp import inp_write_lines_to_file
from util import str2bool
from inp_util import inp_search_token_value
from scan_item import scan_item_add
from scan_item import scan_remove_file
from cal_path import get_image_file_path
from plot_gen import plot_gen
from gpvdm_open import gpvdm_open
from cal_path import get_materials_path
from global_objects import global_object_get
from help import help_window
#from doping import doping_window

#inp
from inp import inp_isfile
from inp import inp_copy_file
from inp import inp_update_token_value
from inp import inp_load_file


#epitaxy
from epitaxy import epitaxy_get_pl_file
from epitaxy import epitay_get_next_pl
from epitaxy import epitaxy_get_name
from epitaxy import epitaxy_get_width
from epitaxy import epitaxy_get_mat_file
from epitaxy import epitaxy_get_electrical_layer
from epitaxy import epitaxy_get_layers
from epitaxy import epitaxy_save
from epitaxy import epitaxy_load_from_arrays
from epitaxy import epitay_get_next_dos

#windows
from contacts import contacts_window
from emesh import tab_electrical_mesh

#qt
from PyQt5.QtCore import QSize, Qt
from PyQt5.QtGui import QIcon
from PyQt5.QtWidgets import QWidget, QVBoxLayout,QProgressBar,QLabel,QDesktopWidget,QToolBar,QHBoxLayout,QAction, QSizePolicy, QTableWidget, QTableWidgetItem,QComboBox,QDialog,QAbstractItemView

from PyQt5.QtGui import QPixmap

import i18n
_ = i18n.language.gettext

from i18n import yes_no

(
  COLUMN_NAME,
  COLUMN_THICKNES,
  COLUMN_MATERIAL,
  COLUMN_DEVICE,
  COLUMN_DOS_LAYER,
  COLUMN_PL_FILE
) = range(6)

from PyQt5.QtWidgets import QWidget

class layer_widget(QWidget):

	def tab_changed(self, x,y):
		print x,y
		self.save_model()
		#self.refresh(True)

	def sync_to_electrical_mesh(self):
		tot=0
		for i in range(0,len(self.model)):
			if yes_no(self.model[i][COLUMN_DEVICE])==True:
				tot=tot+float(self.model[i][COLUMN_THICKNES])

		lines=[]
		if inp_load_file(lines,os.path.join(os.getcwd(),"mesh_y.inp"))==True:
			mesh_layers=int(inp_search_token_value(lines, "#mesh_layers"))
			if mesh_layers==1:
				inp_update_token_value(os.path.join(os.getcwd(),"mesh_y.inp"), "#mesh_layer_length0", str(tot),1)

	def layer_type_edit(self, widget, path, text, model):
		old_text=self.model[path][COLUMN_DEVICE]
		self.model[path][COLUMN_DEVICE]=text
#		print yes_no(old_text), yes_no(text),type(yes_no(text))
		if old_text!="Active layer" and text=="Active layer":
			print "doing update"
			self.model[path][COLUMN_DOS_LAYER]=epitay_get_next_dos()
			self.model[path][COLUMN_PL_FILE]=epitay_get_next_pl()
			new_file=self.model[path][COLUMN_DOS_LAYER]+".inp"
			if inp_isfile(new_file)==False:
				inp_copy_file(new_file,"dos0.inp")

			new_file=self.model[path][COLUMN_PL_FILE]+".inp"
			if inp_isfile(new_file)==False:
				inp_copy_file(new_file,"pl0.inp")
		else:
			self.model[path][COLUMN_DOS_LAYER]=text
			self.model[path][COLUMN_PL_FILE]="none"

		self.save_model()
		self.refresh(True)



	def rebuild_mat_list(self):
		self.material_files.clear()
		self.layer_type.clear()
		mat=find_materials()
		print mat
		for i in range(0,len(mat)):
			self.material_files.append([mat[i]])
			scan_remove_file(os.path.join(get_materials_path(),mat[i]))			
			scan_item_add(os.path.join("materials",mat[i],"fit.inp"),"#wavelength_shift_alpha","Absorption spectrum wavelength shift",1)
			scan_item_add(os.path.join("materials",mat[i],"fit.inp"),"#n_mul","Refractive index spectrum multiplier",1)
			scan_item_add(os.path.join("materials",mat[i],"fit.inp"),"#alpha_mul","Absorption spectrum multiplier",1)

		self.layer_type.append([_("Active layer")])
		self.layer_type.append([_("Contact")])
		self.layer_type.append([_("Other")])


	def callback_view_materials(self):
		dialog=gpvdm_open(get_materials_path())
		dialog.show_inp_files=False
		ret=dialog.window.exec_()

		if ret==QDialog.Accepted:
			plot_gen([dialog.get_filename()],[],"auto")

	def callback_move_down(self, widget, data=None):

		selection = self.treeview.get_selection()
		model, iter = selection.get_selected()

		if iter:
			#path = model.get_path(iter)[0]
 			model.move_after( iter,model.iter_next(iter))
			self.save_model()
			self.refresh(True)

	def callback_edit_mesh(self, widget, data=None):
		help_window().help_set_help(["mesh.png",_("<big><b>Mesh editor</b></big>\nUse this window to setup the mesh, the window can also be used to change the dimensionality of the simulation.")])

		if self.electrical_mesh.isVisible()==True:
			self.electrical_mesh.hide()
		else:
			self.electrical_mesh.show()

	def __init__(self):
		QWidget.__init__(self)
		self.doping_window=False
		self.contacts_window=False

		self.main_vbox=QVBoxLayout()

		self.toolbar=QToolBar()
		self.toolbar.setIconSize(QSize(32, 32))

		self.tb_add = QAction(QIcon(os.path.join(get_image_file_path(),"add.png")), _("Add device layer"), self)
		self.tb_add.triggered.connect(self.on_add_item_clicked)
		self.toolbar.addAction(self.tb_add)

		self.tb_remove = QAction(QIcon(os.path.join(get_image_file_path(),"minus.png")), _("Delete device layer"), self)
		self.tb_remove.triggered.connect(self.on_remove_item_clicked)
		self.toolbar.addAction(self.tb_remove)


		self.tb_remove= QAction(QIcon(os.path.join(get_image_file_path(),"down.png")), _("Move device layer"), self)
		self.tb_remove.triggered.connect(self.on_remove_item_clicked)
		self.toolbar.addAction(self.tb_remove)

		self.tb_mesh = QAction(QIcon(os.path.join(get_image_file_path(),"mesh.png")), _("Edit the electrical mesh"), self)
		self.tb_mesh.triggered.connect(self.callback_edit_mesh)
		self.toolbar.addAction(self.tb_mesh)

		self.tb_doping = QAction(QIcon(os.path.join(get_image_file_path(),"doping.png")), _("Doping"), self)
		self.tb_doping.triggered.connect(self.callback_doping)
		self.toolbar.addAction(self.tb_doping)

		self.tb_contact = QAction(QIcon(os.path.join(get_image_file_path(),"contact.png")), _("Contacts"), self)
		self.tb_contact.triggered.connect(self.callback_contacts)
		self.toolbar.addAction(self.tb_contact)

		self.tb_open = QAction(QIcon(os.path.join(get_image_file_path(),"open.png")), _("Look at the materials database"), self)
		self.tb_open.triggered.connect(self.callback_view_materials)
		self.toolbar.addAction(self.tb_open)

		self.main_vbox.addWidget(self.toolbar)
	

		self.tab = QTableWidget()
		self.tab.resizeColumnsToContents()

		self.tab.verticalHeader().setVisible(False)
		self.create_model()

		self.tab.cellChanged.connect(self.tab_changed)

		self.main_vbox.addWidget(self.tab)

		self.electrical_mesh=tab_electrical_mesh()

		self.setLayout(self.main_vbox)


		return




		self.electrical_mesh.emesh_editor_y.connect("refresh", self.change_active_layer_thickness)


		self.frame.set_label(_("Device layers"))



	def create_model(self):
		#self.rebuild_mat_list()
		self.tab.clear()
		self.tab.setColumnCount(6)
		self.tab.setColumnHidden(5, True)
		self.tab.setColumnHidden(4, True)
		self.tab.setSelectionBehavior(QAbstractItemView.SelectRows)
		self.tab.setHorizontalHeaderLabels([_("Layer name"), _("Thicknes"), _("Optical material"), _("Layer type"), _("DoS Layer"),_("PL Layer")])

		data1 = ['row1','row2','row3','row4']
		data2 = ['1','2.0','3.00000001','3.9999999']
		combo_box_options = ["Option 1","Option 2","Option 3"]

		self.tab.setRowCount(epitaxy_get_layers())

		for i in range(0,epitaxy_get_layers()):
			thick=epitaxy_get_width(i)
			material=epitaxy_get_mat_file(i)
			dos_layer=epitaxy_get_electrical_layer(i)
			pl_file=epitaxy_get_pl_file(i)
			name=epitaxy_get_name(i)

			dos_file=""
			
			if dos_layer.startswith("dos")==True:
				dos_file="Active layer"
			else:
				dos_file=dos_layer

			item1 = QTableWidgetItem(str(name))
			self.tab.setItem(i,0,item1)

			item2 = QTableWidgetItem(str(thick))
			self.tab.setItem(i,1,item2)

			item3 = QTableWidgetItem(str(material))
			self.tab.setItem(i,2,item3)

			item3 = QTableWidgetItem(str(dos_file))
			self.tab.setItem(i,3,item3)


			item3 = QTableWidgetItem(str(dos_layer))
			self.tab.setItem(i,4,item3)

			item3 = QTableWidgetItem(str(pl_file))
			self.tab.setItem(i,5,item3)


			#combo = QComboBox()
			#for t in combo_box_options:
			#	combo.addItem(t)
			#self.tab.setCellWidget(index,2,combo)

			scan_item_add("epitaxy.inp","#layer"+str(i),_("Material for ")+str(material),2)
			scan_item_add("epitaxy.inp","#layer"+str(i),_("Layer width ")+str(material),1)

		return



	def on_remove_item_clicked(self, button):
		index = self.tab.selectionModel().selectedRows()

		print index
		if len(index)>0:
			pos=index[0].row()
			self.tab.removeRow(pos)
#			self.save_model()
#			self.refresh(True)





	def on_cell_edited(self, cell, path_string, new_text, model):
		iter = model.get_iter_from_string(path_string)
		#path = model.get_path(iter)[0]
		column = cell.get_data("column")

		model.set(iter, column, new_text)

		self.save_model()
		self.refresh(True)

	def on_dos_layer_edited(self, cell, path_string, new_text, model):

		iter = model.get_iter_from_string(path_string)
		#path = model.get_path(iter)[0]
		column = cell.get_data("column")

		model.set(iter, column, new_text)

		self.save_model()
		self.refresh(True)

	def change_active_layer_thickness(self,obj):
		thickness=obj.get_data("refresh")
		print thickness
		count=0
		for item in self.model:
			if str2bool(item[COLUMN_DEVICE])==True:
				count=count+1

		if count==1:
			for item in self.model:
				if str2bool(item[COLUMN_DEVICE])==True:
					item[COLUMN_THICKNES]=str(thickness)
					self.save_model()
					self.refresh(False)
					return

	def on_add_item_clicked(self):
		index = self.tab.selectionModel().selectedRows()

		print index
		if len(index)>0:
			pos=index[0].row()+1
		else:
			pos = self.tab.rowCount()

		self.tab.insertRow(pos)

		self.tab.setItem(pos,0,QTableWidgetItem("layer name"))

		self.tab.setItem(pos,1,QTableWidgetItem("100e-9"))

		self.tab.setItem(pos,2,QTableWidgetItem("pcbm"))

		self.tab.setItem(pos,3,QTableWidgetItem(_("Other")))

		self.tab.setItem(pos,4,QTableWidgetItem("none"))

		self.tab.setItem(pos,5,QTableWidgetItem("none"))


		self.save_model()
		#self.refresh(True)


	def refresh(self,emit):
		self.electrical_mesh.refresh()
		if emit==True:
			self.emit("refresh")

		global_object_get("dos-update")()
		global_object_get("pl-update")()


	def save_model(self):

		thick=[]
		mat_file=[]
		dos_file=[]
		pl_file=[]
		name=[]

		for i in range(0,self.tab.rowCount()):
			name.append(str(self.tab.item(i, 0).text()))
			thick.append(str(self.tab.item(i, 1).text()))
			mat_file.append(str(self.tab.item(i, 2).text()))
			dos_file.append(str(self.tab.item(i, 4).text()))
			pl_file.append(str(self.tab.item(i, 5).text()))

		epitaxy_load_from_arrays(name,thick,mat_file,dos_file,pl_file)

		print thick

		epitaxy_save()
		#self.sync_to_electrical_mesh()

	def callback_doping(self, widget, data=None):
		help_window().help_set_help(["doping.png",_("<big><b>Doping window</b></big>\nUse this window to add doping to the simulation")])

		if self.doping_window==False:
			self.doping_window=doping_window()
			self.doping_window.init()

		if self.doping_window.get_property("visible")==True:
			self.doping_window.hide()
		else:
			self.doping_window.show()

	def callback_contacts(self, widget, data=None):
		help_window().help_set_help(["contact.png",_("<big><b>Contacts window</b></big>\nUse this window to change the layout of the contacts on the device")])

		if self.contacts_window==False:
			self.contacts_window=contacts_window()

		if self.contacts_window.isVisible()==True:
			self.contacts_window.hide()
		else:
			self.contacts_window.show()

#gobject.type_register(layer_widget)
#gobject.signal_new("refresh", layer_widget, gobject.SIGNAL_RUN_FIRST,gobject.TYPE_NONE, ())

