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

## @package tb_item_mat_file
#  Toolbar item for the material file.
#


import os

#inp
from inp import inp_update_token_value
from inp import inp_get_token_value
from inp import inp_lsdir
from inp import inp_search_token_value

#qt
from PyQt5.QtWidgets import QMainWindow, QTextEdit, QAction, QApplication
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QSizePolicy,QVBoxLayout,QPushButton,QDialog,QFileDialog,QToolBar,QLabel,QComboBox

import i18n
_ = i18n.language.gettext


class tb_item_mat_file(QWidget):


	def __init__(self,path,token):
		QWidget.__init__(self)
		self.path=path
		self.token=token
		layout=QVBoxLayout()
		label=QLabel()
		label.setText(_("Use:"))
		layout.addWidget(label)

		self.mode = QComboBox(self)
		self.mode.setEditable(True)
		self.mode.clear()
		self.mode.addItem(_("Raw data"))
		self.mode.addItem(_("Equation"))

		value=inp_get_token_value(os.path.join(self.path,"mat.inp"), self.token)
		if value=="data":
			self.mode.setCurrentIndex(0)
		else:
			self.mode.setCurrentIndex(1)

		self.mode.currentIndexChanged.connect(self.call_back_mode_changed)

		layout.addWidget(self.mode)

		self.setLayout(layout)

		return




	def call_back_mode_changed(self):
		mode=self.mode.currentText()
		if mode==_("Raw data"):
			inp_update_token_value(os.path.join(self.path,"mat.inp"), self.token, "data" )
		if mode==_("Equation"):
			inp_update_token_value(os.path.join(self.path,"mat.inp"), self.token, "equation" )

