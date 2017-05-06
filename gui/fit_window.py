#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2017 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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
import shutil
import webbrowser
from code_ctrl import enable_betafeatures
from util_zip import zip_lsdir
from util import strextract_interger
from global_objects import global_object_get
from icon_lib import QIcon_load

from global_objects import global_object_register
from server import server_get
from help import help_window

import i18n
_ = i18n.language.gettext

#inp
from inp import inp_isfile
from inp import inp_copy_file
from inp import inp_remove_file
from inp import inp_update_token_value
from fit_configure_window import fit_configure_window

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QStatusBar, QTableWidget, QAbstractItemView
from PyQt5.QtGui import QPainter,QIcon,QCursor

#windows
from gui_util import yes_no_dlg

from fit_tab import fit_tab
from QHTabBar import QHTabBar

from gui_util import dlg_get_text

from fit_progress import fit_progress
from inp import inp_get_token_value
from util import str2bool

from util import wrap_text
from cal_path import get_sim_path
from QWidgetSavePos import QWidgetSavePos

def fit_new_filename():
	for i in range(0,20):
		pulse_name="fit"+str(i)+".inp"
		if inp_isfile(pulse_name)==False:
			return i
	return -1

class fit_window(QWidgetSavePos):

	def update(self):
		for i in range(0,self.notebook.count()):
			tab = self.notebook.widget(i)
			tab.update()

	def callback_stop(self):
		my_server=server_get()
		my_server.force_stop()

	def callback_configure(self):
		if self.fit_configure_window==None:
			self.fit_configure_window=fit_configure_window("fit_config")
			
		help_window().help_set_help(["vars.png",_("<big><b>The fitting variables window</b></big><br> Use this window to select the variables use to perform the fit.")])
		if self.fit_configure_window.isVisible()==True:
			self.fit_configure_window.hide()
		else:
			self.fit_configure_window.show()

	def callback_help(self):
		webbrowser.open('https://www.gpvdm.com/man/index.html')

	def callback_add_page(self):
		new_sim_name=dlg_get_text( _("New fit name:"), _("fit ")+str(self.notebook.count()+1),"document-new")

		if new_sim_name.ret!=None:
			index=fit_new_filename()
			shutil.copy(os.path.join(get_sim_path(),"fit0.inp"),os.path.join(get_sim_path(),"fit"+str(index)+".inp"))
			shutil.copy(os.path.join(get_sim_path(),"fit_data0.inp"),os.path.join(get_sim_path(),"fit_data"+str(index)+".inp"))
			shutil.copy(os.path.join(get_sim_path(),"fit_patch0.inp"),os.path.join(get_sim_path(),"fit_patch"+str(index)+".inp"))
			shutil.copy(os.path.join(get_sim_path(),"fit_math0.inp"),os.path.join(get_sim_path(),"fit_math"+str(index)+".inp"))
			inp_update_token_value(os.path.join(get_sim_path(),"fit"+str(index)+".inp"), "#fit_name", new_sim_name.ret)
			self.add_page(index)

	def callback_remove_page(self,widget,name):
		tab = self.notebook.currentWidget()
		self.toggle_tab_visible(tab.tab_name)

	def callback_copy_page(self):
		tab = self.notebook.currentWidget()
		old_index=tab.index
		new_sim_name=dlg_get_text( _("Clone the current fit to a new fit called:"), tab.tab_name,"clone.png")
		if new_sim_name.ret!=None:
			index=fit_new_filename()

			shutil.copy(os.path.join(get_sim_path(),"fit"+str(old_index)+".inp"),os.path.join(get_sim_path(),"fit"+str(index)+".inp"))
			shutil.copy(os.path.join(get_sim_path(),"fit_data"+str(old_index)+".inp"),os.path.join(get_sim_path(),"fit_data"+str(index)+".inp"))
			shutil.copy(os.path.join(get_sim_path(),"fit_patch"+str(old_index)+".inp"),os.path.join(get_sim_path(),"fit_patch"+str(index)+".inp"))
			shutil.copy(os.path.join(get_sim_path(),"fit_math"+str(old_index)+".inp"),os.path.join(get_sim_path(),"fit_math"+str(index)+".inp"))

			inp_update_token_value(os.path.join(get_sim_path(),"fit"+str(index)+".inp"), "#fit_name", new_sim_name.ret)
			self.add_page(index)


	def remove_invalid(self,input_name):
		return input_name.replace (" ", "_")
			
	def callback_rename_page(self):
		tab = self.notebook.currentWidget()
		new_sim_name=dlg_get_text( _("Rename the fit to be called:"), tab.tab_name,"rename.png")

		if new_sim_name.ret!=None:
			#new_sim_name=self.remove_invalid(new_sim_name)
			#new_dir=os.path.join(self.sim_dir,new_sim_name)
			#shutil.move(old_dir, new_dir)
			tab.rename(new_sim_name.ret)
			index=self.notebook.currentIndex() 
			self.notebook.setTabText(index, new_sim_name.ret)

	def callback_import(self):
		tab = self.notebook.currentWidget()
		tab.import_data()

	def callback_delete_page(self):
		tab = self.notebook.currentWidget()
		response=yes_no_dlg(self, _("Should I remove the fit file ")+tab.tab_name.split("@")[0])

		if response==True:
			inp_remove_file("fit"+str(tab.index)+".inp")
			inp_remove_file("fit_data"+str(tab.index)+".inp")
			inp_remove_file("fit_patch"+str(tab.index)+".inp")
			inp_remove_file("fit_math"+str(tab.index)+".inp")
			index=self.notebook.currentIndex() 
			self.notebook.removeTab(index)

	def callback_view_toggle_tab(self):
		print("add code")
		#self.toggle_tab_visible(data)

	def load_tabs(self):

		file_list=zip_lsdir(os.path.join(get_sim_path(),"sim.gpvdm"))
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
				token=inp_get_token_value(os.path.join(get_sim_path(),files[i]), "#fit_hidden")
				if str2bool(token)==False:
					self.add_page(value)

		self.fit_progress=fit_progress()
		self.notebook.addTab(self.fit_progress,"Fit progress")

	def clear_pages(self):
		for items in self.tab_menu.get_children():
			self.tab_menu.remove(items)

		for child in self.notebook.get_children():
    			self.notebook.remove(child)


	def add_page(self,index):
		new_tab=fit_tab(index)
		self.notebook.addTab(new_tab,new_tab.tab_name)


	def rod(self):
		tab = self.notebook.currentWidget()
		tab.update()

	def callback_one_fit(self):
		my_server=server_get()
		my_server.clear_cache()
		my_server.add_job(get_sim_path(),"--1fit")
		my_server.set_callback_when_done(self.rod)
		my_server.start()

	def callback_do_fit(self):
		my_server=server_get()
		my_server.clear_cache()
		my_server.add_job(get_sim_path(),"--fit")
		my_server.start()


	def __init__(self,name):
		QWidgetSavePos.__init__(self,name)

		self.main_vbox = QVBoxLayout()

		#self.setFixedSize(900, 700)
		self.setWindowTitle(_("Fit window")+" https://www.gpvdm.com")   
		self.setWindowIcon(QIcon_load("fit"))

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))
		toolbar.setToolButtonStyle( Qt.ToolButtonTextUnderIcon)

		self.new = QAction(QIcon_load("document-new"), wrap_text(_("New fit"),4), self)
		self.new.triggered.connect(self.callback_add_page)
		toolbar.addAction(self.new)

		self.new = QAction(QIcon_load("edit-delete"), wrap_text(_("Delete fit"),4), self)
		self.new.triggered.connect(self.callback_delete_page)
		toolbar.addAction(self.new)

		self.clone = QAction(QIcon_load("clone"), wrap_text(_("Clone fit"),4), self)
		self.clone.triggered.connect(self.callback_copy_page)
		toolbar.addAction(self.clone)

		self.clone = QAction(QIcon_load("rename"), wrap_text(_("Rename fit"),4), self)
		self.clone.triggered.connect(self.callback_rename_page)
		toolbar.addAction(self.clone)

		toolbar.addSeparator()

		self.tb_configure= QAction(QIcon_load("preferences-system"), wrap_text(_("Configure"),4), self)
		self.tb_configure.triggered.connect(self.callback_configure)
		toolbar.addAction(self.tb_configure)

		self.import_data= QAction(QIcon_load("import"), _("Import data"), self)
		self.import_data.triggered.connect(self.callback_import)
		toolbar.addAction(self.import_data)

		toolbar.addSeparator()

		self.play= QAction(QIcon_load("media-playback-start"), wrap_text(_("Run a single fit"),4), self)
		self.play.triggered.connect(self.callback_one_fit)
		toolbar.addAction(self.play)
		
		self.play= QAction(QIcon_load("forward"),wrap_text( _("Start the fitting process"),4), self)
		self.play.triggered.connect(self.callback_do_fit)
		toolbar.addAction(self.play)

		self.pause= QAction(QIcon_load("media-playback-pause"), wrap_text(_("Stop the simulation"),4), self)
		self.pause.triggered.connect(self.callback_stop)
		toolbar.addAction(self.pause)

		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		toolbar.addWidget(spacer)


		self.help = QAction(QIcon_load("help"), _("Help"), self)
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

		self.fit_configure_window=None
		




