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
import shutil
from window_list import windows
import webbrowser
from code_ctrl import enable_betafeatures
from util_zip import zip_lsdir
from util import strextract_interger
from global_objects import global_object_get
from cal_path import get_image_file_path
from global_objects import global_object_register
from server import server_get

import i18n
_ = i18n.language.gettext

#inp
from inp import inp_isfile
from inp import inp_copy_file
from inp import inp_remove_file
from inp import inp_update_token_value

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QMenuBar,QStatusBar, QMenu, QTableWidget, QAbstractItemView
from PyQt5.QtGui import QPainter,QIcon,QCursor

#windows
from gui_util import dlg_get_text
from gui_util import yes_no_dlg

from fit_tab import fit_tab
from QHTabBar import QHTabBar

def fit_new_filename():
	for i in range(0,20):
		pulse_name="fit"+str(i)+".inp"
		if inp_isfile(pulse_name)==False:
			return i
	return -1

class fit_window(QWidget):

	def update(self):
		for item in self.notebook.get_children():
			item.update()

	def get_main_menu(self, window):
		accel_group = gtk.AccelGroup()
		item_factory = gtk.ItemFactory(gtk.MenuBar, "<main>", accel_group)

		item_factory.create_items(self.menu_items)
		if enable_betafeatures()==False:
			item_factory.delete_item(_("/Advanced"))

		window.add_accel_group(accel_group)

		self.item_factory = item_factory

		return item_factory.get_widget("<main>")

	def callback_close(self, widget, data=None):
		self.win_list.update(self,"fit_window")
		self.hide()
		return True

	def callback_help(self, widget, data=None):
		webbrowser.open('http://www.gpvdm.com/man/index.html')

	def callback_add_page(self, widget, data=None):
		new_sim_name=dlg_get_text( _("New fit name:"), _("fit ")+str(len(self.notebook.get_children())+1),image_name="new.png")

		if new_sim_name!=None:
			index=fit_new_filename()
			shutil.copy("fit0.inp","fit"+str(index)+".inp")
			shutil.copy("fit_data0.inp","fit_data"+str(index)+".inp")
			shutil.copy("fit_patch0.inp","fit_patch"+str(index)+".inp")
			inp_update_token_value("fit"+str(index)+".inp", "#fit_name", new_sim_name,1)
			self.add_page(index)

	def callback_remove_page(self,widget,name):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		self.toggle_tab_visible(tab.tab_name)

	def callback_copy_page(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		old_index=tab.index
		new_sim_name=dlg_get_text( _("Clone the current fit to a new fit called:"), tab.tab_name,image_name="clone.png")
		if new_sim_name!=None:
			new_sim_name=new_sim_name
			index=fit_new_filename()

			shutil.copy("fit"+str(old_index)+".inp","fit"+str(index)+".inp")
			shutil.copy("fit_data"+str(old_index)+".inp","fit_data"+str(index)+".inp")
			shutil.copy("fit_patch"+str(old_index)+".inp","fit_patch"+str(index)+".inp")

			inp_update_token_value("fit"+str(index)+".inp", "#fit_name", new_sim_name,1)
			self.add_page(index)


	def remove_invalid(self,input_name):
		return input_name.replace (" ", "_")

	def callback_import(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		tab.import_data()
			
	def callback_rename_page(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		new_sim_name=dlg_get_text( _("Rename the fit to be called:"), tab.tab_name,image_name="rename.png")

		if new_sim_name!=None:
			#new_sim_name=self.remove_invalid(new_sim_name)
			#new_dir=os.path.join(self.sim_dir,new_sim_name)
			#shutil.move(old_dir, new_dir)
			tab.rename(new_sim_name)
			#edit


	def callback_delete_page(self):
		tab = self.notebook.currentWidget()
		response=yes_no_dlg(self, _("Should I remove the fit file ")+tab.tab_name.split("@")[0])

		if response==True:
			inp_remove_file("fit"+str(tab.index)+".inp")
			inp_remove_file("fit_data"+str(tab.index)+".inp")
			inp_remove_file("fit_patch"+str(tab.index)+".inp")
			self.notebook.remove_page(pageNum)
			global_object_get("tb_item_sim_mode_update")()

	def callback_view_toggle_tab(self):
		print("add code")
		#self.toggle_tab_visible(data)

	def load_tabs(self):

		file_list=zip_lsdir(os.path.join(os.getcwd(),"sim.gpvdm"))
		files=[]
		for i in range(0,len(file_list)):
			if file_list[i].startswith("fit") and file_list[i].endswith(".inp"):
				num=file_list[i][3:-4]
				if num.isdigit()==True:
					files.append(file_list[i])

		print("load tabs",files)

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
		new_tab=fit_tab(index)
		self.notebook.addTab(new_tab,new_tab.tab_name)

	def switch_page(self,page, page_num, user_param1):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		self.status_bar.push(self.context_id, tab.tab_name)

	def rod(self):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		tab.update()

	def callback_one_fit(self, widget, data=None):
		my_server=server_get()
		my_server.clear_cache()
		my_server.add_job(os.getcwd(),"--1fit")
		my_server.set_callback_when_done(self.rod)
		my_server.start()

	def callback_do_fit(self, widget, data=None):
		my_server=server_get()
		my_server.clear_cache()
		my_server.add_job(os.getcwd(),"--fit")
		my_server.start()


	def init(self):
		QWidget.__init__(self)

		self.win_list=windows()
		self.win_list.load()
		self.win_list.set_window(self,"fit_window")

		self.main_vbox = QVBoxLayout()

		menubar = QMenuBar()

		file_menu = menubar.addMenu('&File')
		self.menu_close=file_menu.addAction(_("Close"))
		self.menu_close.triggered.connect(self.callback_close)


		self.menu_fit=menubar.addMenu(_("Fits"))
		self.menu_fit_new=self.menu_fit.addAction(_("&New"))
		self.menu_fit_new.triggered.connect(self.callback_add_page)

		self.menu_fit_delete=self.menu_fit.addAction(_("&Delete fit"))
		self.menu_fit_delete.triggered.connect(self.callback_delete_page)

		self.menu_fit_rename=self.menu_fit.addAction(_("&Rename fit"))
		self.menu_fit_rename.triggered.connect(self.callback_rename_page)

		self.menu_fit_import=self.menu_fit.addAction(_("&Import data"))
		self.menu_fit_import.triggered.connect(self.callback_import)

		self.menu_fit_clone=self.menu_fit.addAction(_("&Clone fit"))
		self.menu_fit_clone.triggered.connect(self.callback_copy_page)


		self.menu_help=menubar.addMenu(_("Help"))
		self.menu_help_help=self.menu_help.addAction(_("Help"))
		self.menu_help_help.triggered.connect(self.callback_help)


		self.main_vbox.addWidget(menubar)


		#self.setFixedSize(900, 700)
		self.setWindowTitle(_("Fit window - gpvdm"))   
		self.setWindowIcon(QIcon(os.path.join(get_image_file_path(),"fit.png")))

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

		self.import_data= QAction(QIcon(os.path.join(get_image_file_path(),"import.png")), _("Import data"), self)
		self.import_data.triggered.connect(self.callback_import)
		toolbar.addAction(self.import_data)

		self.play= QAction(QIcon(os.path.join(get_image_file_path(),"play.png")), _("Run a single fit"), self)
		self.play.triggered.connect(self.callback_import)
		toolbar.addAction(self.play)
		
		self.play= QAction(QIcon(os.path.join(get_image_file_path(),"forward.png")), _("Start the fitting process"), self)
		self.play.triggered.connect(self.callback_do_fit)
		toolbar.addAction(self.play)
		
		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		toolbar.addWidget(spacer)


		self.help = QAction(QIcon(os.path.join(get_image_file_path(),"help.png")), 'Help', self)
		self.help.triggered.connect(self.callback_help)
		toolbar.addAction(self.help)

		self.main_vbox.addWidget(toolbar)

		self.notebook = QTabWidget()
		self.notebook.setTabBar(QHTabBar())
		self.notebook.setTabPosition(QTabWidget.West)

		self.notebook.setMovable(True)

		self.load_tabs()

		self.main_vbox.addWidget(self.notebook)
		
		self.status_bar=QStatusBar()
		self.main_vbox.addWidget(self.status_bar)

		self.setLayout(self.main_vbox)

		return






