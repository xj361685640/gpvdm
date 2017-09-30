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

import os
from icon_lib import QIcon_load

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
from cost import cost
from materials_main import materials_main

from parasitic import parasitic
from plot_gen import plot_gen
from cal_path import get_spectra_path

from spectra_main import spectra_main
from layer_widget import layer_widget

class ribbon_database(QToolBar):
	def __init__(self):
		QToolBar.__init__(self)

		self.setToolButtonStyle( Qt.ToolButtonTextUnderIcon)
		self.setIconSize(QSize(42, 42))
		
		self.materials = QAction(QIcon_load("organic_material"), _("Materials\ndatabase"), self)
		self.materials.triggered.connect(self.callback_view_materials)
		self.addAction(self.materials)

		self.spectra_file = QAction(QIcon_load("spectra_file"), _("Optical\ndatabase"), self)
		self.spectra_file.triggered.connect(self.callback_view_optical)
		self.addAction(self.spectra_file)

		if enable_betafeatures()==True:
			self.parasitic = QAction(QIcon_load("sync"), _("Update materials\nfrom PVLighthouse"), self)
			self.parasitic.triggered.connect(self.callback_pvlighthouse)
			self.addAction(self.parasitic)

	def update(self):
		return

	def setEnabled(self,val):
		self.materials.setEnabled(val)
		self.spectra_file.setEnabled(val)



	def callback_view_materials(self):
		dialog=gpvdm_open(get_materials_path())
		dialog.show_inp_files=False
		dialog.menu_new_material_enabled=True
		ret=dialog.exec_()

		if ret==QDialog.Accepted:
			if os.path.isfile(os.path.join(dialog.get_filename(),"mat.inp"))==True:
				self.mat_window=materials_main(dialog.get_filename())
				self.mat_window.show()
			else:
				plot_gen([dialog.get_filename()],[],"auto")

	def callback_view_optical(self):
		dialog=gpvdm_open(get_spectra_path())
		dialog.menu_new_spectra_enabled=True
		dialog.show_inp_files=False
		ret=dialog.exec_()

		if ret==QDialog.Accepted:
			if os.path.isfile(os.path.join(dialog.get_filename(),"mat.inp"))==True:
				self.mat_window=spectra_main(dialog.get_filename())
				self.mat_window.show()


	def callback_pvlighthouse(self):
		from pvlighthouse import pvlighthouse_sync
		pvlighthouse_sync()
