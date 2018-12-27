#!/usr/bin/env python3
# -*- coding: utf-8 -*-
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

## @package gpvdm
#  The main gpvdm gui code
#

import os
import sys

from gui_enable import gui_test
gui_test()

#paths
sys.path.append('./gui/')
sys.path.append('/usr/lib/gpvdm/')
sys.path.append('/usr/lib64/gpvdm/')
sys.path.append('/usr/share/gpvdm/gui/')	#debian
sys.path.append('/usr/share/sip/PyQt5/')

from win_lin import running_on_linux
from cal_path import get_image_file_path
from cal_path import calculate_paths
from cal_path import calculate_paths_init
from cal_path import get_share_path
from cal_path import set_sim_path

calculate_paths_init()
calculate_paths()

import i18n
_ = i18n.language.gettext

from cal_path import get_inp_file_path
from token_lib import build_token_lib
build_token_lib()

from code_ctrl import enable_betafeatures
from code_ctrl import code_ctrl_load
from code_ctrl import enable_webupdates

#undo
from undo import undo_list_class

#ver
from ver import ver_load_info
from ver import ver_error

ver_load_info()

code_ctrl_load()

from command_args import command_args

command_args(len(sys.argv),sys.argv)

from import_archive import update_simulaton_to_new_ver

from inp import inp_get_token_value

from scan_item import scan_items_clear
from scan_item import scan_items_populate_from_known_tokens

from plot_gen import plot_gen
from help import help_window
from help import help_init
from help import language_advert
from notice import notice
from scan_item import scan_item_add
from window_list import resize_window_to_be_sane


from gpvdm_notebook import gpvdm_notebook
from epitaxy import epitaxy_load
from contacts_io import contacts_load

#server
from server import server
from server import server_init
from server import server_get

#mesh
from mesh import mesh_load_all


from PyQt5.QtWidgets import QMainWindow,QApplication
from splash import splash_window
from process_events import process_events
from error_han import error_han

#qt
from PyQt5.QtWidgets import QTextEdit, QAction
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import QSize, Qt,QFile,QIODevice
from PyQt5.QtWidgets import QWidget,QSizePolicy,QVBoxLayout,QHBoxLayout,QPushButton,QDialog,QFileDialog,QToolBar,QMessageBox, QLineEdit, QToolButton

def do_import():
	global new_simulation
	global dlg_export
	global device_lib_class
	global cool_menu

	global equation

	global webbrowser
	global ver_check_compatibility
	global update_thread
	global update_now

	global gen_workbook

	global plot_dlg_class
	global yes_no_dlg
	global isfiletype
	global dbus
	global DBusQtMainLoop
	global win_pipe


	global QTabWidget
	global ribbon

	global error_dlg

	global to_native_path
	global get_sim_path
	global clone_materials	
	global wpos_load
	global global_object_run
	global check_sim_exists

	#qt
	from PyQt5.QtWidgets import QTextEdit, QAction
	from PyQt5.QtGui import QIcon
	from PyQt5.QtCore import QSize, Qt,QFile,QIODevice
	from PyQt5.QtWidgets import QWidget,QSizePolicy,QVBoxLayout,QHBoxLayout,QPushButton,QDialog,QFileDialog,QToolBar,QMessageBox, QLineEdit, QToolButton
	
	from icon_lib import icons_load

	icons_load()
	#windows
	from new_simulation import new_simulation
	from dlg_export import dlg_export
	from device_lib import device_lib_class
	from cool_menu import cool_menu

	from equation import equation

	#python modules
	import webbrowser

	#ver
	from ver import ver_check_compatibility

	#updates
	from update import update_thread
	from update import update_now

	from workbook import gen_workbook

	from plot_dlg import plot_dlg_class
	from gui_util import yes_no_dlg
	from util import isfiletype
	if running_on_linux()==True:
		import dbus
		from dbus.mainloop.pyqt5 import DBusQtMainLoop

		if os.geteuid() == 0:
			exit(_("Don't run me as root!!"))
	else:
		from windows_pipe import win_pipe

	print(notice())

	#gobject.threads_init()

	from PyQt5.QtWidgets import QTabWidget
	from ribbon import ribbon

	from error_dlg import error_dlg

	from cal_path import to_native_path
	from cal_path import get_sim_path
	from clone import clone_materials	
	from window_list import wpos_load
	from global_objects import global_object_run
	from check_sim_exists import check_sim_exists

