#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2017 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
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
from cal_path import get_image_file_path
from gui_util import dlg_get_text
from window_list import windows
from util import delete_second_level_link_tree
from util import copy_scan_dir
from search import return_file_list
import webbrowser
from search import find_fit_log
from scan_io import get_scan_dirs
from code_ctrl import enable_betafeatures
from inp import inp_update_token_value
from inp import inp_get_token_value
from gui_util import yes_no_dlg

import i18n
_ = i18n.language.gettext

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QMenuBar,QStatusBar
from PyQt5.QtGui import QPainter,QIcon

#window
from scan_tab import scan_vbox
from QHTabBar import QHTabBar

class scan_class(QWidget):


	def callback_close(self):
		self.win_list.update(self,"scan_window")
		self.hide()
		return True

	def callback_change_dir(self):
		dialog = gtk.FileChooserDialog(_("Change directory"),
                               None,
                               gtk.FILE_CHOOSER_ACTION_OPEN,
                               (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                                gtk.STOCK_OK, gtk.RESPONSE_OK))
		dialog.set_default_response(gtk.RESPONSE_OK)
		dialog.set_action(gtk.FILE_CHOOSER_ACTION_CREATE_FOLDER)

		filter = gtk.FileFilter()
		filter.set_name(_("All files"))
		filter.add_pattern("*")
		dialog.add_filter(filter)


		response = dialog.run()
		if response == gtk.RESPONSE_OK:
			self.sim_dir=dialog.get_filename()

			a = open("scan_window.inp", "w")
			a.write(self.sim_dir)
			a.close()

			self.clear_pages()
			self.load_tabs()
			dialog.destroy()

		return True

	def callback_help(self):
		webbrowser.open('http://www.gpvdm.com/man/index.html')

	def callback_add_page(self):
		new_sim_name=dlg_get_text( _("New simulation name:"), _("Simulation ")+str(self.notebook.count()+1),"document-new.png")
		new_sim_name=new_sim_name.ret

		if new_sim_name!=None:
			new_sim_name=self.remove_invalid(new_sim_name)
			name=os.path.join(os.getcwd(),new_sim_name)
			self.add_page(name)

	def callback_cluster_fit_log(self):
		tab = self.notebook.currentWidget()
		name=tab.tab_name
		path=os.path.join(self.sim_dir,name)
		find_fit_log("./fit.dat",path)
		os.system("gnuplot -persist ./fit.dat &\n")


	def callback_copy_page(self):
		tab = self.notebook.currentWidget()
		name=tab.tab_name
		old_dir=os.path.join(self.sim_dir,name)
		new_sim_name=dlg_get_text( _("Clone the current simulation to a new simulation called:"), name,"clone.png")
		new_sim_name=new_sim_name.ret

		if new_sim_name!=None:
			new_sim_name=self.remove_invalid(new_sim_name)
			new_dir=os.path.join(self.sim_dir,new_sim_name)

			copy_scan_dir(new_dir,old_dir)
			print(_("I want to copy"),new_dir,old_dir)
			self.add_page(new_sim_name)

	def callback_run_simulation(self):
		tab = self.notebook.currentWidget()
		tab.simulate(True,False,"")

	def callback_run_single_fit(self):
		tab = self.notebook.currentWidget()
		tab.simulate(True,False,"--1fit")

	def callback_run_simulation_nogen(self):
		tab = self.notebook.currentWidget()
		tab.simulate(True,False,"")

	def callback_plot_fits(self):
		tab = self.notebook.currentWidget()
		tab.plot_fits()

	def callback_nested_simulation(self):
		tab = self.notebook.currentWidget()
		tab.nested_simulation()


	def callback_clean_simulation(self):
		tab = self.notebook.currentWidget()
		tab.clean_scan_dir()

	def callback_clean_unconverged_simulation(self):
		tab = self.notebook.currentWidget()
		tab.scan_clean_unconverged()

	def callback_clean_simulation_output(self):
		tab = self.notebook.currentWidget()
		tab.scan_clean_simulation_output()


	def callback_push_unconverged_to_hpc(self):
		tab = self.notebook.currentWidget()
		tab.push_unconverged_to_hpc()

	def remove_invalid(self,input_name):
		return input_name.replace (" ", "_")

	def callback_rename_page(self):
		tab = self.notebook.currentWidget()
		name=tab.tab_name
		old_dir=os.path.join(self.sim_dir,name)

		new_sim_name=dlg_get_text( _("Rename the simulation to be called:"), name,"rename.png")
		new_sim_name=new_sim_name.ret

		if new_sim_name!=None:
			new_sim_name=self.remove_invalid(new_sim_name)
			new_dir=os.path.join(self.sim_dir,new_sim_name)
			shutil.move(old_dir, new_dir)
			tab.rename(new_dir)
			index=self.notebook.currentIndex() 
			self.notebook.setTabText(index, new_sim_name)

	def callback_delete_page(self):
		tab = self.notebook.currentWidget()
		name=tab.tab_name
		dir_to_del=os.path.join(self.sim_dir,name)

		response=yes_no_dlg(self,_("Should I remove the simulation directory ")+dir_to_del)

		if response==True:
			index=self.notebook.currentIndex() 
			self.notebook.removeTab(index)
			print(_("I am going to delete file"),dir_to_del)
			delete_second_level_link_tree(dir_to_del)


	def callback_run_all_simulations(self):
		for i in range(0,self.notebook.count()):
			tab = self.notebook.widget(i)
			tab.simulate(True,True,"")

	def callback_stop_simulation(self,widget):
		tab = self.notebook.currentWidget()
		tab.stop_simulation()

	def load_tabs(self):
		sim_dirs=[]

		get_scan_dirs(sim_dirs,self.sim_dir)

		print(sim_dirs,self.sim_dir)

		if len(sim_dirs)==0:
			sim_dirs.append("scan1")
		else:
			for i in range(0,len(sim_dirs)):
				sim_dirs[i]=sim_dirs[i]

		for i in range(0,len(sim_dirs)):
			self.add_page(sim_dirs[i])

	def clear_pages(self):
		self.notebook.clear()

	def add_page(self,name):
		tab=scan_vbox(self.myserver,self.status_bar,self.sim_dir,name)
		self.notebook.addTab(tab,os.path.basename(name))


	def __init__(self,my_server):
		QWidget.__init__(self)
		self.myserver=my_server

		self.win_list=windows()
		self.win_list.load()
		self.win_list.set_window(self,"scan_window")
		self.setWindowTitle(_("Parameter scan - gpvdm"))
		self.setWindowIcon(QIcon(os.path.join(get_image_file_path(),"scan.png")))

		self.rod=[]
		if os.path.isfile("scan_window.inp"):
			f = open("scan_window.inp")
			lines = f.readlines()
			f.close()

			path=lines[0].strip()
			if path.startswith(os.getcwd()):
				self.sim_dir=path
			else:
				self.sim_dir=os.getcwd()
		else:
			self.sim_dir=os.getcwd()



		self.main_vbox = QVBoxLayout()

		menubar = QMenuBar()

		file_menu = menubar.addMenu("&"+_("File"))
		self.menu_change_dir=file_menu.addAction(_("Change dir"))
		self.menu_change_dir.triggered.connect(self.callback_close)
		self.menu_close=file_menu.addAction(_("Close"))
		self.menu_close.triggered.connect(self.callback_close)



		self.menu_simulation=menubar.addMenu(_("Simulations"))
		self.menu_new=self.menu_simulation.addAction("&"+_("New"))
		self.menu_new.triggered.connect(self.callback_add_page)

		self.menu_delete=self.menu_simulation.addAction("&"+_("Delete simulation"))
		self.menu_delete.triggered.connect(self.callback_delete_page)

		self.menu_rename=self.menu_simulation.addAction("&"+_("Rename simulation"))
		self.menu_rename.triggered.connect(self.callback_rename_page)

		self.menu_copy=self.menu_simulation.addAction("&"+_("Clone simulation"))
		self.menu_copy.triggered.connect(self.callback_copy_page)

		self.menu_simulation.addSeparator()

		self.menu_run=self.menu_simulation.addAction("&"+_("Run simulation"))
		self.menu_run.triggered.connect(self.callback_run_simulation)

		self.menu_advanced=menubar.addMenu(_("Advanced"))

		self.menu_plot_fits=self.menu_advanced.addAction("&"+_("Plot fits"))
		self.menu_plot_fits.triggered.connect(self.callback_plot_fits)

		self.menu_run_nested=self.menu_advanced.addAction("&"+_("Run nested simulation"))
		self.menu_run_nested.triggered.connect(self.callback_nested_simulation)

		self.menu_run_nested=self.menu_advanced.addAction("&"+_("Run simulation no generation"))
		self.menu_run_nested.triggered.connect(self.callback_run_simulation_nogen)

		self.menu_run_nested=self.menu_advanced.addAction("&"+_("Run simulation no generation"))
		self.menu_run_nested.triggered.connect(self.callback_run_simulation_nogen)

		self.menu_run_single_fit=self.menu_advanced.addAction("&"+_("Run single fit"))
		self.menu_run_single_fit.triggered.connect(self.callback_run_single_fit)

		self.menu_clean_simulation=self.menu_advanced.addAction("&"+_("Clean simulation"))
		self.menu_clean_simulation.triggered.connect(self.callback_clean_simulation)

		self.menu_clean_unconverged_simulation=self.menu_advanced.addAction("&"+_("Clean unconverged simulation"))
		self.menu_clean_unconverged_simulation.triggered.connect(self.callback_clean_unconverged_simulation)

		self.menu_clean_simulation_output=self.menu_advanced.addAction("&"+_("Clean simulation output"))
		self.menu_clean_simulation_output.triggered.connect(self.callback_clean_simulation_output)

		self.menu_clean_simulation_output=self.menu_advanced.addAction("&"+_("Clean simulation output"))
		self.menu_clean_simulation_output.triggered.connect(self.callback_clean_simulation_output)

		self.menu_advanced.addSeparator()

		self.menu_push_to_hpc=self.menu_advanced.addAction("&"+_("Push unconverged to hpc"))
		self.menu_push_to_hpc.triggered.connect(self.callback_push_unconverged_to_hpc)



		self.menu_help=menubar.addMenu(_("Help"))
		self.menu_help_help=self.menu_help.addAction(_("Help"))
		self.menu_help_help.triggered.connect(self.callback_help)

		self.main_vbox.addWidget(menubar)		


		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))

		self.tb_new = QAction(QIcon(os.path.join(get_image_file_path(),"document-new.png")), _("New simulation"), self)
		self.tb_new.triggered.connect(self.callback_add_page)
		toolbar.addAction(self.tb_new)

		self.tb_delete = QAction(QIcon(os.path.join(get_image_file_path(),"edit-delete.png")), _("Delete simulation"), self)
		self.tb_delete.triggered.connect(self.callback_delete_page)
		toolbar.addAction(self.tb_delete)

		self.tb_clone = QAction(QIcon(os.path.join(get_image_file_path(),"clone.png")), _("Clone simulation"), self)
		self.tb_clone.triggered.connect(self.callback_copy_page)
		toolbar.addAction(self.tb_clone)

		self.tb_rename = QAction(QIcon(os.path.join(get_image_file_path(),"rename.png")), _("Rename simulation"), self)
		self.tb_rename.triggered.connect(self.callback_rename_page)
		toolbar.addAction(self.tb_rename)

		self.tb_run_all = QAction(QIcon(os.path.join(get_image_file_path(),"32_forward2.png")), _("Run all simulations"), self)
		self.tb_run_all.triggered.connect(self.callback_run_all_simulations)
		toolbar.addAction(self.tb_run_all)

		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		toolbar.addWidget(spacer)


		self.help = QAction(QIcon(os.path.join(get_image_file_path(),"help.png")), _("Help"), self)
		self.help.setStatusTip(_("Close"))
		self.help.triggered.connect(self.callback_help)
		toolbar.addAction(self.help)

		self.main_vbox.addWidget(toolbar)

		self.notebook = QTabWidget()
		self.notebook.setTabBar(QHTabBar())

		self.notebook.setTabPosition(QTabWidget.West)
		self.notebook.setMovable(True)


		self.main_vbox.addWidget(self.notebook)


		self.status_bar=QStatusBar()
		self.main_vbox.addWidget(self.status_bar)		

		self.load_tabs()

		self.setLayout(self.main_vbox)


