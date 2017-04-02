#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2016 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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
from gui_util import dlg_get_text
from window_list import windows
import webbrowser
from inp import inp_update_token_value
from fxexperiment_tab import fxexperiment_tab
from util_zip import zip_lsdir
from inp import inp_isfile
from inp import inp_copy_file
from inp import inp_remove_file
from util import strextract_interger
from global_objects import global_object_get
from icon_lib import QIcon_load
from global_objects import global_object_register
from code_ctrl import enable_betafeatures
from gui_util import yes_no_dlg

import i18n
_ = i18n.language.gettext

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QMenuBar,QStatusBar
from PyQt5.QtGui import QPainter,QIcon

#window
from QHTabBar import QHTabBar
from PyQt5.QtCore import pyqtSignal

def experiment_new_filename():
	for i in range(0,20):
		name="fxdomain"+str(i)+".inp"
		if inp_isfile(name)==False:
			return i
	return -1

class fxexperiment(QWidget):

	changed = pyqtSignal()

	def callback_close(self):
		self.win_list.update(self,"experiment_window")
		self.hide()
		return True

	def callback_help(self):
		webbrowser.open('http://www.gpvdm.com/man/index.html')

	def callback_add_page(self):
		new_sim_name=dlg_get_text( _("New experiment name")+":", _("experiment ")+str(self.notebook.count()+1),"document-new.png")

		new_sim_name=new_sim_name.ret

		if new_sim_name!=None:
			index=experiment_new_filename()
			inp_copy_file("fxdomain"+str(index)+".inp","fxdomain0.inp")
			inp_copy_file("fxmesh"+str(index)+".inp","fxmesh0.inp")
			inp_update_token_value("fxdomain"+str(index)+".inp", "#sim_menu_name", new_sim_name+"@fxdomain",1)
			self.add_page(index)
			self.changed.emit()


	def callback_copy_page(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		old_index=tab.index
		new_sim_name=dlg_get_text( _("Clone the current experiment to a new experiment called")+":", tab.tab_name.split("@")[0],image_name="clone.png")
		if new_sim_name!=None:
			new_sim_name=new_sim_name+"@"+tab.tab_name.split("@")[1]
			index=experiment_new_filename()
			if inp_copy_file("fxdomain"+str(index)+".inp","fxdomain"+str(old_index)+".inp")==False:
				print("Error copying file"+"fxdomain"+str(old_index)+".inp")
				return
			if inp_copy_file("fxmesh"+str(index)+".inp","fxmesh"+str(old_index)+".inp")==False:
				print("Error copying file"+"fxdomain"+str(old_index)+".inp")
				return

			inp_update_token_value("fxdomain"+str(index)+".inp", "#sim_menu_name", new_sim_name,1)
			self.add_page(index)
			self.changed.emit()


	#def callback_run_experiment(self,widget,data):
	#	pageNum = self.notebook.get_current_page()
	#	tab = self.notebook.get_nth_page(pageNum)
	#	tab.simulate(True,True)


	def remove_invalid(self,input_name):
		return input_name.replace (" ", "_")

	def callback_rename_page(self):
		tab = self.notebook.currentWidget()

		new_sim_name=dlg_get_text( _("Rename the experiment to be called")+":", tab.tab_name.split("@")[0],"rename.png")

		new_sim_name=new_sim_name.ret

		if new_sim_name!=None:
			tab.rename(new_sim_name)
			index=self.notebook.currentIndex() 
			self.notebook.setTabText(index, new_sim_name)
			self.changed.emit()


	def callback_delete_page(self):
		tab = self.notebook.currentWidget()

		response=yes_no_dlg(self,_("Should I remove the experiment file")+" "+tab.tab_name.split("@")[0])

		if response == True:
			inp_remove_file("fxdomain"+str(tab.index)+".inp")
			inp_remove_file("fxmesh"+str(tab.index)+".inp")
			index=self.notebook.currentIndex() 
			self.notebook.removeTab(index)
			self.changed.emit()


	def load_tabs(self):

		file_list=zip_lsdir(os.path.join(os.getcwd(),"sim.gpvdm"))
		files=[]
		for i in range(0,len(file_list)):
			if file_list[i].startswith("fxdomain") and file_list[i].endswith(".inp"):
				files.append(file_list[i])

		print("load tabs",files)

		for i in range(0,len(files)):
			value=strextract_interger(files[i])
			if value!=-1:
				self.add_page(value)

	def clear_pages(self):
		self.notebook.clear()

	def add_page(self,index):
		tab=fxexperiment_tab()
		tab.init(index)
		self.notebook.addTab(tab,tab.tab_name.split("@")[0])

	def __init__(self):
		QWidget.__init__(self)
		self.setMinimumSize(1200, 700)

		self.win_list=windows()
		self.win_list.load()
		self.win_list.set_window(self,"fxexperiments_window")

		self.main_vbox = QVBoxLayout()

		menubar = QMenuBar()

		file_menu = menubar.addMenu("&"+_("File"))
		self.menu_close=file_menu.addAction(_("Close"))
		self.menu_close.triggered.connect(self.callback_close)


		self.menu_experiment=menubar.addMenu("&"+_("Experiments"))
		self.menu_experiment_new=self.menu_experiment.addAction("&"+_("New"))
		self.menu_experiment_new.triggered.connect(self.callback_add_page)

		self.menu_experiment_delete=self.menu_experiment.addAction("&"+_("Delete experiment"))
		self.menu_experiment_delete.triggered.connect(self.callback_delete_page)

		self.menu_experiment_rename=self.menu_experiment.addAction("&"+_("Rename experiment"))
		self.menu_experiment_rename.triggered.connect(self.callback_rename_page)

		self.menu_experiment_clone=self.menu_experiment.addAction("&"+_("Clone experiment"))
		self.menu_experiment_clone.triggered.connect(self.callback_copy_page)


		self.menu_help=menubar.addMenu(_("Help"))
		self.menu_help_help=self.menu_help.addAction(_("Help"))
		self.menu_help_help.triggered.connect(self.callback_help)


		self.main_vbox.addWidget(menubar)

		self.setWindowTitle(_("Frequency domain experiment editor")+" https://www.gpvdm.com") 
		self.setWindowIcon(QIcon_load("spectrum"))

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))

		self.new = QAction(QIcon_load("document-new"), _("New experiment"), self)
		self.new.triggered.connect(self.callback_add_page)
		toolbar.addAction(self.new)

		self.new = QAction(QIcon_load("edit-delete"), _("Delete experiment"), self)
		self.new.triggered.connect(self.callback_delete_page)
		toolbar.addAction(self.new)

		self.clone = QAction(QIcon_load("clone"), _("Clone experiment"), self)
		self.clone.triggered.connect(self.callback_copy_page)
		toolbar.addAction(self.clone)

		self.clone = QAction(QIcon_load("rename"), _("Rename experiment"), self)
		self.clone.triggered.connect(self.callback_rename_page)
		toolbar.addAction(self.clone)

		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		toolbar.addWidget(spacer)


		self.help = QAction(QIcon_load("help"), _("Help"), self)
		self.help.setStatusTip(_("Close"))
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


