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
from cal_path import find_materials
from util import str2bool
from inp_util import inp_search_token_value
from scan_item import scan_item_add
from scan_item import scan_remove_file
from icon_lib import QIcon_load
from gpvdm_open import gpvdm_open
from cal_path import get_materials_path
from global_objects import global_object_get
from help import help_window

#inp
from inp import inp_isfile
from inp import inp_copy_file
from inp import inp_update_token_value
from inp import inp_load_file
from inp import inp_lsdir
from inp import inp_remove_file

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
from gui_util import tab_move_down
from gui_util import tab_move_up
from gui_util import tab_add
from gui_util import tab_remove
from gui_util import tab_get_value
from gui_util import tab_set_value
from gui_util import yes_no_dlg
from gui_util import tab_insert_row
from gui_util import error_dlg

#qt
from PyQt5.QtCore import QSize, Qt
from PyQt5.QtGui import QIcon,QPalette
from PyQt5.QtWidgets import QWidget, QVBoxLayout,QProgressBar,QLabel,QDesktopWidget,QToolBar,QHBoxLayout,QAction, QSizePolicy, QTableWidget, QTableWidgetItem,QComboBox,QDialog,QAbstractItemView

from PyQt5.QtGui import QPixmap
from PyQt5.QtWidgets import QWidget

from global_objects import global_object_run

from global_objects import global_isobject
from global_objects import global_object_get

from QComboBoxLang import QComboBoxLang

import i18n
_ = i18n.language.gettext

from i18n import yes_no

from gpvdm_select import gpvdm_select

from code_ctrl import enable_betafeatures
from cal_path import get_sim_path
from materials_select import materials_select
from QWidgetSavePos import QWidgetSavePos

from epitaxy_mesh_update import epitaxy_mesh_update

class layer_widget(QWidgetSavePos):

	
	def combo_changed(self):
		self.save_model()
		self.emit_change()
		self.emit_structure_changed()

	def callback_tab_selection_changed(self):
		#self.tab_changed(0,0)
		self.emit_change()

	def tab_changed(self, x,y):
		self.save_model()
		self.emit_structure_changed()
		
	def emit_change(self):
		global_object_run("gl_force_redraw")
		
	def emit_structure_changed(self):		#This will emit when there has been an edit
		global_object_run("mesh_update")
		global_object_run("optics_force_redraw")
		global_object_run("gl_force_redraw")

	def layer_type_edit(self):
		for i in range(0,self.tab.rowCount()):
			if tab_get_value(self.tab,i,3).lower()=="active layer" and tab_get_value(self.tab,i,4).startswith("dos")==False:
				tab_set_value(self.tab,i,4,epitay_get_next_dos())
				tab_set_value(self.tab,i,5,epitay_get_next_pl())

				mat_dir=os.path.join(get_materials_path(),tab_get_value(self.tab,i,2))
				
				new_file=tab_get_value(self.tab,i,4)+".inp"
				if inp_isfile(new_file)==False:
					inp_copy_file(new_file,os.path.join(mat_dir,"dos.inp"))

				new_file=tab_get_value(self.tab,i,5)+".inp"
				if inp_isfile(new_file)==False:
					inp_copy_file(new_file,os.path.join(mat_dir,"pl.inp"))

			if tab_get_value(self.tab,i,3).lower()!="active layer" and tab_get_value(self.tab,i,4).startswith("dos")==True:
				tab_set_value(self.tab,i,4,tab_get_value(self.tab,i,3))
				tab_set_value(self.tab,i,5,"none")

			if tab_get_value(self.tab,i,3).lower()=="other":
				tab_set_value(self.tab,i,4,tab_get_value(self.tab,i,3))

			if tab_get_value(self.tab,i,3).lower()=="contact":
				tab_set_value(self.tab,i,4,tab_get_value(self.tab,i,3))

		self.save_model()
		self.emit_change()
		global_object_run("dos_update")
		global_object_run("pl_update")

	def on_move_down(self):
		tab_move_down(self.tab)
		self.save_model()
		self.emit_change()
		self.emit_structure_changed()

	def on_move_up(self):
		tab_move_up(self.tab)
		self.save_model()
		self.emit_change()
		self.emit_structure_changed()

	def __init__(self):
		QWidgetSavePos.__init__(self,"layer_widget")

		self.setWindowTitle(_("Layer editor")+" https://www.gpvdm.com")
		self.setWindowIcon(QIcon_load("layers"))
		self.resize(800,500)

		self.cost_window=False

		self.main_vbox=QVBoxLayout()

		self.toolbar=QToolBar()
		self.toolbar.setIconSize(QSize(32, 32))

		self.tb_add = QAction(QIcon_load("list-add"), _("Add device layer"), self)
		self.tb_add.triggered.connect(self.on_add_item_clicked)
		self.toolbar.addAction(self.tb_add)

		self.tb_remove = QAction(QIcon_load("list-remove"), _("Delete device layer"), self)
		self.tb_remove.triggered.connect(self.on_remove_item_clicked)
		self.toolbar.addAction(self.tb_remove)


		self.tb_down= QAction(QIcon_load("go-down"), _("Move device layer"), self)
		self.tb_down.triggered.connect(self.on_move_down)
		self.toolbar.addAction(self.tb_down)

		self.tb_up= QAction(QIcon_load("go-up"), _("Move device layer"), self)
		self.tb_up.triggered.connect(self.on_move_up)
		self.toolbar.addAction(self.tb_up)
		
		self.main_vbox.addWidget(self.toolbar)
	
		self.tab = QTableWidget()
		#self.tab.resizeColumnsToContents()


		self.tab.verticalHeader().setVisible(False)
		self.create_model()

		self.tab.cellChanged.connect(self.tab_changed)
		self.tab.itemSelectionChanged.connect(self.callback_tab_selection_changed)
		self.main_vbox.addWidget(self.tab)

		self.setLayout(self.main_vbox)

		self.tab.itemSelectionChanged.connect(self.layer_selection_changed)


	def create_model(self):
		self.tab.clear()
		self.tab.setColumnCount(6)
		if enable_betafeatures()==False:
			self.tab.setColumnHidden(5, True)
			self.tab.setColumnHidden(4, True)

		self.tab.setSelectionBehavior(QAbstractItemView.SelectRows)
		self.tab.setHorizontalHeaderLabels([_("Layer name"), _("Thicknes"), _("Optical material"), _("Layer type"), _("DoS Layer"),_("PL Layer")])
		self.tab.setColumnWidth(2, 250)
		self.tab.setRowCount(epitaxy_get_layers())

		for i in range(0,epitaxy_get_layers()):
			thick=epitaxy_get_width(i)
			material=epitaxy_get_mat_file(i)
			dos_layer=epitaxy_get_electrical_layer(i)
			pl_file=epitaxy_get_pl_file(i)
			name=epitaxy_get_name(i)

			self.add_row(i,thick,material,dos_layer,pl_file,name)
		return

	def add_row(self,i,thick,material,dos_layer,pl_file,name):
		self.tab.blockSignals(True)

		dos_file=""
		
		if dos_layer.startswith("dos")==True:
			dos_file="active layer"
		else:
			dos_file=dos_layer

		item1 = QTableWidgetItem(str(name))
		self.tab.setItem(i,0,item1)

		item2 = QTableWidgetItem(str(thick))
		self.tab.setItem(i,1,item2)


		combobox = gpvdm_select()
		combobox.setText(material)
		combobox.button.clicked.connect(self.callback_material_select)
		
		self.tab.setCellWidget(i,2, combobox)
