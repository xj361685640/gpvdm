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

## @package dim_editor
#  A window to edit the dimentions of the device.
#

import os
from util import str2bool
from inp import inp_search_token_value
from scan_item import scan_item_add
from scan_item import scan_remove_file
from icon_lib import icon_get
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
from error_dlg import error_dlg

#mesh
from mesh import mesh_get_xlen
from mesh import mesh_get_zlen
from mesh import mesh_set_xlen
from mesh import mesh_set_zlen
from mesh import mesh_save_x
from mesh import mesh_save_z

#qt
from PyQt5.QtCore import QSize, Qt
from PyQt5.QtGui import QIcon,QPalette
from PyQt5.QtWidgets import QWidget, QVBoxLayout,QProgressBar,QLineEdit,QLabel,QDesktopWidget,QToolBar,QHBoxLayout,QAction, QSizePolicy, QTableWidget, QTableWidgetItem,QComboBox,QDialog,QAbstractItemView

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
from cal_path import get_default_material_path
from materials_select import materials_select
from QWidgetSavePos import QWidgetSavePos

from epitaxy_mesh_update import epitaxy_mesh_update

class dim_editor(QWidgetSavePos):

	def __init__(self):
		QWidgetSavePos.__init__(self,"dim_editor")

		self.setWindowTitle(_("xz dimension editor")+" https://www.gpvdm.com")
		self.setWindowIcon(icon_get("dimensions"))
		self.resize(400,200)

		self.cost_window=False

		self.main_vbox=QVBoxLayout()

		self.toolbar=QToolBar()
		self.toolbar.setToolButtonStyle( Qt.ToolButtonTextUnderIcon)
		self.toolbar.setIconSize(QSize(42, 42))

		spacer = QWidget()


		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		self.toolbar.addWidget(spacer)

		self.help = QAction(icon_get("internet-web-browser"), _("Help"), self)
		self.toolbar.addAction(self.help)
		
		self.main_vbox.addWidget(self.toolbar)
	
		self.widget0 = QWidget()
		self.widget0_hbox=QHBoxLayout()
		self.widget0.setLayout(self.widget0_hbox)

		self.widget0_label=QLabel("x size")
		self.widget0_hbox.addWidget(self.widget0_label)

		self.widget0_edit=QLineEdit()
		self.widget0_edit.setText(str(mesh_get_xlen()))
		self.widget0_edit.textChanged.connect(self.apply)
		self.widget0_hbox.addWidget(self.widget0_edit)
		self.widget0_label=QLabel("m")
		self.widget0_hbox.addWidget(self.widget0_label)

		self.main_vbox.addWidget(self.widget0)

		self.widget1 = QWidget()
		self.widget1_hbox=QHBoxLayout()
		self.widget1.setLayout(self.widget1_hbox)
		self.widget1_label=QLabel("z size")
		self.widget1_hbox.addWidget(self.widget1_label)
		self.widget1_edit=QLineEdit()
		self.widget1_edit.setText(str(mesh_get_zlen()))
		self.widget1_edit.textChanged.connect(self.apply)
		self.widget1_hbox.addWidget(self.widget1_edit)
		self.widget1_label=QLabel("m")
		self.widget1_hbox.addWidget(self.widget1_label)
		self.main_vbox.addWidget(self.widget1)

		
		#self.tab.itemSelectionChanged.connect(self.callback_tab_selection_changed)


		self.setLayout(self.main_vbox)

		#self.tab.itemSelectionChanged.connect(self.layer_selection_changed)


	def apply(self):
		try:
			val=float(self.widget0_edit.text())
			if val<=0:
				return
			if val>0.1:
				return
		except:
			return
		mesh_set_xlen(val)

		try:
			val=float(self.widget1_edit.text())
			if val<=0:
				return
			if val>0.1:
				return

		except:
			return
		mesh_set_zlen(val)

		mesh_save_x()
		mesh_save_z()

		global_object_run("mesh_update")
		global_object_run("gl_force_redraw")



