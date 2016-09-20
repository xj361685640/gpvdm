#!/usr/bin/env python3
# -*- coding: utf-8 -*-
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

import sys

#paths
sys.path.append('./gui/')
sys.path.append('/usr/lib/gpvdm/')
sys.path.append('/usr/lib64/gpvdm/')
sys.path.append('/usr/share/gpvdm/gui/')	#debian

from win_lin import running_on_linux
from win_lin import desktop_open
from cal_path import get_exe_command
from cal_path import get_exe_name
from cal_path import get_image_file_path
from cal_path import calculate_paths
from cal_path import calculate_paths_init
calculate_paths_init()
calculate_paths()

import i18n
_ = i18n.language.gettext

from code_ctrl import enable_webupdates
from code_ctrl import enable_betafeatures
from code_ctrl import code_ctrl_load
from code_ctrl import enable_webupdates

#undo
from undo import undo_list_class

#ver
from ver import ver_load_info
from ver import ver_error

from gl import glWidget

ver_load_info()
code_ctrl_load()

from command_args import command_args

command_args(len(sys.argv),sys.argv)


import os
from inp import inp_get_token_value

from scan_item import scan_items_clear
from plot_gen import plot_gen
from help import help_window
from help import help_init
from notice import notice
from scan_item import scan_item_add
from window_list import windows
from qe import qe_window


from gpvdm_notebook import gpvdm_notebook
from epitaxy import epitaxy_load
from contacts_io import contacts_load

#server
from server import server
from server import server_init
from server import server_get

#mesh
from mesh import mesh_load_all

from info import sim_info

#qt
from PyQt5.QtWidgets import QMainWindow, QTextEdit, QAction, QApplication
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QSizePolicy,QHBoxLayout,QPushButton,QDialog,QFileDialog,QToolBar,QMessageBox, QLineEdit

#windows
from splash import splash_window
from config_window import class_config_window
from gpvdm_open import gpvdm_open
from jv import jv
from optics import class_optical
from new_simulation import new_simulation
from tb_item_sim_mode import tb_item_sim_mode
from tb_item_sun import tb_item_sun
from hpc import hpc_class
from lasers import lasers
from experiment import experiment
from fxexperiment import fxexperiment
from scan import scan_class 
from cmp_class import cmp_class
from about import about_dlg
from dlg_export import dlg_export
from fit_window import fit_window
from device_lib import device_lib_class

from equation import equation

#python modules
import webbrowser

#ver
from ver import ver_check_compatibility

#updates
from update import update_thread
from update import update_now

from workbook import gen_workbook

from error_han import error_han

from plot_dlg import plot_dlg_class

bubble=False
if running_on_linux()==True:
	import dbus
	from dbus.mainloop.pyqt5 import DBusQtMainLoop

	if os.geteuid() == 0:
		exit(_("Don't run me as root!!"))
	
else:
	from windows_pipe import win_pipe

print(notice())

#gobject.threads_init()


def set_active_name(combobox, name):
	liststore = combobox.get_model()
	for i in range(0,len(liststore)):
		if liststore[i][0] == name:
			combobox.set_active(i)

class gpvdm_main_window(QMainWindow):

	plot_after_run=False
	plot_after_run_file=""
	scan_window=None

	def adbus(self,bus, message):
		data=message.get_member()
		if data!=None:
			self.my_server.callback_dbus(data)

	def win_dbus(self, data):
		self.my_server.callback_dbus(data)


	def gui_sim_start(self):
		self.notebook_active_page=self.notebook.get_current_page()#self.tabText(i)#self.notebook.currentIndex()
		self.notebook.goto_page("Terminal")

	def gui_sim_stop(self):
		message=""
		self.notebook.goto_page(self.notebook_active_page)

		if os.path.isfile("signal_stop.dat")==True:
			f = open('signal_stop.dat')
			lines = f.readlines()
			f.close()
			message=lines[0].rstrip()



	def callback_plot_after_run_toggle(self, widget, data):
		self.plot_after_run=data.get_active()
		self.config.set_value("#plot_after_simulation",data.get_active())

	def callback_qe_window(self, widget):
		if self.qe_window==None:
			self.qe_window=qe_window()

		if self.qe_window.isVisible()==True:
			self.qe_window.hide()
		else:
			self.qe_window.show()

	def callback_set_plot_auto_close(self, widget, data):
		set_plot_auto_close(data.get_active())
		self.config.set_value("#one_plot_window",data.get_active())


	def callback_run_scan(self, widget, data=None):
		if self.scan_window!=None:
			self.scan_window.callback_run_simulation(None)

	def callback_simulate(self):

		self.my_server.clear_cache()
		self.my_server.add_job(os.getcwd(),"")
		self.my_server.start()


	def callback_simulate_stop(self):
		cmd = 'killall '+get_exe_name()
		print(cmd)
