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

class new_simulation():

	# close the window and quit
	def delete_event(self, widget, event, data=None):
		self.win_list.update(self.window,"new_simulation")
		gtk.main_quit()
		return False


	def callback_close(self, widget, data=None):
		self.win_list.update(self.window,"new_simulation")
		self.window.reject()


	def callback_next(self, widget, data=None):
		selection = self.treeview.get_selection()
		model, iter = selection.get_selected()

		if iter:
			path = model.get_path(iter)[0]
			print path
			print

		dialog = gtk.FileChooserDialog(_("Make new simulation directory"),
                               None,
                               gtk.FILE_CHOOSER_ACTION_OPEN,
                               (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                                gtk.STOCK_NEW, gtk.RESPONSE_OK))

		dialog.set_default_response(gtk.RESPONSE_OK)
		dialog.set_action(gtk.FILE_CHOOSER_ACTION_CREATE_FOLDER)

		filter = gtk.FileFilter()
		filter.set_name(_("All files"))
		filter.add_pattern("*")
		dialog.add_filter(filter)

		response = dialog.run()
		if response == gtk.RESPONSE_OK:
			if not os.path.exists(dialog.get_filename()):
				os.makedirs(dialog.get_filename())

			self.ret_path=dialog.get_filename()
			os.chdir(self.ret_path)
			gpvdm_clone(os.getcwd(),True)
			import_archive(os.path.join(get_device_lib_path(),self.liststore[path][2]),os.path.join(os.getcwd(),"sim.gpvdm"),False)
			self.response(True)
			#self.change_dir_and_refresh_interface(dialog.get_filename())
			print _("OK")

		elif response == gtk.RESPONSE_CANCEL:
			print _("Closed, no dir selected")

		dialog.destroy()

		self.window.accept()

	def get_return_path(self):
		return self.ret_path

	def __init__(self):
		self.ret_path=""
		# Create a new window

		self.window = loadUi('./gui/new.ui')

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



