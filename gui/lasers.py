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
#from about import about_dialog_show
from window_list import windows
import webbrowser
from inp import inp_update
from inp import inp_update_token_value
from inp import inp_get_token_value
from tab import tab_class
from util_zip import zip_lsdir
from inp import inp_isfile
from inp import inp_copy_file
from inp import inp_remove_file
from util import strextract_interger
from cal_path import get_image_file_path

import i18n
_ = i18n.language.gettext

#qt
from PyQt5.QtWidgets import QMainWindow, QTextEdit, QAction, QApplication
from PyQt5.QtGui import QIcon, QPainter, QFont, QColor
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QSizePolicy,QVBoxLayout,QPushButton,QDialog,QFileDialog,QToolBar,QLabel,QComboBox, QTabWidget,QStatusBar,QMenuBar, QTabBar, QStylePainter, QStyleOptionTab,QStyle

#window
from gui_util import yes_no_dlg
from gui_util import dlg_get_text
 
def laser_new_filename():
	for i in range(0,20):
		pulse_name="laser"+str(i)+".inp"
		if inp_isfile(pulse_name)==False:
			return i
	return -1



class lasers(QWidget):

	def update(self):
		for item in self.notebook.get_children():
			item.update()

	def get_main_menu(self, window):
		accel_group = gtk.AccelGroup()
		item_factory = gtk.ItemFactory(gtk.MenuBar, "<main>", accel_group)

		item_factory.create_items(self.menu_items)

		window.add_accel_group(accel_group)

		self.item_factory = item_factory

		return item_factory.get_widget("<main>")

	def callback_close(self, widget, data=None):
		self.win_list.update(self,"lasers_window")
		self.hide()
		return True

	def callback_help(self, widget, data=None):
		webbrowser.open('http://www.gpvdm.com/man/index.html')

	def callback_add_page(self):
		new_sim_name=dlg_get_text( _("New laser name:"), _("laser ")+str(self.notebook.count()),"new.png")
		if new_sim_name.ret!=None:
			index=laser_new_filename()
			inp_copy_file("laser"+str(index)+".inp","laser0.inp")
			inp_update_token_value("laser"+str(index)+".inp", "#laser_name", new_sim_name.ret,1)
			self.add_page(index)


	def callback_copy_page(self):
		tab = self.notebook.currentWidget()
		new_sim_name=dlg_get_text(_("Clone the current laser to a new laser called:"), "clone.png")
		print new_sim_name
		return
		#dlg_get_text( _("Clone the current laser to a new laser called:"), tab.tab_name,image_name="clone.png")
		if new_sim_name!=None:
			index=laser_new_filename()
			if inp_copy_file("laser"+str(index)+".inp",tab.file_name)==False:
				print "Error copying file"+tab.file_name
				return

			inp_update_token_value("laser"+str(index)+".inp", "#laser_name", new_sim_name,1)
			self.add_page(index)


	def remove_invalid(self,input_name):
		return input_name.replace (" ", "_")

	def callback_rename_page(self):
		tab = self.notebook.currentWidget()
		new_laser_name=dlg_get_text( _("Rename the laser to be called:"), tab.tab_name ,"rename.png")

		if new_laser_name.ret!=None:
			print "calling rename"
			index=self.notebook.currentIndex() 
			self.notebook.setTabText(index, new_laser_name.ret);
			inp_update(tab.file_name, "#laser_name", new_laser_name.ret)

	def callback_delete_page(self):
		tab = self.notebook.currentWidget()
		response=yes_no_dlg(self,_("Should I remove the laser file ")+tab.tab_name)

		if response == True:
			inp_remove_file(tab.file_name)
			index=self.notebook.currentIndex() 
			self.notebook.removeTab(index)


	def load_tabs(self):

		file_list=zip_lsdir(os.path.join(os.getcwd(),"sim.gpvdm"))
		files=[]
		for i in range(0,len(file_list)):
			if file_list[i].startswith("laser") and file_list[i].endswith(".inp"):
				files.append(file_list[i])

		print "load tabs",files

		for i in range(0,len(files)):
			value=strextract_interger(files[i])
			if value!=-1:
				self.add_page(value)

	def clear_pages(self):
		for items in self.tab_menu.get_children():
			self.tab_menu.remove(items)

		for child in self.notebook.get_children():
    			self.notebook.remove(child)


	def add_page(self,index):
		file_name="laser"+str(index)+".inp"
		laser_name=inp_get_token_value(file_name, "#laser_name")
		tab=tab_class()
		tab.init(file_name,laser_name)
		self.notebook.addTab(tab,laser_name)




	def switch_page(self,page, page_num, user_param1):
		pageNum = self.notebook.get_current_page()
