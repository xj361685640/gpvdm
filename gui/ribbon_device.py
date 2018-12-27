# -*- coding: utf-8 -*-
#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2017 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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

## @package ribbon_device
#  The device editor ribbon.
#


import os
from icon_lib import icon_get

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

from doping import doping_window
from contacts import contacts_window
from cal_path import get_materials_path
from gpvdm_open import gpvdm_open

from help import help_window
from materials_main import materials_main

from parasitic import parasitic
from plot_gen import plot_gen
from cal_path import get_spectra_path

from spectra_main import spectra_main
from layer_widget import layer_widget
from dim_editor import dim_editor

from electrical import electrical
from global_objects import global_object_register

class ribbon_device(QToolBar):
	def __init__(self):
		QToolBar.__init__(self)
		self.doping_window=None
		self.cost_window=None
		self.parasitic=None
		self.contacts_window=None
		self.parasitic_window=None
		self.layer_editor=None
		self.dim_editor=None
		self.electrical_editor=None

		self.setToolButtonStyle( Qt.ToolButtonTextUnderIcon)
		self.setOrientation(Qt.Vertical);
		self.setIconSize(QSize(42, 42))

		self.tb_layer_editor = QAction(icon_get("layers"), _("Layer\neditor"), self)
		self.tb_layer_editor.triggered.connect(self.callback_layer_editor)
		self.addAction(self.tb_layer_editor)
		global_object_register("show_layer_editor",self.callback_layer_editor)
		
		self.contacts = QAction(icon_get("contact"), _("Contacts"), self)
		self.contacts.triggered.connect(self.callback_contacts)
		self.addAction(self.contacts)
		
		self.doping = QAction(icon_get("doping"), _("Doping"), self)
		self.doping.triggered.connect(self.callback_doping)
		self.addAction(self.doping)

		self.parasitic = QAction(icon_get("parasitic"), _("Parasitic\n components"), self)
		self.parasitic.triggered.connect(self.callback_parasitic)
		self.addAction(self.parasitic)

		self.tb_electrical_editor = QAction(icon_get("electrical"), _("Electrical\nparameters"), self)
		self.tb_electrical_editor.triggered.connect(self.callback_electrical_editor)
		self.addAction(self.tb_electrical_editor)

		self.tb_dimension_editor = QAction(icon_get("dimensions"), _("xz-size"), self)
		self.tb_dimension_editor.triggered.connect(self.callback_dimension_editor)
		self.addAction(self.tb_dimension_editor)

	def update(self):
		if self.cost_window!=None:
			del self.cost_window
			self.cost_window=None
			
		if self.doping_window!=None:
			del self.doping_window
			self.doping_window=None

		if self.contacts_window!=None:
			del self.contacts_window
			self.contacts_window=None

		if self.parasitic_window!=None:
			del self.parasitic_window
			self.parasitic_window=None

		if self.layer_editor!=None:
			del self.layer_editor
			self.layer_editor=None

		if self.dim_editor!=None:
			del self.dim_editor
			self.dim_editor=None

		if self.electrical_editor!=None:
			del self.electrical_editor
			self.electrical_editor=None

	def setEnabled(self,val):
		self.doping.setEnabled(val)
		self.cost.setEnabled(val)
		self.contacts.setEnabled(val)
		self.parasitic.setEnabled(val)
		self.tb_electrical_editor.setEnabled(val)
		
	def callback_doping(self):
		help_window().help_set_help(["doping.png",_("<big><b>Doping window</b></big>\nUse this window to add doping to the simulation")])

		if self.doping_window==None:
			self.doping_window=doping_window()

		if self.doping_window.isVisible()==True:
			self.doping_window.hide()
		else:
			self.doping_window.show()
		
	def callback_contacts(self):
		
		
		help_window().help_set_help(["contact.png",_("<big><b>Contacts window</b></big>\nUse this window to change the layout of the contacts on the device")])

		if self.contacts_window==None:
			self.contacts_window=contacts_window()
			
		if self.contacts_window.isVisible()==True:
			self.contacts_window.hide()
		else:
			self.contacts_window.show()


	def callback_parasitic(self):
		help_window().help_set_help(["parasitic.png",_("<big><b>Parasitic components</b></big>\nUse this window to edit the shunt and series resistance.")])

		if self.parasitic_window==None:
			self.parasitic_window=parasitic()

		if self.parasitic_window.isVisible()==True:
			self.parasitic_window.hide()
		else:
			self.parasitic_window.show()

	def callback_layer_editor(self):
		help_window().help_set_help(["layers.png",_("<big><b>Device layers</b></big>\nUse this window to configure the structure of the device.")])

		if self.layer_editor==None:
			self.layer_editor=layer_widget()

		if self.layer_editor.isVisible()==True:
			self.layer_editor.hide()
		else:
			self.layer_editor.show()

	def callback_dimension_editor(self):
		help_window().help_set_help(["dimension.png",_("<big><b>xz dimension editor</b></big>\nUse this window to configure the xz size of the device.")])

		if self.dim_editor==None:
			self.dim_editor=dim_editor()

		if self.dim_editor.isVisible()==True:
			self.dim_editor.hide()
		else:
			self.dim_editor.show()


	def callback_electrical_editor(self):
		help_window().help_set_help(["electrical.png",_("<big><b>Electrical parameters</b></big>\nUse this window to change the electrical parameters of each layer.")])

		if self.electrical_editor==None:
			self.electrical_editor=electrical()

		if self.electrical_editor.isVisible()==True:
			self.electrical_editor.hide()
		else:
			self.electrical_editor.show()

