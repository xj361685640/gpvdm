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
from window_list import windows
from cal_path import get_image_file_path

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget
from PyQt5.QtGui import QPainter,QIcon

#python modules
import webbrowser

#windows
from tab import tab_class
from tab_lang import language_tab_class

from PyQt5.QtCore import pyqtSignal
from global_objects import global_object_run

articles = []
mesh_articles = []

class class_config_window(QWidget):

	changed = pyqtSignal()
	
	def callback_tab_changed(self):
		self.changed.emit()
		global_object_run("ribbon_configure_dump_refresh")

	def init(self):
		self.setFixedSize(900, 600)
		self.setWindowIcon(QIcon(os.path.join(get_image_file_path(),"cog.png")))

		self.setWindowTitle(_("Configure")+" (https://www.gpvdm.com)") 
		

		self.main_vbox = QVBoxLayout()

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))

		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		toolbar.addWidget(spacer)


		self.undo = QAction(QIcon(os.path.join(get_image_file_path(),"help.png")), _("Help"), self)
		self.undo.setStatusTip(_("Help"))
		self.undo.triggered.connect(self.callback_help)
		toolbar.addAction(self.undo)

		self.main_vbox.addWidget(toolbar)

		

		self.notebook = QTabWidget()

		self.notebook.setMovable(True)

		self.main_vbox.addWidget(self.notebook)

		files=["math.inp","dump.inp","thermal.inp","led.inp","config.inp","server.inp"]
		description=[_("Solver configuration"),_("Output files"),_("Thermal"),_("LED"),_("GUI configuration"),_("Server configuration")]

		for i in range(0,len(files)):
			tab=tab_class()
			tab.init(files[i],description[i])
			self.notebook.addTab(tab,description[i])
			if files[i]=="dump.inp":
				tab.changed.connect(self.callback_tab_changed)

		lang_tab=language_tab_class()
		self.notebook.addTab(lang_tab,_("Language"))

		self.setLayout(self.main_vbox)
		self.win_list=windows()
		self.win_list.load()
		self.win_list.set_window(self,"config_window")

		#self.connect("delete-event", self.callback_close_window) 

		#self.hide()

	def callback_help(self,widget):
		webbrowser.open('http://www.gpvdm.com/man/index.html')

	def closeEvent(self, event):
		self.win_list.update(self,"config_window")
		#self.hide()



