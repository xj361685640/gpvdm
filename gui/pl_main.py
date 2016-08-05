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
from epitaxy import epitaxy_get_pl_file
from global_objects import global_object_register
from epitaxy import epitaxy_get_name

#qt5
from PyQt5.QtWidgets import QMainWindow, QTextEdit, QAction, QApplication
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QSizePolicy,QVBoxLayout,QPushButton,QDialog,QFileDialog,QToolBar,QMessageBox,QTabWidget
from about import about_dlg

class pl_main(QWidget,tab_base):

	lines=[]
	edit_list=[]
	file_name=""
	line_number=[]
	save_file_name=""
	name="Luminescence"


	def __init__(self):
		QWidget.__init__(self)
		self.main_vbox = QVBoxLayout()
		self.notebook = QTabWidget()

		self.main_vbox.addWidget(self.notebook)
		self.setLayout(self.main_vbox)

		self.notebook.setTabsClosable(True)
		self.notebook.setMovable(True)
		self.notebook.setTabPosition(QTabWidget.West)


	def update(self):
		self.notebook.clear()

		files=epitaxy_get_dos_files()
		for i in range(0,epitaxy_get_layers()):
			pl_file=epitaxy_get_pl_file(i)
			if pl_file.startswith("pl")==True:
				widget	= QWidget()
 
				name="Luminescence of "+epitaxy_get_name(i)
				print pl_file,files

				tab=tab_class()
				tab.init(pl_file+".inp",name)
				widget.setLayout(tab)

				self.notebook.addTab(widget,name)


	def help(self):
		help_window().help_set_help(["tab.png","<big><b>Luminescence</b></big>\nIf you set 'Turn on luminescence' to true, the simulation will assume recombination is a raditave process and intergrate it to produce Voltage-Light intensity curves (lv.dat).  Each number in the tab tells the model how efficient each recombination mechanism is at producing photons."])

#gobject.type_register(dos_main)
#gobject.signal_new("update", dos_main, gobject.SIGNAL_RUN_FIRST,gobject.TYPE_NONE, ())