#		ret= os.system(cmd)

	def callback_run_fit(self, widget, data=None):
		if self.fit_window==None:
			self.fit_window=fit_window()
			self.fit_window.init()

		help_window().help_set_help(["fit.png",_("<big><b>Fit window</b></big><br> Use this window to fit the simulation to experimental data.")])
		if self.fit_window.isVisible()==True:
			self.fit_window.hide()
		else:
			self.fit_window.show()


	def callback_scan(self, widget, data=None):
		help_window().help_set_help(["scan.png",_("<big><b>The scan window</b></big><br> Very often it is useful to be able to systematically very a device parameter such as mobility or density of trap states.  This window allows you to do just that."),"add.png",_("Use the plus icon to add a new scan line to the list.")])
		self.tb_run_scan.setEnabled(True)

		if self.scan_window==None:
			self.scan_window=scan_class(self.my_server)


		if self.scan_window.isVisible()==True:
			self.scan_window.hide()
		else:
			self.scan_window.show()

	def close_now(self):
		self.win_list.update(self,"main_window")
		QApplication.quit()
		

	def closeEvent(self, event):
		print("closing")
		self.close_now()
		event.accept()


	def callback_plot_select(self):
		help_window().help_set_help(["dat_file.png",_("<big>Select a file to plot</big><br>Single clicking shows you the content of the file")])

		dialog=gpvdm_open(os.getcwd())
		dialog.show_inp_files=False
		dialog.show_directories=False
		ret=dialog.window.exec_()
		if ret==QDialog.Accepted:
			split=dialog.get_filename().split(".")
			if len(split)>0:
				if split[1]=="xlsx" or split[1]=="xls":
					desktop_open(dialog.get_filename())
	
					print("open with excel")
					return
				
			if os.path.basename(dialog.get_filename())=="sim_info.dat":
				if self.sim_info_window==None:
					self.sim_info_window=sim_info(dialog.get_filename())

				if self.sim_info_window.isVisible()==True:
					self.sim_info_window.hide()
				else:
					self.sim_info_window.show()
				return

			plot_gen([dialog.get_filename()],[],"auto")

			#self.plotted_graphs.refresh()
			#self.plot_after_run_file=dialog.get_filename()

	def callback_plot_open(self, widget, data=None):
		plot_gen([self.plot_after_run_file],[],"")

	def callback_last_menu_click(self, widget, data):
		#self.plot_open.set_sensitive(True)
		file_to_load=os.path.join(os.getcwd(),data.file0)
		plot_gen([file_to_load],[],"auto")
		self.plot_after_run_file=file_to_load


	def callback_import(self, widget, data=None):
		dialog = QFileDialog(self)
		dialog.setWindowTitle(_("Import an old gpvdm simulation"))
		dialog.setNameFilter('Simulations - gpvdm (*.gpvdm *.opvdm)')
		dialog.setFileMode(QFileDialog.ExistingFile)
		if dialog.exec_() == QDialog.Accepted:
			filename = dialog.selectedFiles()[0]

			import_archive(filename,os.path.join(os.getcwd(),"sim.gpvdm"),False)
			self.change_dir_and_refresh_interface(os.getcwd())

	def callback_import_from_lib(self):
		device_lib=device_lib_class()
		device_lib.exec_()
		path=device_lib.file_path
		if path != "":
			device_lib.destroy()
			import_archive(path,os.path.join(os.getcwd(),"sim.gpvdm"),False)
			self.change_dir_and_refresh_interface(os.getcwd())
			print(_("file opened"),path)

		


	def callback_new(self):
		help_window().help_set_help(["p3ht_pcbm.png",_("<big><b>New simulation!</b></big><br> Now selected the type of device you would like to simulate.")])

		dialog=new_simulation()
		dialog.window.exec_()
		ret=dialog.ret_path
		if ret!=None:
			self.change_dir_and_refresh_interface(dialog.ret_path)


	def change_dir_and_refresh_interface(self,new_dir):
		scan_items_clear()
		os.chdir(new_dir)
		calculate_paths()
		epitaxy_load()
		contacts_load()
		mesh_load_all()

		#print "rod",os.getcwd(),new_dir
		self.statusBar().showMessage(os.getcwd())
		#self.plot_open.setEnabled(False)

		#self.notebook.set_item_factory(self.item_factory)
		if self.notebook.load()==True:
			self.sim_mode_button.update()
			#self.ti_light.connect('refresh', self.notebook.main_tab.update)
			self.run.setEnabled(True)
			self.stop.setEnabled(True)
			self.examine.setEnabled(True)
			self.param_scan.setEnabled(True)
			self.plot_select.setEnabled(True)
			self.undo.setEnabled(True)
			self.jv_button.setEnabled(True)
			self.optics_button.setEnabled(True)
			self.laser_button.setEnabled(True)
			self.tb_time_domain.setEnabled(True)
			#self.save_sim.setEnabled(True)
			self.experiment_window_button.setEnabled(True)
			self.light_button.setEnabled(True)
			self.light_button.update()
			help_window().help_set_help(["play.png",_("<big><b>Now run the simulation</b></big><br> Click on the play icon to start a simulation.")])

			self.menu_new_optical_material.setEnabled(True)
			self.menu_export_data.setEnabled(True)
			self.menu_import_data.setEnabled(True)
			self.menu_import_lib.setEnabled(True)
			self.menu_run.setEnabled(True)
			self.menu_stop.setEnabled(True)
			self.menu_scan.setEnabled(True)
			self.menu_configure.setEnabled(True)
			self.sim_mode_button.setEnabled(True)
			if enable_betafeatures()==True:
				self.tb_run_fit.setEnabled(True)
				self.qe_button.setEnabled(True)
		else:
			self.run.setEnabled(False)
			self.stop.setEnabled(False)
			self.examine.setEnabled(False)
			self.param_scan.setEnabled(False)
			self.plot_select.setEnabled(False)
			self.undo.setEnabled(False)
			self.jv_button.setEnabled(False)
			self.optics_button.setEnabled(False)
			#self.save_sim.setEnabled(False)
			self.experiment_window_button.setEnabled(False)

			self.laser_button.setEnabled(False)
			self.tb_time_domain.setEnabled(False)
			self.sim_mode_button.setEnabled(False)
			self.light_button.setEnabled(False)

			help_window().help_set_help(["icon.png",_("<big><b>Hi!</b></big><br> I'm the on-line help system :).  If you find any bugs please report them to roderick.mackenzie@nottingham.ac.uk."),"new.png",_("Click on the new icon to make a new simulation directory.")])

			self.menu_new_optical_material.setEnabled(False)
			self.menu_export_data.setEnabled(False)
			self.menu_import_data.setEnabled(False)
			self.menu_import_lib.setEnabled(False)
			self.menu_run.setEnabled(False)
			self.menu_stop.setEnabled(False)
			self.menu_scan.setEnabled(False)
			self.menu_configure.setEnabled(False)
			if enable_betafeatures()==True:
				self.tb_run_fit.setEnabled(False)
				self.qe_button.setEnabled(False)

		if self.notebook.terminal!=None:
			self.my_server.set_terminal(self.notebook.terminal)

		#self.plotted_graphs.init(os.getcwd(),self.callback_last_menu_click)

		#set_active_name(self.light, inp_get_token_value("light.inp", "#Psun"))

		scan_item_add("sim.inp","#simmode","sim mode",1)
		scan_item_add("light.inp","#Psun","light intensity",1)
		#scan_populate_from_file("light.inp")

		if self.scan_window!=None:
			del self.scan_window
			self.scan_window=None

		if self.experiment_window!=None:
			del self.experiment_window
			self.experiment_window=None

		if self.fxexperiment_window!=None:
			del self.fxexperiment_window
			self.fxexperiment_window=None

		if self.jvexperiment_window!=None:
			del self.jvexperiment_window
			self.jvexperiment_window=None

		if self.sim_info_window!=None:
			del self.sim_info_window
			self.sim_info_window=None

		if self.fit_window!=None:
			del self.fit_window
			self.fit_window=None

		if self.lasers_window!=None:
			del self.lasers_window
			self.lasers_window=None

		if self.config_window!=None:
			del self.config_window
			self.config_window=None

		if self.qe_window!=None:
			del self.qe_window
			self.qe_window=None

		#myitem=self.item_factory.get_item("/Plots/One plot window")
		#myitem.set_active(self.config.get_value("#one_plot_window",False))
		#myitem=self.item_factory.get_item("/Plots/Plot after simulation")
		#myitem.set_active(self.config.get_value("#plot_after_simulation",False))

	def callback_open(self, widget, data=None):
		dialog = QFileDialog(self)
		dialog.setWindowTitle(_("Open an existing gpvdm simulation"))
		dialog.setNameFilter('Simulations - gpvdm (*.gpvdm *.opvdm)')
		dialog.setFileMode(QFileDialog.ExistingFile)
		if dialog.exec_() == QDialog.Accepted:
			filename = dialog.selectedFiles()[0]

			new_path=os.path.dirname(filename)

			if ver_check_compatibility(filename)==True:
				self.change_dir_and_refresh_interface(new_path)
			else:
				reply = yes_no_dlg(self,"The simulation you want to import looks like it was made on an old version of gpvdm, do you want to try to open it anyway?")

				if reply == True:
					update_simulaton_to_new_ver(dialog.get_filename())
					self.change_dir_and_refresh_interface(new_path)

	def callback_export(self, widget, data=None):
		dlg_export(self)

	def callback_about_dialog(self):
		dlg=about_dlg()
		dlg.ui.exec_()

	def callback_help(self, widget, data=None):
		help_window().toggle_visible()

	def callback_update(self, widget, data=None):
		update_now()

	def callback_on_line_help(self):
		webbrowser.open('www.gpvdm.com')

	def callback_license(self):
		webbrowser.open('www.gpvdm.com/license.html')
		
	def callback_new_window(self, widget, data=None):
		if self.window2.isVisible()==True:
			self.window2.hide()
		else:
			self.window2.show()

	def callback_close_window2(self, widget, data=None):
		self.window2.hide()
		return True


	def callback_examine(self, widget, data=None):
		help_window().help_set_help(["plot_time.png",_("<big><b>Examine the results in time domain</b></big><br> After you have run a simulation in time domain, if is often nice to be able to step through the simulation and look at the results.  This is what this window does.  Use the slider bar to move through the simulation.  When you are simulating a JV curve, the slider sill step through voltage points rather than time points.")])
		self.my_cmp_class=cmp_class()
		self.my_cmp_class.show()

		return
		ret=mycmp.init()
		if ret==False:
			msgBox = QMessageBox(self)
			msgBox.setIcon(QMessageBox.Critical)
			msgBox.setText(self.tr("gpvdm"))
			msgBox.setInformativeText(_("Re-run the simulation with 'dump all slices' set to one to use this tool."))
			msgBox.setStandardButtons(QMessageBox.Ok )
			msgBox.setDefaultButton(QMessageBox.Ok)
			reply = msgBox.exec_()
			return


	def callback_edit_experiment_window(self):

		if self.experiment_window==None:
			self.experiment_window=experiment()
			self.experiment_window.changed.connect(self.sim_mode_button.update)
			
		help_window().help_set_help(["time.png",_("<big><b>The time mesh editor</b></big><br> To do time domain simulations one must define how voltage the light vary as a function of time.  This can be done in this window.  Also use this window to define the simulation length and time step.")])
		if self.experiment_window.isVisible()==True:
			self.experiment_window.hide()
		else:
			self.experiment_window.show()


	def callback_fxexperiment_window(self):

		if self.fxexperiment_window==None:
			self.fxexperiment_window=fxexperiment()
			self.fxexperiment_window.changed.connect(self.sim_mode_button.update)
			
		help_window().help_set_help(["spectrum.png",_("<big><b>Frequency domain mesh editor</b></big><br> Some times it is useful to do frequency domain simulations such as when simulating impedance spectroscopy.  This window will allow you to choose which frequencies will be simulated.")])
		if self.fxexperiment_window.isVisible()==True:
			self.fxexperiment_window.hide()
		else:
			self.fxexperiment_window.show()
		
	def callback_configure_lasers(self):

		if self.lasers_window==None:
			self.lasers_window=lasers()

		help_window().help_set_help(["lasers.png",_("<big><b>Laser setup</b></big><br> Use this window to set up your lasers.")])
		if self.lasers_window.isVisible()==True:
			self.lasers_window.hide()
		else:
			self.lasers_window.show()

	def callback_jv_window(self):

		if self.jvexperiment_window==None:
			self.jvexperiment_window=jv()

		help_window().help_set_help(["jv.png",_("<big><b>JV simulation editor</b></big><br> Use this window to select the step size and parameters of the JV simulations.")])
		if self.jvexperiment_window.isVisible()==True:
			self.jvexperiment_window.hide()
		else:
			self.jvexperiment_window.show()

	def callback_config_window(self):

		if self.config_window==None:
			self.config_window=class_config_window()
			self.config_window.init()

		help_window().help_set_help(["cog.png",_("<big><b>Configuration editor</b></big><br> Use this window to control advanced simulation parameters.")])
		if self.config_window.isVisible()==True:
			self.config_window.hide()
		else:
			self.config_window.show()

	def callback_undo(self, widget, data=None):
		l=self.undo_list.get_list()
		if len(l)>0:
			value=l[len(l)-1][2]
			w_type=l[len(l)-1][3]

			if type(w_type)==QLineEdit:
				self.undo_list.disable()
				w_type.setText(value)
				self.undo_list.enable()

			l.pop()



	def make_tool_box1(self):
		toolbar=QToolBar()
		toolbar.setIconSize(QSize(42, 42))
		if enable_betafeatures()==True:
			self.qe_button = QAction(QIcon(os.path.join(get_image_file_path(),"qe.png")), _("Quantum efficiency"), self)
			self.qe_button.triggered.connect(self.callback_qe_window)
			toolbar.addAction(self.qe_button)
			self.qe_button.setEnabled(False)

		self.sim_mode_button=tb_item_sim_mode()
		toolbar.addWidget(self.sim_mode_button)
		self.sim_mode_button.setEnabled(False)


		self.light_button=tb_item_sun()
		toolbar.addWidget(self.light_button)
		self.light_button.setEnabled(False)

		return toolbar




	def callback_optics_sim(self, widget, data=None):
		help_window().help_set_help(["optics.png",_("<big><b>The optical simulation window</b></big><br>Use this window to perform optical simulations.  Click on the play button to run a simulation."),"play.png",_("Click on the play button to run an optical simulation.  The results will be displayed in the tabs to the right.")])

		if self.optics_window==False:
			self.optics_window=class_optical()

		if self.optics_window.isVisible()==True:
			self.optics_window.hide()
		else:
			self.optics_window.show()


	def __init__(self):
		self.undo_list=undo_list_class()
		super(gpvdm_main_window,self).__init__()
		self.setGeometry(200, 100, 1300, 600)
		self.setWindowTitle(_("General-purpose Photovoltaic Device Model (www.gpvdm.com)"))

		#super(gpvdm_main_window, self).__init__(parent, QtCore.Qt.FramelessWindowHint)
		#gobject.GObject.__init__(self)
		server_init()
		self.my_server=server_get()
		self.my_server.init(os.getcwd())
		self.my_server.setup_gui(self.gui_sim_start)
		self.my_server.sim_finished.connect(self.gui_sim_stop)

		help_init()
		#help_window().help_set_help(["star.png",_("<big><b>Update available!</b></big><br>")])
		self.win_list=windows()
		self.win_list.load()

		#self.show()

		if running_on_linux()==True:
			DBusQtMainLoop(set_as_default=True)
			self.bus = dbus.SessionBus()
			self.bus.add_match_string_non_blocking("type='signal',interface='org.my.gpvdm'")
			self.bus.add_message_filter(self.adbus)
		else:
			self.win_pipe=win_pipe()
			self.win_pipe.new_data.connect(self.win_dbus)
			self.win_pipe.start()

		self.notebook=gpvdm_notebook()
		self.setCentralWidget(self.notebook)
		self.show()

		self.statusBar()

		toolbar = self.addToolBar('Exit')
		toolbar.setIconSize(QSize(42, 42))

		self.splash=splash_window()
		self.splash.init()

		temp_error=ver_error()
		print(temp_error)
		if len(temp_error)>0:
			msgBox = QMessageBox(self)
			msgBox.setIcon(QMessageBox.Critical)
			msgBox.setText(self.tr("gpvdm"))
			msgBox.setInformativeText(temp_error)
			msgBox.setStandardButtons(QMessageBox.Ok )
			msgBox.setDefaultButton(QMessageBox.Ok)
			reply = msgBox.exec_()
			return



		self.experiment_window=None

		self.fxexperiment_window=None

		self.jvexperiment_window=None

		self.fit_window=None

		self.config_window=None

		self.optics_window=False

		self.qe_window=None

		self.lasers_window=None

		self.sim_info_window=None

		self.setWindowIcon(QIcon(os.path.join(get_image_file_path(),"image.jpg")))		

		self.show_tabs = True
		self.show_border = True

		menubar = self.menuBar()


		file_menu = menubar.addMenu('&File')
		self.menu_new=file_menu.addAction(_("&New simulation"))
		self.menu_new.triggered.connect(self.callback_new)

		self.menu_new_optical_material=file_menu.addAction(_("New optical material"))

		self.menu_export_open=file_menu.addAction(_("&Open simulation"))
		self.menu_export_open.triggered.connect(self.callback_open)

		self.menu_export_data=file_menu.addAction(_("&Export data"))
		self.menu_export_data.triggered.connect(self.callback_export)

		self.menu_import_data=file_menu.addAction(_("&Import data"))
		self.menu_import_data.triggered.connect(self.callback_import)

		self.menu_import_lib=file_menu.addAction(_("Import from library"))
		self.menu_import_lib.triggered.connect(self.callback_import_from_lib)

		self.menu_quit=file_menu.addAction(_("&Quit"))
		self.menu_quit.triggered.connect(self.close_now)

		simulation_menu = menubar.addMenu('&Simulation')

		self.menu_run=simulation_menu.addAction(_("&Run"))
		self.menu_run.triggered.connect(self.callback_simulate)

		self.menu_stop=simulation_menu.addAction(_("&Stop"))
		self.menu_stop.triggered.connect(self.callback_simulate_stop)

		self.menu_scan=simulation_menu.addAction(_("&Parameter scan"))
		self.menu_scan.triggered.connect(self.callback_scan)

		self.menu_configure=simulation_menu.addAction(_("&Configure"))
		self.menu_configure.triggered.connect(self.callback_config_window)


		view_menu = menubar.addMenu('&View')
		view_menu.addAction(_("&None"))


		plot_menu = menubar.addMenu('&Plot')
		self.plot_menu_plot=plot_menu.addAction(_("&Plot simulation result"))
		self.plot_menu_plot.triggered.connect(self.callback_plot_select)


		help_menu = menubar.addMenu('Help')

		help_web=help_menu.addAction(_("&Help window"))
		help_web.triggered.connect(self.callback_help)

		help_web=help_menu.addAction(_("&Online help"))
		help_web.triggered.connect(self.callback_on_line_help)

		help_web=help_menu.addAction(_("&License"))
		help_web.triggered.connect(self.callback_license)

		about=help_menu.addAction(_("&About"))
		about.triggered.connect(self.callback_about_dialog)




		if enable_webupdates()==False:
			self.help_menu_update=help_menu.addAction(_("&Check for updates"))
			self.help_menu_update.triggered.connect(self.callback_update)


		new_sim = QAction(QIcon(os.path.join(get_image_file_path(),"new.png")), _("Make a new simulation"), self)
		new_sim.triggered.connect(self.callback_new)
		toolbar.addAction(new_sim)

		open_sim = QAction(QIcon(os.path.join(get_image_file_path(),"open.png")), _("Open a simulation"), self)
		open_sim.triggered.connect(self.callback_open)
		toolbar.addAction(open_sim)


		toolbar.addSeparator()

		self.undo = QAction(QIcon(os.path.join(get_image_file_path(),"undo.png")), _("Undo"), self)
		self.undo.triggered.connect(self.callback_undo)
		toolbar.addAction(self.undo)
		#seperator


		self.run = QAction(QIcon(os.path.join(get_image_file_path(),"play.png")), _("Run the simulation"), self)
		self.run.triggered.connect(self.callback_simulate)
		toolbar.addAction(self.run)

		self.tb_run_scan = QAction(QIcon(os.path.join(get_image_file_path(),"forward.png")), _("Run parameter scan"), self)
		self.tb_run_scan.triggered.connect(self.callback_run_scan)
		self.tb_run_scan.setEnabled(False)
		toolbar.addAction(self.tb_run_scan)

		self.stop = QAction(QIcon(os.path.join(get_image_file_path(),"pause.png")), _("Stop the simulation"), self)
		self.stop.triggered.connect(self.callback_simulate_stop)
		toolbar.addAction(self.stop)
		self.stop.setEnabled(False)


		toolbar.addSeparator()

		self.param_scan = QAction(QIcon(os.path.join(get_image_file_path(),"scan.png")), _("Parameter scan"), self)
		self.param_scan.triggered.connect(self.callback_scan)
		toolbar.addAction(self.param_scan)
		self.param_scan.setEnabled(False)


		if enable_betafeatures()==True:
			self.tb_run_fit = QAction(QIcon(os.path.join(get_image_file_path(),"fit.png")), _("Run a fit command"), self)
			self.tb_run_fit.triggered.connect(self.callback_run_fit)
			toolbar.addAction(self.tb_run_fit)
			self.tb_run_fit.setEnabled(True)


		toolbar.addSeparator()


		self.plot_select = QAction(QIcon(os.path.join(get_image_file_path(),"plot.png")), _("Find a file to plot"), self)
		self.plot_select.triggered.connect(self.callback_plot_select)
		toolbar.addAction(self.plot_select)

		self.examine = QAction(QIcon(os.path.join(get_image_file_path(),"plot_time.png")), _("Examine results in time domain"), self)
		self.examine.triggered.connect(self.callback_examine)
		toolbar.addAction(self.examine)

		toolbar.addSeparator()

		self.tb_time_domain = QAction(QIcon(os.path.join(get_image_file_path(),"time.png")), _("Time domain simulation editor."), self)
		self.tb_time_domain.triggered.connect(self.callback_edit_experiment_window)
		toolbar.addAction(self.tb_time_domain)


		self.experiment_window_button = QAction(QIcon(os.path.join(get_image_file_path(),"spectrum.png")), _("Frequency domain simulation editor"), self)
		self.experiment_window_button.triggered.connect(self.callback_fxexperiment_window)
		toolbar.addAction(self.experiment_window_button)


		self.jv_button = QAction(QIcon(os.path.join(get_image_file_path(),"jv.png")), _("Steady state simulation editor"), self)
		self.jv_button.triggered.connect(self.callback_jv_window)
		toolbar.addAction(self.jv_button)

		self.optics_button = QAction(QIcon(os.path.join(get_image_file_path(),"optics.png")), _("Optical simulation"), self)
		self.optics_button.triggered.connect(self.callback_optics_sim)
		toolbar.addAction(self.optics_button)


		self.laser_button = QAction(QIcon(os.path.join(get_image_file_path(),"lasers.png")), _("Lasers editor"), self)
		self.laser_button.triggered.connect(self.callback_configure_lasers)
		toolbar.addAction(self.laser_button)


		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		toolbar.addWidget(spacer)


		self.help = QAction(QIcon(os.path.join(get_image_file_path(),"help.png")), _("Help"), self)
		self.help.triggered.connect(self.callback_on_line_help)
		toolbar.addAction(self.help)



		self.addToolBarBreak()
		toolbar1=self.make_tool_box1()
		self.addToolBar(toolbar1)


		if enable_betafeatures()==True:
			self.hpc_toolbar=hpc_class(self.my_server)
			#self.addToolBarBreak()
			toolbar_hpc = self.addToolBar(self.hpc_toolbar)
	
		self.win_list.set_window(self,"main_window")



#		self.menubar.show()

#		self.make_window2(main_vbox)
		#help_window().show()
		self.change_dir_and_refresh_interface(os.getcwd())



#		self.window.show()

#		process_events()

		self.show()

		self.light_button.changed.connect(self.notebook.update)
		#from plot_state import plot_state
		#plot_data=plot_state()
		#plot_data.example_file0=os.path.join(os.getcwd(),"dos0.inp")
		#a=plot_dlg_class(plot_data)
		#a.run()
		
if __name__ == '__main__':
	app = QApplication(sys.argv)
	sys.excepthook = error_han

	ex = gpvdm_main_window()
	sys.exit(app.exec_())