#		combobox.setCurrentIndex(combobox.findText(material))

		#p=combobox.palette()
		#p.setColor(QPalette.Active, QPalette.Button, Qt.white);
		#p.setColor(QPalette.Inactive, QPalette.Button, Qt.white);
		#combobox.setPalette(p)
		
		#item3 = QTableWidgetItem(str(dos_file))
		#self.tab.setItem(i,3,item3)
		combobox_layer_type = QComboBoxLang()
		#combobox.setEditable(True)

		combobox_layer_type.addItemLang("contact",_("contact"))
		combobox_layer_type.addItemLang("active layer",_("active layer"))
		combobox_layer_type.addItemLang("other",_("other"))

		self.tab.setCellWidget(i,3, combobox_layer_type)
		combobox_layer_type.setValue_using_english(str(dos_file).lower())

		item3 = QTableWidgetItem(str(dos_layer))
		self.tab.setItem(i,4,item3)

		item3 = QTableWidgetItem(str(pl_file))
		self.tab.setItem(i,5,item3)

		#combobox.currentIndexChanged.connect(self.combo_changed)
		combobox_layer_type.currentIndexChanged.connect(self.layer_type_edit)

		self.tab.blockSignals(False)

	def callback_material_select(self):
		self.mat_select=materials_select()
		self.mat_select.init(self.tab)
		self.mat_select.set_save_function(self.combo_changed)
		self.mat_select.show()
		
	def clean_dos_files(self):
		files=inp_lsdir("sim.gpvdm")
		tab=[]
		for i in range(0,self.tab.rowCount()):
			tab.append(str(tab_get_value(self.tab,i, 4))+".inp")

		for i in range(0,len(files)):
			if files[i].startswith("dos") and files[i].endswith(".inp"):
				disk_file=files[i]
				if disk_file not in tab:
					inp_remove_file(disk_file)

	def on_remove_item_clicked(self):
		tab_remove(self.tab)
		self.save_model()
		self.emit_change()

	def change_active_layer_thickness(self,obj):
		thickness=obj.get_data("refresh")
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
		row=tab_insert_row(self.tab)

		self.add_row(row,"100e-9","pcbm","other","none","layer"+str(row))
		self.save_model()
		#self.changed.emit()
		self.emit_change()

	def save_model(self):

		thick=[]
		mat_file=[]
		dos_file=[]
		pl_file=[]
		name=[]
		for i in range(0,self.tab.rowCount()):
			name.append(str(tab_get_value(self.tab,i, 0)))
			thick.append(str(tab_get_value(self.tab,i, 1)))
			mat_file.append(str(tab_get_value(self.tab,i, 2)))
			dos_file.append(str(tab_get_value(self.tab,i, 4)))
			pl_file.append(str(tab_get_value(self.tab,i, 5)))

		ret=epitaxy_load_from_arrays(name,thick,mat_file,dos_file,pl_file)
		if ret==False:
			error_dlg(self,_("Error in epitaxy, check the input values."))

		epitaxy_save(get_sim_path())
		self.clean_dos_files()
		epitaxy_mesh_update()

	def layer_selection_changed(self):
		a=self.tab.selectionModel().selectedRows()

		if len(a)>0:
			y=a[0].row()
		else:
			y=-1
		
		if global_isobject("display_set_selected_layer")==True:
			global_object_get("display_set_selected_layer")(y)
		global_object_run("gl_force_redraw")

		#self.three_d.set_selected_layer(y)
		#self.three_d.update()
