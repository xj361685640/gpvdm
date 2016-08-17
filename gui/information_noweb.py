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
from tab_base import tab_base
from help import my_help_class
from cal_path import get_image_file_path

import i18n
_ = i18n.language.gettext

#qt
from PyQt5.QtWidgets import QMainWindow, QTextEdit, QAction, QApplication
from PyQt5.QtGui import QIcon,QPixmap
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QSizePolicy,QHBoxLayout,QPushButton,QDialog,QFileDialog,QToolBar, QMessageBox, QVBoxLayout, QGroupBox, QTableWidget,QAbstractItemView, QTableWidgetItem, QLabel

class information(QWidget,tab_base):

	lines=[]
	edit_list=[]
	file_name=""
	line_number=[]
	save_file_name=""
	name="Welcome"

	def __init__(self):
		QWidget.__init__(self)
		hbox=QHBoxLayout()
		
		self.label = QLabel()
		self.label.setText(_("<b>General-purpose photovoltaic device model</b><br>(<a href=\"http://www.gpvdm.com\" title=\"Click to find out more\">www.gpvdm.com</a>)<br><br> To make a new simulation directory click <i>new</i> in the <i>file</i> menu<br> or to open an existing simulation click on the <i>open</i> button.<br> There is more help on the <a href=\"http://www.gpvdm.com/man/index.html\">man pages</a>.  Please report bugs to\nroderick.mackenzie@nottingham.ac.uk.<br><br> Rod<br>18/10/13<br>~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~<br><br><br><br>"))
		#read_page=False

		hbox.addWidget(self.label)

		image = QLabel()
		pixmap = QPixmap(os.path.join(get_image_file_path(),"image.jpg"))
		image.setPixmap(pixmap)

		hbox.addWidget(image)

		self.setLayout(hbox)

	def update(self):
		print("")
		
	def help(self):
		my_help_class.help_set_help(["icon.png",_("<big><b>Welcome to gpvdm</b></big>\n The window will provide you with information about new versions and bugs in gpvdm.")])


