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
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QStatusBar
from PyQt5.QtGui import QPainter,QIcon

#window
from QHTabBar import QHTabBar
from PyQt5.QtCore import pyqtSignal

from util import wrap_text
from tb_item_is_imps import tb_item_is_imps

from cal_path import get_sim_path
from QWidgetSavePos import QWidgetSavePos
from experiment_util import experiment_new_filename

class fxexperiment(QWidgetSavePos):

	changed = pyqtSignal()

	def callback_help(self):
		webbrowser.open("https://www.gpvdm.com/man/index.html")

	def callback_add_page(self):
		new_sim_name=dlg_get_text( _("New experiment name")+":", _("experiment ")+str(self.notebook.count()+1),"document-new.png")

		new_sim_name=new_sim_name.ret

		if new_sim_name!=None:
			index=experiment_new_filename("fxdomain")
			inp_copy_file(os.path.join(get_sim_path(),"fxdomain"+str(index)+".inp"),os.path.join(get_sim_path(),"fxdomain0.inp"))
			inp_copy_file(os.path.join(get_sim_path(),"fxmesh"+str(index)+".inp"),os.path.join(get_sim_path(),"fxmesh0.inp"))
			inp_update_token_value(os.path.join(get_sim_path(),"fxdomain"+str(index)+".inp"), "#sim_menu_name", new_sim_name+"@fxdomain")
			self.add_page(index)
			self.changed.emit()


	def callback_copy_page(self):
		tab = self.notebook.currentWidget()
		old_index=tab.index
		new_sim_name=dlg_get_text( _("Clone the current experiment to a new experiment called")+":", tab.tab_name.split("@")[0],"clone.png")
		new_sim_name=new_sim_name.ret
		if new_sim_name!=None:
			new_sim_name=new_sim_name+"@"+tab.tab_name.split("@")[1]
			index=experiment_new_filename("fxdomain")
			if inp_copy_file(os.path.join(get_sim_path(),"fxdomain"+str(index)+".inp"),os.path.join(get_sim_path(),"fxdomain"+str(old_index)+".inp"))==False:
				print("Error copying file"+os.path.join(get_sim_path(),"fxdomain"+str(old_index)+".inp"))
				return
			if inp_copy_file(os.path.join(get_sim_path(),"fxmesh"+str(index)+".inp"),os.path.join(get_sim_path(),"fxmesh"+str(old_index)+".inp"))==False:
				print("Error copying file"+os.path.join(get_sim_path(),"fxdomain"+str(old_index)+".inp"))
				return

			inp_update_token_value(os.path.join(get_sim_path(),"fxdomain"+str(index)+".inp"), "#sim_menu_name", new_sim_name)
			self.add_page(index)
			self.changed.emit()


	#def callback_run_experiment(self,widget,data):
	#	pageNum = self.notebook.get_current_page()
	#	tab = self.notebook.get_nth_page(pageNum)
	#	tab.simulate(True,True)


	def remove_invalid(self,input_name):
		return input_name.replace (" ", "_")

	def callback_tab_changed(self):
		tab = self.notebook.currentWidget()
		self.mode.setText(tab.get_mode())

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
			inp_remove_file(os.path.join(get_sim_path(),"fxdomain"+str(tab.index)+".inp"))
			inp_remove_file(os.path.join(get_sim_path(),"fxmesh"+str(tab.index)+".inp"))
			index=self.notebook.currentIndex() 
			self.notebook.removeTab(index)
			self.changed.emit()


	def load_tabs(self):

		file_list=zip_lsdir(os.path.join(get_sim_path(),"sim.gpvdm"))
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

	def callback_mode_changed(self):
		tab = self.notebook.currentWidget()
		tab.update_mode(self.mode.mode.currentText())

	def callback_save(self):
		tab = self.notebook.currentWidget()
		tab.save_image()
	
	def __init__(self):
		QWidgetSavePos.__init__(self,"fxexperiment")
		self.setMinimumSize(1200, 700)

		self.main_vbox = QVBoxLayout()

		self.setWindowTitle(_("Frequency domain experiment editor")+" https://www.gpvdm.com") 
		self.setWindowIcon(QIcon_load("spectrum"))

		toolbar=QToolBar()
		toolbar.setToolButtonStyle( Qt.ToolButtonTextUnderIcon)
		toolbar.setIconSize(QSize(48, 48))

		self.new = QAction(QIcon_load("document-new"), wrap_text(_("New experiment"),2), self)
		self.new.triggered.connect(self.callback_add_page)
		toolbar.addAction(self.new)

		self.new = QAction(QIcon_load("edit-delete"), wrap_text(_("Delete experiment"),4), self)
		self.new.triggered.connect(self.callback_delete_page)
		toolbar.addAction(self.new)

		self.clone = QAction(QIcon_load("clone"), wrap_text(_("Clone experiment"),4), self)
		self.clone.triggered.connect(self.callback_copy_page)
		toolbar.addAction(self.clone)

		self.clone = QAction(QIcon_load("rename"), wrap_text(_("Rename experiment"),4), self)
		self.clone.triggered.connect(self.callback_rename_page)
		toolbar.addAction(self.clone)

		self.tb_save = QAction(QIcon_load("document-save-as"), wrap_text(_("Save image"),3), self)
		self.tb_save.triggered.connect(self.callback_save)
		toolbar.addAction(self.tb_save)

		self.mode=tb_item_is_imps()
		self.mode.changed.connect(self.callback_mode_changed)
		toolbar.addWidget(self.mode)


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
		self.notebook.currentChanged.connect(self.callback_tab_changed)

		self.load_tabs()

		self.main_vbox.addWidget(self.notebook)


		self.status_bar=QStatusBar()
		self.main_vbox.addWidget(self.status_bar)


		self.setLayout(self.main_vbox)


