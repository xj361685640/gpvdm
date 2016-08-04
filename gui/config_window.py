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



import os
from tab import tab_class
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

articles = []
mesh_articles = []

class class_config_window(QWidget):


	def init(self):
		self.setFixedSize(900, 600)
		self.setWindowIcon(QIcon(os.path.join(get_image_file_path(),"cog.png")))

		self.setWindowTitle(_("Configure (www.gpvdm.com)")) 
		

		self.main_vbox = QVBoxLayout()

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))

		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		toolbar.addWidget(spacer)


		self.undo = QAction(QIcon(os.path.join(get_image_file_path(),"help.png")), 'Hide', self)
		self.undo.setStatusTip(_("Close"))
		self.undo.triggered.connect(self.callback_help)
		toolbar.addAction(self.undo)

		#self.status_bar = gtk.Statusbar()
		#self.context_id = self.status_bar.get_context_id("Statusbar example")
		#self.status_bar.show()
		#self.tooltips.set_tip(self.qe_button, "Quantum efficiency")


		self.main_vbox.addWidget(toolbar)

		

		self.notebook = QTabWidget()

		self.notebook.setTabsClosable(True)
		self.notebook.setMovable(True)

		self.main_vbox.addWidget(self.notebook)

		files=["math.inp","dump.inp","thermal.inp"]
		description=["Math","Dump","Thermal"]

		for i in range(0,len(files)):
			widget	= QWidget()
			tab=tab_class()
			tab.init(files[i],description[i])
			widget.setLayout(tab)
			self.notebook.addTab(widget,description[i])


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



