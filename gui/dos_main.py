#!/usr/bin/env python2.7
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


from tab_base import tab_base
from epitaxy import epitaxy_get_dos_files
from tab import tab_class
from epitaxy import epitaxy_get_layers
from epitaxy import epitaxy_get_electrical_layer
from global_objects import global_object_register
from epitaxy import epitaxy_get_name

#qt5
from PyQt5.QtWidgets import QMainWindow, QTextEdit, QAction, QApplication
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QSizePolicy,QVBoxLayout,QPushButton,QDialog,QFileDialog,QToolBar,QMessageBox,QTabWidget
from about import about_dlg

class dos_main(QWidget,tab_base):

	lines=[]
	edit_list=[]
	file_name=""
	line_number=[]
	save_file_name=""
	name="Desnity of states"


	def __init__(self):
		QWidget.__init__(self)
		self.main_vbox = QVBoxLayout()
		self.notebook = QTabWidget()

		self.main_vbox.addWidget(self.notebook)
		self.setLayout(self.main_vbox)

		self.notebook.setTabsClosable(True)
		self.notebook.setMovable(True)
		self.notebook.setTabPosition(QTabWidget.West)

		#global_object_register("dos-update",self.update)

	def update(self):
		print "DoS update"


		self.notebook.clear()

		files=epitaxy_get_dos_files()
		for i in range(0,epitaxy_get_layers()):
			dos_layer=epitaxy_get_electrical_layer(i)
			if dos_layer.startswith("dos")==True:
				#add_to_widget=True
				widget	= QWidget()
 


				name="DoS of "+epitaxy_get_name(i)
				print dos_layer,files

				tab=tab_class()
				tab.init(dos_layer+".inp",name)
				widget.setLayout(tab)

				self.notebook.addTab(widget,name)

	def help(self):
		help_window().help_set_help(["tab.png","<big><b>Density of States</b></big>\nThis tab contains the electrical model parameters, such as mobility, tail slope energy, and band gap."])

#gobject.type_register(dos_main)
#gobject.signal_new("update", dos_main, gobject.SIGNAL_RUN_FIRST,gobject.TYPE_NONE, ())

