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

from clone import gpvdm_clone
import os
from import_archive import import_archive
from window_list import windows
from gui_util import save_as_gpvdm

import i18n
_ = i18n.language.gettext

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QSystemTrayIcon,QMenu,QListWidgetItem
from PyQt5.QtGui import QIcon
from PyQt5.uic import loadUi

#calpath
from cal_path import get_device_lib_path
from cal_path import get_image_file_path
from cal_path import get_ui_path
from gui_util import error_dlg

class new_simulation():

	# close the window and quit
	def delete_event(self, widget, event, data=None):
		self.win_list.update(self.window,"new_simulation")
		gtk.main_quit()
		return False


	def callback_close(self, widget, data=None):
		self.win_list.update(self.window,"new_simulation")
		self.window.reject()


	def callback_next(self):
		if len(self.window.listwidget.selectedItems())>0:

			file_path=save_as_gpvdm(self.window)

			selection=self.window.listwidget.selectedItems()[0].text()
			selection_file=selection[selection.find("(")+1:selection.find(")")]

			if not os.path.exists(file_path):
				os.makedirs(file_path)

			self.ret_path=file_path
			os.chdir(self.ret_path)
			gpvdm_clone(os.getcwd(),True)
			import_archive(os.path.join(get_device_lib_path(),selection_file),os.path.join(os.getcwd(),"sim.gpvdm"),False)
			self.window.close()
		else:
			error_dlg(self.window,_("Please select a device before clicking next"))


	def get_return_path(self):
		return self.ret_path

	def __init__(self):
		self.ret_path=None
		# Create a new window

		self.window = loadUi(os.path.join(get_ui_path(),"new.ui"))

		self.win_list=windows()
		self.win_list.load()
		self.win_list.set_window(self.window,"new_simulation")

		self.window.listwidget.setIconSize(QSize(64,64))
		self.window.listwidget.clear()

		itm = QListWidgetItem( "Organic solar cell (p3htpcbm.gpvdm)" )
		itm.setIcon(QIcon(os.path.join(get_image_file_path(),"icon.svg")))
		self.window.listwidget.addItem(itm)

		itm = QListWidgetItem( "Organic LED (oled.gpvdm)" )
		itm.setIcon(QIcon(os.path.join(get_image_file_path(),"oled.svg")))
		self.window.listwidget.addItem(itm)

		itm = QListWidgetItem( "Crystalline silicon solar cell new/beta (silicon.gpvdm)" )
		itm.setIcon(QIcon(os.path.join(get_image_file_path(),"si.svg")))
		self.window.listwidget.addItem(itm)

		itm = QListWidgetItem( "CIGS Solar cell new/beta (cigs.gpvdm)" )
		itm.setIcon(QIcon(os.path.join(get_image_file_path(),"si.svg")))
		self.window.listwidget.addItem(itm)

		itm = QListWidgetItem( "a-Si solar cell new/beta (a-silicon.gpvdm)" )
		itm.setIcon(QIcon(os.path.join(get_image_file_path(),"asi.svg")))
		self.window.listwidget.addItem(itm)

		itm = QListWidgetItem( "polycrystalline silicon (new/beta) (silicon.gpvdm)" )
		itm.setIcon(QIcon(os.path.join(get_image_file_path(),"psi.svg")))
		self.window.listwidget.addItem(itm)

		self.window.next.clicked.connect(self.callback_next)
		self.window.cancel.clicked.connect(self.callback_close)