class gpvdm_main_window(QMainWindow):

	plot_after_run=False
	plot_after_run_file=""

	def adbus(self,bus, message):
		data=message.get_member()
		if data!=None:
			self.my_server.callback_dbus(data)
		
	def win_dbus(self, data):
		self.my_server.callback_dbus(data)


	def gui_sim_start(self):
		self.notebook_active_page=self.notebook.get_current_page()#self.tabText(i)#self.notebook.currentIndex()
		self.notebook.goto_page(_("Terminal"))

	def gui_sim_stop(self):
		message=""
		if self.notebook_active_page!=None:
			self.notebook.goto_page(self.notebook_active_page)
		global_object_run("display_recalculate")
	
	def callback_plot_after_run_toggle(self, widget, data):
		self.plot_after_run=data.get_active()
		self.config.set_value("#plot_after_simulation",data.get_active())


	def callback_set_plot_auto_close(self, widget, data):
		set_plot_auto_close(data.get_active())
		self.config.set_value("#one_plot_window",data.get_active())


	#def callback_run_scan(self, widget, data=None):
	#	if self.scan_window!=None:
	#		self.scan_window.callback_run_simulation()

	def callback_simulate(self):

		self.my_server.clear_cache()
		self.my_server.add_job(get_sim_path(),"")
		self.my_server.start()


	def close_now(self):
		QApplication.quit()
		

	def closeEvent(self, event):
		print("closing")
		self.close_now()
		event.accept()

	def callback_last_menu_click(self, widget, data):
		#self.plot_open.set_sensitive(True)
		file_to_load=os.path.join(get_sim_path(),data.file0)
		plot_gen([file_to_load],[],"auto")
		self.plot_after_run_file=file_to_load


	def callback_import_from_lib(self):
		device_lib=device_lib_class()
		device_lib.exec_()
		path=device_lib.file_path
		if path != "":
			device_lib.destroy()
			import_archive(path,os.path.join(get_sim_path(),"sim.gpvdm"),False)
			self.change_dir_and_refresh_interface(get_sim_path())
			print(_("file opened"),path)



	def callback_new(self):
		help_window().help_set_help(["p3ht_pcbm.png",_("<big><b>New simulation!</b></big><br> Now selected the type of device you would like to simulate.")])

		dialog=new_simulation()
		dialog.exec_()
		ret=dialog.ret_path
		if ret!=None:
			self.change_dir_and_refresh_interface(dialog.ret_path)


	def update_interface(self):
		if self.notebook.load()==True:
			#self.ti_light.connect('refresh', self.notebook.main_tab.update)
			self.check_sim_exists.set_dir(get_sim_path())
			self.ribbon.home.setEnabled(True)
			self.ribbon.simulations.setEnabled(True)
			#self.save_sim.setEnabled(True)
			#self.ribbon.device.setEnabled(True)
			help_window().help_set_help(["media-playback-start",_("<big><b>Now run the simulation</b></big><br> Click on the play icon to start a simulation.")])

			self.ribbon.home_export.setEnabled(True)
			#self.menu_import_lib.setEnabled(True)
			self.ribbon.configure.setEnabled(True)
			self.ribbon.goto_page(_("Home"))
			if enable_betafeatures()==True:
				self.ribbon.simulations.qe.setVisible(True)
		else:
			self.check_sim_exists.set_dir("")
			self.ribbon.home.setEnabled(False)

			self.ribbon.simulations.setEnabled(False)
			#self.ribbon.device.setEnabled(False)
			self.ribbon.goto_page(_("File"))
			help_window().help_set_help(["icon.png",_("<big><b>Hi!</b></big><br> I'm the on-line help system :).  If you find any bugs please report them to <a href=\"mailto:roderick.mackenzie@nottingham.ac.uk\">roderick.mackenzie@nottingham.ac.uk</a>."),"document-new.png",_("Click on the new icon to make a new simulation directory.")])
			language_advert()

			self.ribbon.home_export.setEnabled(False)
			self.ribbon.configure.setEnabled(False)
			if enable_betafeatures()==True:
				self.ribbon.simulations.qe.setVisible(True)

	def change_dir_and_refresh_interface(self,new_dir):
		scan_items_clear()
		self.splash.inc_value()

		scan_items_populate_from_known_tokens()
		self.splash.inc_value()

		self.splash.inc_value()

		set_sim_path(new_dir)
		self.splash.inc_value()

		calculate_paths()
		self.splash.inc_value()

		epitaxy_load(get_sim_path())
		self.splash.inc_value()

		contacts_load()
		self.splash.inc_value()

		if mesh_load_all()==False:
			error_dlg(self,_("There was a problem loading the electrical mesh, I suspect you are trying to open a file generated in a very old version of gpvdm."))
			return

		self.statusBar().showMessage(get_sim_path())
		self.splash.inc_value()

		self.update_interface()
		self.splash.inc_value()

		if self.notebook.terminal!=None:
			self.my_server.set_terminal(self.notebook.terminal)

		if self.notebook.update_display_function!=None:
			self.my_server.set_display_function(self.notebook.update_display_function)


		scan_item_add("sim.inp","#simmode","sim mode",1)
		scan_item_add("light.inp","#Psun","light intensity",1)
		#scan_populate_from_file("light.inp")

		self.ribbon.update()
		self.splash.inc_value()

	def load_sim(self,filename):
		new_path=os.path.dirname(filename)
		if filename.startswith(get_share_path())==True:
			error_dlg(self,_("You should not try to open simulations in the root gpvdm directory."))
			return

		if ver_check_compatibility(filename)==True:
			self.change_dir_and_refresh_interface(new_path)
		else:
			update = yes_no_dlg(self,_("The simulation you want to import looks like it was made on an old version of gpvdm, do you want to try to open it anyway?"))

			if update == True:
				update_simulaton_to_new_ver(filename)
				self.change_dir_and_refresh_interface(new_path)

			if os.path.isdir(os.path.join(new_path,"materials"))==False:
				copy = yes_no_dlg(self,_("It looks like there is no materials database in the simulation directory should I import one?"))
				if copy==True:
					clone_materials(new_path)

	def callback_open(self, widget, data=None):
		dialog = QFileDialog(self)
		dialog.setWindowTitle(_("Open an existing gpvdm simulation"))
		dialog.setNameFilter('Simulations - gpvdm (*.gpvdm *.opvdm)')
		dialog.setFileMode(QFileDialog.ExistingFile)
		if dialog.exec_() == QDialog.Accepted:
			filename = dialog.selectedFiles()[0]
			filename=to_native_path(filename)
			self.load_sim(filename)

	def callback_export(self, widget, data=None):
		dlg_export(self)

	def callback_update(self, widget, data=None):
		update_now()

	def callback_on_line_help(self):
		#print("here")
		#self.a=cool_menu(self.ribbon.home.help.icon())
		#self.a.show()
		#self.a.setVisible(True)

		#self.a.setFocusPolicy(Qt.StrongFocus)
		#self.a.setFocus(True)
		#self.a.hasFocus()
		webbrowser.open("https://www.gpvdm.com")


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

	def sim_gone(self):
		error_dlg(self,_("The simulation directory has been deleted."))
		self.update_interface()

	def __init__(self):
		super(gpvdm_main_window,self).__init__()
		self.splash=splash_window()
		self.splash.inc_value()
		process_events()
		process_events()

		#from wiz import wiz
		#a=wiz()
		#a.exec_()

		#sys.exit()
		do_import()

		if os.path.isdir(os.path.dirname(sys.argv[0]))==False:
			error_dlg(self,_("I can't run from inside a zip file!"))
			sys.exit()

		self.splash.inc_value()
		self.splash.inc_value()

		server_init()
		self.splash.inc_value()

		self.check_sim_exists=check_sim_exists()
		self.splash.inc_value()

		self.check_sim_exists.start_thread()
		self.splash.inc_value()

		self.check_sim_exists.sim_gone.connect(self.sim_gone)
		self.splash.inc_value()

		self.my_server=server_get()
		self.my_server.init(get_sim_path())
		self.splash.inc_value()


		self.undo_list=undo_list_class()
		wpos_load()
		self.splash.inc_value()

		self.ribbon=ribbon()
		self.splash.inc_value()

		self.notebook_active_page=None
		self.setAcceptDrops(True)
		#self.setGeometry(200, 100, 1300, 600)
		self.setWindowTitle("General-purpose Photovoltaic Device Model (https://www.gpvdm.com)")

		#super(gpvdm_main_window, self).__init__(parent, QtCore.Qt.FramelessWindowHint)
		#gobject.GObject.__init__(self)

		self.my_server.setup_gui(self.gui_sim_start)
		self.splash.inc_value()

		self.my_server.sim_finished.connect(self.gui_sim_stop)
		self.splash.inc_value()

		help_init()
		self.splash.inc_value()

		#help_window().help_set_help(["star.png",_("<big><b>Update available!</b></big><br>")])


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
		vbox=QVBoxLayout()
		self.splash.inc_value()

		vbox.addWidget(self.ribbon)
		vbox.addWidget(self.notebook)
		wvbox=QWidget()
		self.splash.inc_value()

		wvbox.setLayout(vbox)
		self.setCentralWidget(wvbox)

		self.splash.inc_value()

		self.statusBar()

		temp_error=ver_error()
		#print(temp_error)
		if len(temp_error)>0:
			error_dlg(self,temp_error)
			return


		self.setWindowIcon(QIcon(os.path.join(get_image_file_path(),"image.jpg")))		
		self.splash.inc_value()

		self.show_tabs = True
		self.show_border = True

		self.ribbon.home_export.triggered.connect(self.callback_export)

		#if enable_webupdates()==False:
		#	self.help_menu_update=help_menu.addAction("&"+_("Check for updates"))
		#	self.help_menu_update.triggered.connect(self.callback_update)

		self.ribbon.home_new.triggered.connect(self.callback_new)
		self.ribbon.home_open.triggered.connect(self.callback_open)
		self.ribbon.home.undo.triggered.connect(self.callback_undo)
		self.ribbon.home.run.triggered.connect(self.callback_simulate)
		self.splash.inc_value()

		self.ribbon.home.stop.setEnabled(False)

		self.ribbon.home.scan.setEnabled(False)
		
		self.ribbon.home.help.triggered.connect(self.callback_on_line_help)

		resize_window_to_be_sane(self,0.7,0.75)

		self.change_dir_and_refresh_interface(get_sim_path())
		self.splash.inc_value()


		self.ribbon.home.sun.changed.connect(self.notebook.update)
		self.ribbon.setAutoFillBackground(True)
		self.splash.inc_value()
		self.show()
		help_window().show()

	def dragEnterEvent(self, event):
		if event.mimeData().hasUrls:
			event.accept()
		else:
			event.ignore()

	def dropEvent(self, event):
		if event.mimeData().hasUrls:
			event.setDropAction(Qt.CopyAction)
			event.accept()
			links = []
			for url in event.mimeData().urls():
				links.append(str(url.toLocalFile()))
			if len(links)==1:
				file_name=links[0]
				if isfiletype(file_name,"gpvdm")==True:
					self.load_sim(file_name)
				elif os.path.isdir(file_name)==True:
					file_name=os.path.join(file_name,"sim.gpvdm")
					if os.path.isfile(file_name)==True:
						self.load_sim(file_name)
		else:
			event.ignore()
            
if __name__ == '__main__':
	
	app = QApplication(sys.argv)
	sys.excepthook = error_han

	ex = gpvdm_main_window()
	sys.exit(app.exec_())
