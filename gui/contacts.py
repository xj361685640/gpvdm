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
from numpy import *
from inp import inp_load_file
import webbrowser
from inp_util import inp_search_token_value
from cal_path import get_image_file_path
from window_list import windows

from epitaxy import epitaxy_get_layers
from epitaxy import epitaxy_get_dos_file
from epitaxy import epitaxy_get_width
from inp import inp_update
from scan_item import scan_item_add

import i18n
_ = i18n.language.gettext


#contacts io
from contacts_io import segment
from contacts_io import contacts_save
from contacts_io import contacts_get_array
from contacts_io import contacts_clear
from contacts_io import contacts_print
from contacts_io import contacts_load
from contacts_io import contacts_print
from contacts_io import contacts_append

#qt
from PyQt5.QtWidgets import QMainWindow, QTextEdit, QAction, QApplication
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QSizePolicy,QHBoxLayout,QPushButton,QDialog,QFileDialog,QToolBar, QMessageBox, QVBoxLayout, QGroupBox, QTableWidget,QAbstractItemView, QTableWidgetItem, QComboBox

from PyQt5.QtCore import pyqtSignal

from gui_util import tab_get_value
from gui_util import tab_set_value
from gui_util import tab_insert_row
from gui_util import tab_remove

from util import str2bool

class contacts_window(QWidget):

	visible=1
	
	changed = pyqtSignal()
	
	def update_contact_db(self):
		contacts_clear()
		for i in range(0,self.tab.rowCount()):
			contacts_append(float(tab_get_value(self.tab,i, 0)),float(tab_get_value(self.tab,i, 2)),float(tab_get_value(self.tab,i, 3)),float(tab_get_value(self.tab,i, 1)),str2bool(tab_get_value(self.tab,i, 4)))	

	def add_row(self,pos,start,width,depth,voltage,active):
		pos= tab_insert_row(self.tab)

		self.tab.setItem(pos,0,QTableWidgetItem(start))
		self.tab.setItem(pos,1,QTableWidgetItem(width))
		self.tab.setItem(pos,2,QTableWidgetItem(depth))
		self.tab.setItem(pos,3,QTableWidgetItem(voltage))

		combobox = QComboBox()
		combobox.addItem(_("true"))
		combobox.addItem(_("false"))

		self.tab.setCellWidget(pos,4, combobox)
		combobox.setCurrentIndex(combobox.findText(active.lower()))
		combobox.currentIndexChanged.connect(self.save)
		
	def on_add_clicked(self, button):
		self.tab.blockSignals(True)
		index = self.tab.selectionModel().selectedRows()

		if len(index)>0:
			pos=index[0].row()+1
		else:
			pos = self.tab.rowCount()

		self.add_row(pos,_("start"),_("width"),_("depth"),_("voltage"),_("false"))
 
		self.save()
		self.tab.blockSignals(False)

	def on_remove_clicked(self, button):
		tab_remove(self.tab)
		self.save()

	def save(self):
		self.update_contact_db()
		contacts_save()
		self.changed.emit()

	def callback_close(self, widget, data=None):
		self.win_list.update(self,"contact")
		self.hide()
		return True

	def callback_help(self):
		webbrowser.open('http://www.gpvdm.com/man/index.html')

	def tab_changed(self, x,y):
		self.save()

	def load(self):
		self.tab.clear()
		self.tab.setHorizontalHeaderLabels([_("Start"), _("Width"),_("Depth"),_("Voltage"),_("Active contact")])
		contacts_load()
		#contacts_print()
		contacts=contacts_get_array()
		i=0
		for c in contacts_get_array():
			self.add_row(i,str(c.start),str(c.width),str(c.depth),str(c.voltage),str(c.active))

			i=i+1


	def __init__(self):
		QWidget.__init__(self)
		self.setFixedSize(600, 400)

		self.win_list=windows()
		self.win_list.set_window(self,"contacts")

		self.setWindowIcon(QIcon(os.path.join(get_image_file_path(),"contact.png")))

		self.setWindowTitle(_("Edit contacts (www.gpvdm.com)")) 
		
		self.main_vbox = QVBoxLayout()

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))

		add = QAction(QIcon(os.path.join(get_image_file_path(),"add.png")),  _("Add contact"), self)
		add.triggered.connect(self.on_add_clicked)
		toolbar.addAction(add)

		remove = QAction(QIcon(os.path.join(get_image_file_path(),"minus.png")),  _("Remove contacts"), self)
		remove.triggered.connect(self.on_remove_clicked)
		toolbar.addAction(remove)

		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		toolbar.addWidget(spacer)


		self.help = QAction(QIcon(os.path.join(get_image_file_path(),"help.png")), 'Hide', self)
		self.help.setStatusTip(_("Close"))
		self.help.triggered.connect(self.callback_help)
		toolbar.addAction(self.help)

		self.main_vbox.addWidget(toolbar)

		self.tab = QTableWidget()
		self.tab.resizeColumnsToContents()

		self.tab.verticalHeader().setVisible(False)

		self.tab.clear()
		self.tab.setColumnCount(5)
		self.tab.setSelectionBehavior(QAbstractItemView.SelectRows)

		self.load()

		self.tab.cellChanged.connect(self.tab_changed)

		self.main_vbox.addWidget(self.tab)


		self.setLayout(self.main_vbox)

