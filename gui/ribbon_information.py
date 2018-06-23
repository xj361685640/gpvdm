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

import webbrowser
from help import help_window

class ribbon_information(QToolBar):
	def __init__(self):
		QToolBar.__init__(self)
		self.setToolButtonStyle( Qt.ToolButtonTextUnderIcon)
		self.setIconSize(QSize(42, 42))




		self.license = QAction(QIcon_load("license"), _("License")+"\n"	, self)
		self.license.triggered.connect(self.callback_license)
		self.addAction(self.license)		

		self.ref = QAction(QIcon_load("ref"), _("How to\ncite"), self)
		self.ref.triggered.connect(self.callback_ref)
		self.addAction(self.ref)

		self.hints = QAction(QIcon_load("hints.png"), _("Hints\nWindow"), self)
		self.hints.triggered.connect(self.callback_help)
		self.addAction(self.hints)

		#self.about = QAction(QIcon_load("help"), _("About")+"\n", self)
		#self.addAction(self.about)


		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		self.addWidget(spacer)


		self.twitter = QAction(QIcon_load("twitter.png"), _("twitter"), self)
		self.twitter.triggered.connect(self.callback_twitter)
		self.addAction(self.twitter)

		self.fb = QAction(QIcon_load("fb.png"), _("Facebook"), self)
		self.fb.triggered.connect(self.callback_fb)
		self.addAction(self.fb)

		
		self.youtube = QAction(QIcon_load("youtube.png"), _("Youtube\nchannel"), self)
		self.youtube.triggered.connect(self.callback_youtube)
		self.addAction(self.youtube)

		self.man = QAction(QIcon_load("internet-web-browser"), _("Help")+"\n", self)
		self.man.triggered.connect(self.callback_on_line_help)
		self.addAction(self.man)

	def update(self):
		print("update")
		
	def setEnabled(self,val):
		self.license.setEnabled(val)
		self.ref.setEnabled(val)
		self.hints.setEnabled(val)
		self.youtube.setEnabled(val)
		self.man.setEnabled(val)

	def callback_license(self):
		webbrowser.open("https://www.gpvdm.com/license.html")
		
	def callback_youtube(self):
		webbrowser.open("https://www.youtube.com/channel/UCbm_0AKX1SpbMMT7jilxFfA")

	def callback_twitter(self):
		webbrowser.open("https://twitter.com/gpvdm_info")

	def callback_fb(self):
		webbrowser.open("https://www.facebook.com/gpvdminfo/")

	def callback_ref(self):
		webbrowser.open("https://gpvdm.com/how_to_cite.html")
		
	def callback_on_line_help(self):
		#print("here")
		#self.a=cool_menu(self.ribbon.home.help.icon())
		#self.a.show()
		#self.a.setVisible(True)

		#self.a.setFocusPolicy(Qt.StrongFocus)
		#self.a.setFocus(True)
		#self.a.hasFocus()
		webbrowser.open("https://www.gpvdm.com")
		
	def callback_help(self, widget, data=None):
		help_window().toggle_visible()
