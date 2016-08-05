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
from gui_util import dlg_get_text
from window_list import windows
import webbrowser
from inp import inp_update_token_value
#from fxexperiment_tab import fxexperiment_tab
from util_zip import zip_lsdir
from inp import inp_isfile
from inp import inp_copy_file
from inp import inp_remove_file
from util import strextract_interger
from global_objects import global_object_get
from cal_path import get_image_file_path
from global_objects import global_object_register
from code_ctrl import enable_betafeatures

import i18n
_ = i18n.language.gettext

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QMenuBar,QStatusBar
from PyQt5.QtGui import QPainter,QIcon

def experiment_new_filename():
	for i in range(0,20):
		name="fxdomain"+str(i)+".inp"
		if inp_isfile(name)==False:
			return i
	return -1

class fxexperiment(QWidget):

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
		#self.win_list.update(self,"experiment_window")
		self.hide()
		return True

	def callback_help(self, widget, data=None):
		webbrowser.open('http://www.gpvdm.com/man/index.html')

	def callback_add_page(self, widget, data=None):
		new_sim_name=dlg_get_text( _("New experiment name:"), _("experiment ")+str(len(self.notebook.get_children())+1),image_name="new.png")

		if new_sim_name!=None:
			index=experiment_new_filename()
			inp_copy_file("fxdomain"+str(index)+".inp","fxdomain0.inp")
			inp_copy_file("fxmesh"+str(index)+".inp","fxmesh0.inp")
			inp_update_token_value("fxdomain"+str(index)+".inp", "#sim_menu_name", new_sim_name+"@fxdomain",1)
			self.add_page(index)
			global_object_get("tb_item_sim_mode_update")()

	def callback_remove_page(self,widget,name):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		self.toggle_tab_visible(tab.tab_name)

	def callback_copy_page(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		old_index=tab.index
		new_sim_name=dlg_get_text( _("Clone the current experiment to a new experiment called:"), tab.tab_name.split("@")[0],image_name="clone.png")
		if new_sim_name!=None:
			new_sim_name=new_sim_name+"@"+tab.tab_name.split("@")[1]
			index=experiment_new_filename()
			if inp_copy_file("fxdomain"+str(index)+".inp","fxdomain"+str(old_index)+".inp")==False:
				print "Error copying file"+"fxdomain"+str(old_index)+".inp"
				return
			if inp_copy_file("fxmesh"+str(index)+".inp","fxmesh"+str(old_index)+".inp")==False:
				print "Error copying file"+"fxdomain"+str(old_index)+".inp"
				return

			inp_update_token_value("fxdomain"+str(index)+".inp", "#sim_menu_name", new_sim_name,1)
			self.add_page(index)
			global_object_get("tb_item_sim_mode_update")()

	#def callback_run_experiment(self,widget,data):
	#	pageNum = self.notebook.get_current_page()
	#	tab = self.notebook.get_nth_page(pageNum)
	#	tab.simulate(True,True)


	def remove_invalid(self,input_name):
		return input_name.replace (" ", "_")

	def callback_rename_page(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		new_sim_name=dlg_get_text( _("Rename the experiment to be called:"), tab.tab_name.split("@")[0],image_name="rename.png")

		if new_sim_name!=None:
			#new_sim_name=self.remove_invalid(new_sim_name)
			#new_dir=os.path.join(self.sim_dir,new_sim_name)
			#shutil.move(old_dir, new_dir)
			tab.rename(new_sim_name)
			global_object_get("tb_item_sim_mode_update")()
			#edit


	def callback_delete_page(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		md = gtk.MessageDialog(None, 0, gtk.MESSAGE_QUESTION,  gtk.BUTTONS_YES_NO, _("Should I remove the experiment file ")+tab.tab_name.split("@")[0])

		response = md.run()

		if response == gtk.RESPONSE_YES:
			inp_remove_file("fxdomain"+str(tab.index)+".inp")
			inp_remove_file("fxmesh"+str(tab.index)+".inp")
			self.notebook.remove_page(pageNum)
			global_object_get("tb_item_sim_mode_update")()

		elif response == gtk.RESPONSE_NO:
			print _("Not deleting")
			#edit


		md.destroy()

	#def callback_view_toggle(self, widget, data):
		#self.toggle_tab_visible(widget.get_label())

	def callback_view_toggle_tab(self, widget, data):
		print "add code"
		#self.toggle_tab_visible(data)

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
		widget	= QWidget()
		tab=fxexperiment_tab()
		tab.init(index)
		widget.setLayout(tab)
		self.notebook.addTab(widget,tab.title)

	def switch_page(self,page, page_num, user_param1):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		self.status_bar.push(self.context_id, tab.tab_name)

	def __init__(self):
		QWidget.__init__(self)

		self.win_list=windows()
		self.win_list.load()
		self.win_list.set_window(self,"fxexperiments_window")

		self.main_vbox = QVBoxLayout()

		menubar = QMenuBar()

		file_menu = menubar.addMenu('&File')
		self.menu_close=file_menu.addAction(_("Close"))
		self.menu_close.triggered.connect(self.callback_close)


		self.menu_experiment=menubar.addMenu(_("Experiments"))
		self.menu_experiment_new=self.menu_experiment.addAction(_("&New"))
		self.menu_experiment_new.triggered.connect(self.callback_add_page)

		self.menu_experiment_delete=self.menu_experiment.addAction(_("&Delete experiment"))
		self.menu_experiment_delete.triggered.connect(self.callback_delete_page)

		self.menu_experiment_rename=self.menu_experiment.addAction(_("&Rename experiment"))
		self.menu_experiment_rename.triggered.connect(self.callback_rename_page)

		self.menu_experiment_rename=self.menu_experiment.addAction(_("&Rename experiment"))
		self.menu_experiment_rename.triggered.connect(self.callback_rename_page)

		self.menu_experiment_clone=self.menu_experiment.addAction(_("&Clone experiment"))
		self.menu_experiment_clone.triggered.connect(self.callback_copy_page)


		self.menu_help=menubar.addMenu(_("Help"))
		self.menu_help_help=self.menu_help.addAction(_("Help"))
		self.menu_help_help.triggered.connect(self.callback_help)


		self.main_vbox.addWidget(menubar)

		self.setFixedSize(900, 500)
		self.setWindowTitle(_("Frequency domain experiment editor - gpvdm")) 
		self.setWindowIcon(QIcon(os.path.join(get_image_file_path(),"image.png")))

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))

		self.new = QAction(QIcon(os.path.join(get_image_file_path(),"new.png")), _("New experiment"), self)
		self.new.triggered.connect(self.callback_add_page)
		toolbar.addAction(self.new)

		self.new = QAction(QIcon(os.path.join(get_image_file_path(),"delete.png")), _("Delete experiment"), self)
		self.new.triggered.connect(self.callback_delete_page)
		toolbar.addAction(self.new)

		self.clone = QAction(QIcon(os.path.join(get_image_file_path(),"clone.png")), _("Clone experiment"), self)
		self.clone.triggered.connect(self.callback_copy_page)
		toolbar.addAction(self.clone)

		self.clone = QAction(QIcon(os.path.join(get_image_file_path(),"rename.png")), _("Rename experiment"), self)
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

		self.notebook.setTabsClosable(True)
		self.notebook.setMovable(True)

		#self.load_tabs()

		self.main_vbox.addWidget(self.notebook)


		self.status_bar=QStatusBar()
		self.main_vbox.addWidget(self.status_bar)


		self.setLayout(self.main_vbox)

		return

