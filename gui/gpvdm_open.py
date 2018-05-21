#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2917 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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
from util import gpvdm_delete_file
#from global_objects import global_object_get
from plot_io import get_plot_file_info
from dat_file_class import dat_file

#qt
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import QSize, Qt, QTimer
from PyQt5.uic import loadUi
from PyQt5.QtWidgets import QMenu,QAbstractItemView,QApplication,QDialog,QGraphicsScene,QListWidgetItem,QPushButton,QListView,QVBoxLayout,QDialog,QWidget,QListWidget,QHBoxLayout,QLineEdit
from PyQt5.QtGui import QPixmap

#cal_path
from icon_lib import QIcon_load
from cal_path import get_ui_path

from help import help_window

from error_dlg import error_dlg

from ref import get_ref_text
from gui_util import dlg_get_text
from gui_util import yes_no_dlg

from clone import clone_material
from clone import clone_spectra
from cal_path import get_base_material_path
from cal_path import get_base_spectra_path

from inp import inp_get_token_value
from util import isfiletype
from win_lin import desktop_open
from gpvdm_viewer import gpvdm_viewer

COL_PATH = 0
COL_PIXBUF = 1
COL_IS_DIRECTORY = 2

import i18n
_ = i18n.language.gettext

#util
from util import latex_to_html

class gpvdm_open(QDialog):

	def __init__(self,path,show_inp_files=True):
		QWidget.__init__(self)
		self.menu_new_material_enabled=False
		self.menu_new_spectra_enabled=False
		self.show_inp_files=show_inp_files
		self.show_directories=True
		self.file_path=""
		self.vbox=QVBoxLayout()
		self.setLayout(self.vbox)
		self.top_h_widget=QWidget()
		self.top_h_widget.setStyleSheet("margin: 0; padding: 0; ")
		self.top_hbox=QHBoxLayout()
		self.top_h_widget.setLayout(self.top_hbox)
		self.top_h_widget.setMaximumHeight(50)
		self.up=QPushButton()
		self.home=QPushButton()	
		self.resize(800,500)
		self.path=QLineEdit()
		self.path.setMinimumHeight(30)
		self.path.setStyleSheet("padding: 0; ")
		self.top_hbox.addWidget(self.up)
		self.top_hbox.addWidget(self.home)
		self.top_hbox.addWidget(self.path)
		self.setWindowTitle(_("Open file")+" https://www.gpvdm.com")
		self.setWindowIcon(QIcon_load("folder"))
#		self.listwidget=QListWidget()
#		self.listwidget.setSelectionMode(QAbstractItemView.ExtendedSelection)
#		self.listwidget.setStyleSheet("margin: 0; padding: 0; ")
		self.vbox.addWidget(self.top_h_widget)
		
		self.viewer=gpvdm_viewer(path)
		self.viewer.set_directory_view(True)
		self.vbox.addWidget(self.viewer)
		
	
		self.up.setFixedSize(42,42)
		self.up.setStyleSheet("margin: 0; padding: 0; border: none;")
		self.home.setFixedSize(42,42)
		self.home.setStyleSheet("margin: 0; padding: 0; border: none ;")
		#self.window.center()

		self.up.setIcon(QIcon_load("go-up"))
		self.up.clicked.connect(self.on_up_clicked)


		self.home.setIcon(QIcon_load("user-home"))
		self.home.clicked.connect(self.on_home_clicked)


		self.dir = path
		self.root_dir= path

		self.path.setText(path)

		self.viewer.path_changed.connect(self.change_path)
		self.change_path()
		self.show()


	def get_icon(self, name):
		return QIcon_load(name+"_file")


	def get_filename(self):
		return self.file_path


	def on_home_clicked(self, widget):
		self.dir = self.root_dir
		self.change_path()
		

	def change_path(self):
		self.path.setText(self.viewer.path)

		self.viewer.fill_store()
		sensitive = True
		#print(self.dir,self.root_dir)
		print(self.viewer.path,self.root_dir)
		if self.viewer.path == self.root_dir:
			sensitive = False

		self.up.setEnabled(sensitive)

	def on_up_clicked(self, widget):
		self.viewer.set_path(os.path.dirname(self.viewer.path))
		self.change_path()
		self.viewer.fill_store()

