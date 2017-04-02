#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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
from open_save_dlg import save_as_gpvdm

import i18n
_ = i18n.language.gettext

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QPushButton,QHBoxLayout,QLabel,QWidget,QDialog,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QSystemTrayIcon,QMenu,QListWidget,QListWidgetItem
from PyQt5.QtGui import QIcon
from PyQt5.uic import loadUi

#calpath
from cal_path import get_device_lib_path
from icon_lib import QIcon_load
from cal_path import get_ui_path
from gui_util import error_dlg
from cal_path import get_exe_path

from help import help_window

class new_simulation(QDialog):

	# close the window and quit
	#def delete_event(self, widget, event, data=None):
	#	self.win_list.update(self.window,"new_simulation")
	#	gtk.main_quit()
	#	return False


	def callback_close(self, widget, data=None):
		#self.win_list.update(self.window,"new_simulation")
		self.reject()


	def callback_next(self):
		help_window().help_set_help(["document-save-as.png",_("<big><b>Now save the simulation</b></big><br>Now select where you would like to save the simulation directory.")])

		if len(self.listwidget.selectedItems())>0:

			file_path=save_as_gpvdm(self)
			print(file_path,get_exe_path())
			if file_path!=None:
				if file_path.startswith(get_exe_path())==True:
					error_dlg(self,_("It's not a good idea to save the simulation in the gpvdm installation directory."))
					return

				selection=self.listwidget.selectedItems()[0].text()
				selection_file=selection[selection.find("(")+1:selection.find(")")]

				if not os.path.exists(file_path):
					os.makedirs(file_path)

				self.ret_path=file_path
				os.chdir(self.ret_path)
				gpvdm_clone(os.getcwd(),True)
				import_archive(os.path.join(get_device_lib_path(),selection_file),os.path.join(os.getcwd(),"sim.gpvdm"),False)
				self.close()
		else:
			error_dlg(self,_("Please select a device before clicking next"))


	def get_return_path(self):
		return self.ret_path

	def __init__(self):
		QDialog.__init__(self)
		self.main_vbox=QVBoxLayout()
		self.setFixedSize(450,580) 
		self.setWindowTitle(_("New simulation")+" (https://www.gpvdm.com)")
		self.setWindowIcon(QIcon_load("si"))
		self.title=QLabel("<big><b>"+_("Which type of device would you like to simulate?")+"</b></big>")

		self.listwidget=QListWidget()
		self.main_vbox.addWidget(self.title)
		self.main_vbox.addWidget(self.listwidget)

		self.hwidget=QWidget()

		self.nextButton = QPushButton(_("Next"))
		self.cancelButton = QPushButton(_("Cancel"))

		hbox = QHBoxLayout()
		hbox.addStretch(1)
		hbox.addWidget(self.cancelButton)
		hbox.addWidget(self.nextButton)
		self.hwidget.setLayout(hbox)

		self.main_vbox.addWidget(self.hwidget)

		self.setLayout(self.main_vbox)
		self.show()
		print(get_exe_path())
		self.ret_path=None
		# Create a new window

		#self.win_list=windows()
		#self.win_list.load()
		#self.win_list.set_window(self,"new_simulation")

		self.listwidget.setIconSize(QSize(64,64))
		self.listwidget.clear()

		itm = QListWidgetItem( _("Organic solar cell")+" (p3htpcbm.gpvdm)" )
		itm.setIcon(QIcon_load("icon"))
		self.listwidget.addItem(itm)

		itm = QListWidgetItem( _("Organic LED")+" (oled.gpvdm)" )
		itm.setIcon(QIcon_load("oled"))
		self.listwidget.addItem(itm)

		itm = QListWidgetItem( _("Crystalline silicon solar cell")+" (silicon.gpvdm)" )
		itm.setIcon(QIcon_load("si"))
		self.listwidget.addItem(itm)

		itm = QListWidgetItem( _("a-Si solar cell ")+" (a-silicon.gpvdm)" )
		itm.setIcon(QIcon_load("asi"))
		self.listwidget.addItem(itm)

		itm = QListWidgetItem( _("polycrystalline silicon ")+" (silicon.gpvdm)" )
		itm.setIcon(QIcon_load("psi"))
		self.listwidget.addItem(itm)

		itm = QListWidgetItem( _("OFET ")+" (ofet.gpvdm)" )
		itm.setIcon(QIcon_load("ofet"))
		self.listwidget.addItem(itm)

		itm = QListWidgetItem( _("Perovskite solar cell")+" (perovskite.gpvdm)" )
		itm.setIcon(QIcon_load("perovskite"))
		self.listwidget.addItem(itm)
		
		itm = QListWidgetItem( _("CIGS Solar cell")+" (cigs.gpvdm)" )
		itm.setIcon(QIcon_load("cigs"))
		self.listwidget.addItem(itm)
		
		self.listwidget.itemDoubleClicked.connect(self.callback_next)
		self.nextButton.clicked.connect(self.callback_next)
		self.cancelButton.clicked.connect(self.callback_close)