#		tab = self.notebook.get_nth_page(pageNum)
		self.status_bar.push(self.context_id, "Laser "+str(pageNum))

	def __init__(self):
		QWidget.__init__(self)

		self.win_list=windows()
		self.win_list.load()
		self.win_list.set_window(self,"laser_window")

		self.main_vbox = QVBoxLayout()

		menubar = QMenuBar()

		file_menu = menubar.addMenu('&File')
		self.menu_close=file_menu.addAction(_("Close"))
		self.menu_close.triggered.connect(self.callback_close)


		self.menu_lasers=menubar.addMenu(_("Lasers"))
		self.menu_lasers_new=self.menu_lasers.addAction(_("&New"))
		self.menu_lasers_new.triggered.connect(self.callback_add_page)

		self.menu_lasers_delete=self.menu_lasers.addAction(_("&Delete laser"))
		self.menu_lasers_delete.triggered.connect(self.callback_delete_page)

		self.menu_lasers_rename=self.menu_lasers.addAction(_("&Rename laser"))
		self.menu_lasers_rename.triggered.connect(self.callback_rename_page)

		self.menu_lasers_rename=self.menu_lasers.addAction(_("&Rename laser"))
		self.menu_lasers_rename.triggered.connect(self.callback_rename_page)

		self.menu_lasers_clone=self.menu_lasers.addAction(_("&Clone laser"))
		self.menu_lasers_clone.triggered.connect(self.callback_copy_page)


		self.menu_help=menubar.addMenu(_("Help"))
		self.menu_help_help=self.menu_help.addAction(_("Help"))
		self.menu_help_help.triggered.connect(self.callback_help)


		self.main_vbox.addWidget(menubar)

		self.setFixedSize(900, 500)
		self.setWindowTitle(_("Laser configuration window - gpvdm"))   
		self.setWindowIcon(QIcon(os.path.join(get_image_file_path(),"lasers.png")))

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))

		self.new = QAction(QIcon(os.path.join(get_image_file_path(),"new.png")), _("New laser"), self)
		self.new.triggered.connect(self.callback_add_page)
		toolbar.addAction(self.new)

		self.new = QAction(QIcon(os.path.join(get_image_file_path(),"delete.png")), _("Delete laser"), self)
		self.new.triggered.connect(self.callback_delete_page)
		toolbar.addAction(self.new)

		self.clone = QAction(QIcon(os.path.join(get_image_file_path(),"clone.png")), _("Clone laser"), self)
		self.clone.triggered.connect(self.callback_copy_page)
		toolbar.addAction(self.clone)

		self.clone = QAction(QIcon(os.path.join(get_image_file_path(),"rename.png")), _("Rename laser"), self)
		self.clone.triggered.connect(self.callback_rename_page)
		toolbar.addAction(self.clone)

		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		toolbar.addWidget(spacer)


		self.help = QAction(QIcon(os.path.join(get_image_file_path(),"help.png")), 'Hide', self)
		self.help.setStatusTip(_("Close"))
		self.help.triggered.connect(self.callback_help)
		toolbar.addAction(self.help)

		self.main_vbox.addWidget(toolbar)


		self.notebook = QTabWidget()
		self.notebook.setMovable(True)

		self.load_tabs()

		self.main_vbox.addWidget(self.notebook)


		self.status_bar=QStatusBar()
		self.main_vbox.addWidget(self.status_bar)


		self.setLayout(self.main_vbox)


