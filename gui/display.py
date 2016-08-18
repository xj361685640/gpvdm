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


from util import read_xyz_data

import os

#inp
from inp import inp_load_file
from inp_util import inp_search_token_value

#path
from cal_path import get_materials_path

from gl import glWidget
from gl_fallback import gl_fallback
#qt
from PyQt5.QtWidgets import QMainWindow, QTextEdit, QAction, QApplication
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QSizePolicy,QHBoxLayout,QPushButton,QDialog,QFileDialog,QToolBar,QMessageBox, QLineEdit,QLabel
from PyQt5.QtCore import QTimer

open_gl_working=False

def is_open_gl_working():
	global open_gl_working
	if open_gl_working==True:
		return "yes"
	else:
		return "no"

class display_widget(QWidget):

	colors=[]
	def __init__(self):
		QWidget.__init__(self)
		self.complex_display=False
		self.hbox=QHBoxLayout()
		self.display=glWidget(self)
		self.hbox.addWidget(self.display)
		self.setMinimumSize(800, 500)

		self.setLayout(self.hbox)
		self.timer=QTimer()
		self.timer.setSingleShot(True)
		self.timer.timeout.connect(self.update)
		self.timer.start(2000)

	def update(self):
		#self.display.enabled=False
		global open_gl_working
		
		open_gl_working=self.display.enabled

		if self.display.enabled==True:
			print("OpenGL is working")
		else:
			print("OpenGL is not working going to fallback")
			self.hbox.removeWidget(self.display)
			self.display.deleteLater()
			self.display = None

			self.display=gl_fallback()
			self.hbox.addWidget(self.display)

		
	def recalculate(self):
#		print("recalculate")
		self.display.recalculate()
