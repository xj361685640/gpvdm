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
from open_save_dlg import save_as_gpvdm

import i18n
_ = i18n.language.gettext

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QPushButton,QCheckBox,QHBoxLayout,QLabel,QWidget,QDialog,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QSystemTrayIcon,QMenu,QListWidget,QListWidgetItem
from PyQt5.QtGui import QIcon
from PyQt5.uic import loadUi

#calpath
from cal_path import get_device_lib_path
from icon_lib import QIcon_load
from cal_path import get_ui_path
from error_dlg import error_dlg
from cal_path import get_exe_path

from help import help_window
from inp import inp_load_file
from inp import inp_get_token_value_from_list
from inp import inp_get_token_value

from util import str2bool
from gui_util import dlg_get_text

from gpvdm_viewer import gpvdm_viewer

from bugs import bugs_add_action
from bugs import bugs_clear

class simulation():
	name=""
	file_name=""
	icon=None
	hidden=False

class new_simulation(QDialog):

	def callback_close(self, widget, data=None):
		self.reject()


	def callback_next(self):
		help_window().help_set_help(["document-save-as.png",_("<big><b>Now save the simulation</b></big><br>Now select where you would like to save the simulation directory.")])

		if len(self.viewer.selectedItems())>0:
			
			password=inp_get_token_value("info.inp", "#info_password",archive=self.viewer.file_path)
			if password!="":
				pw_dlg=dlg_get_text( _("password:"), "","document-new")
				if password!=pw_dlg.ret:
					return

			file_path=save_as_gpvdm(self)
			#print(file_path,get_exe_path())
			if file_path!=None:
				if file_path.startswith(get_exe_path())==True:
					error_dlg(self,_("It's not a good idea to save the simulation in the gpvdm installation directory."))
					return

				if os.path.isdir(file_path)==True:
					error_dlg(self,_("That directory already exists.  Pick another name or delete the old one."))
					return

				if not os.path.exists(file_path):
					os.makedirs(file_path)

				self.ret_path=file_path
				os.chdir(self.ret_path)
				bugs_clear()
				bugs_add_action(os.path.basename(self.viewer.file_path))
				gpvdm_clone(os.getcwd(),copy_dirs=True)
				import_archive(self.viewer.file_path,os.path.join(os.getcwd(),"sim.gpvdm"),False)
				self.close()
		else:
			error_dlg(self,_("Please select a device before clicking next"))


	def get_return_path(self):
		return self.ret_path

		return
		print(_("Organic LED"))
		print(_("Crystalline silicon solar cell"))
		print(_("a-Si solar cell "))
		print(_("polycrystalline silicon "))
		print(_("OFET "))
		print(_("Perovskite solar cell"))
		print(_("CIGS Solar cell"))

	def callback_toggle_hidden(self):
		self.viewer.set_show_hidden(self.show_hidden.isChecked())
		self.viewer.fill_store()
		#print("toggle")
	
	def __init__(self):
		QDialog.__init__(self)
		self.main_vbox=QVBoxLayout()
		self.setFixedSize(450,580) 
		self.setWindowTitle(_("New simulation")+" (https://www.gpvdm.com)")
		self.setWindowIcon(QIcon_load("si"))
		self.title=QLabel("<big><b>"+_("Which type of device would you like to simulate?")+"</b></big>")

		self.viewer=gpvdm_viewer(get_device_lib_path())
		self.viewer.set_back_arrow(True)
		self.viewer.set_enable_menu(False)
		self.main_vbox.addWidget(self.title)
		self.main_vbox.addWidget(self.viewer)

		self.hwidget=QWidget()

		self.nextButton = QPushButton(_("Next"))
		self.cancelButton = QPushButton(_("Cancel"))

		self.files=[]

		hbox = QHBoxLayout()
		self.show_hidden=QCheckBox(_("Show hidden"))
		self.show_hidden.clicked.connect(self.callback_toggle_hidden)
		hbox.addWidget(self.show_hidden)
		hbox.addStretch(1)
		hbox.addWidget(self.cancelButton)
		hbox.addWidget(self.nextButton)
		self.hwidget.setLayout(hbox)

		self.main_vbox.addWidget(self.hwidget)

		self.setLayout(self.main_vbox)
		self.show()

		self.ret_path=None
		# Create a new window

		
		self.viewer.accept.connect(self.callback_next)
		self.nextButton.clicked.connect(self.callback_next)
		self.cancelButton.clicked.connect(self.callback_close)



