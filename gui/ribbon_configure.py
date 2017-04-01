# -*- coding: utf-8 -*-
#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2017 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
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

from dump_io import dump_io
from tb_item_sim_mode import tb_item_sim_mode
from tb_item_sun import tb_item_sun

from code_ctrl import enable_betafeatures
from cal_path import get_css_path

#qt
from PyQt5.QtWidgets import QMainWindow, QTextEdit, QAction, QApplication
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import QSize, Qt,QFile,QIODevice
from PyQt5.QtWidgets import QWidget,QSizePolicy,QVBoxLayout,QHBoxLayout,QPushButton,QDialog,QFileDialog,QToolBar,QMessageBox, QLineEdit, QToolButton
from PyQt5.QtWidgets import QTabWidget

from emesh import tab_electrical_mesh
from config_window import class_config_window

from help import help_window

from global_objects import global_object_register

class ribbon_configure(QToolBar):
	def __init__(self):
		QToolBar.__init__(self)
		self.config_window=None
		self.electrical_mesh=tab_electrical_mesh()
		self.setToolButtonStyle( Qt.ToolButtonTextUnderIcon)
		self.setIconSize(QSize(42, 42))

		self.configwindow = QAction(QIcon(os.path.join(get_image_file_path(),"preferences-system.png")), _("Configure"), self)
		self.configwindow.triggered.connect(self.callback_config_window)
		self.addAction(self.configwindow)
		
		self.dump = dump_io(self)
		global_object_register("ribbon_configure_dump_refresh",self.dump.refresh)
		self.addAction(self.dump)

		self.mesh = QAction(QIcon(os.path.join(get_image_file_path(),"mesh.png")), _("Electrical\nmesh"), self)
		self.mesh.triggered.connect(self.callback_edit_mesh)		
		self.addAction(self.mesh)
		
	def update(self):
		if self.config_window!=None:
			del self.config_window
			self.config_window=None

		self.dump.refresh()
		self.electrical_mesh.update()
		
	def setEnabled(self,val):
		self.configwindow.setEnabled(val)
		self.dump.setEnabled(val)
		self.mesh.setEnabled(val)
		
		
	def callback_edit_mesh(self):
		help_window().help_set_help(["mesh.png",_("<big><b>Mesh editor</b></big>\nUse this window to setup the mesh, the window can also be used to change the dimensionality of the simulation.")])

		if self.electrical_mesh.isVisible()==True:
			self.electrical_mesh.hide()
		else:
			self.electrical_mesh.show()
			
	def callback_config_window(self):

		if self.config_window==None:
			self.config_window=class_config_window()
			self.config_window.init()
			self.config_window.changed.connect(self.dump.refresh)

		help_window().help_set_help(["preferences-system.png",_("<big><b>Configuration editor</b></big><br> Use this window to control advanced simulation parameters.")])
		if self.config_window.isVisible()==True:
			self.config_window.hide()
		else:
			self.config_window.show()
