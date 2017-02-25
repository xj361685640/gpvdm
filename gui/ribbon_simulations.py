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


class ribbon_simulations(QToolBar):
	def __init__(self):
		QToolBar.__init__(self)
		self.setToolButtonStyle( Qt.ToolButtonTextUnderIcon)
		self.setIconSize(QSize(42, 42))

		self.time = QAction(QIcon(os.path.join(get_image_file_path(),"time.png")), _("Time domain\nsimulation editor."), self)
		self.addAction(self.time )


		self.fx = QAction(QIcon(os.path.join(get_image_file_path(),"spectrum.png")), _("Frequency domain\nsimulation editor"), self)
		self.addAction(self.fx)


		self.jv = QAction(QIcon(os.path.join(get_image_file_path(),"jv.png")), _("Steady state\nsimulation editor"), self)
		self.addAction(self.jv)


		self.qe = QAction(QIcon(os.path.join(get_image_file_path(),"qe.png")), _("Quantum\nefficiency"), self)
		self.addAction(self.qe)
		self.qe.setVisible(False)

		self.mode=tb_item_sim_mode()
		self.addWidget(self.mode)
		
		self.optics = QAction(QIcon(os.path.join(get_image_file_path(),"optics.png")), _("Optical\nSimulation"), self)
		self.addAction(self.optics)

		self.lasers = QAction(QIcon(os.path.join(get_image_file_path(),"lasers.png")), _("Laser\neditor"), self)
		self.addAction(self.lasers)
		
	def update(self):
		print("update")
		
	def setEnabled(self,val):
		self.time.setEnabled(val)
		self.fx.setEnabled(val)
		self.jv.setEnabled(val)
		self.qe.setEnabled(val)
		self.mode.setEnabled(val)
		self.optics.setEnabled(val)
		self.lasers.setEnabled(val)
		
