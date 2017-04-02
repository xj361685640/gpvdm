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

import os
from icon_lib import QIcon_load

from dump_io import dump_io
from tb_item_sim_mode import tb_item_sim_mode
from tb_item_sun import tb_item_sun

from code_ctrl import enable_betafeatures
from cal_path import get_css_path

#qt
from PyQt5.QtWidgets import QMainWindow, QTextEdit, QAction, QApplication
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import QSize, Qt,QFile,QIODevice
from PyQt5.QtWidgets import QWidget,QSizePolicy,QVBoxLayout,QHBoxLayout,QPushButton,QDialog,QFileDialog,QToolBar,QMessageBox, QLineEdit, QToolButton
from PyQt5.QtWidgets import QTabWidget

from plot_gen import plot_gen
from info import sim_info
from win_lin import desktop_open

#windows
from scan import scan_class 
from help import help_window
from gpvdm_open import gpvdm_open
from gui_util import error_dlg
from server import server_get
from fit_window import fit_window
from cmp_class import cmp_class

from global_objects import global_object_run
from util import isfiletype

from server import server_get
from util import wrap_text

class ribbon_cluster(QToolBar):
	def __init__(self):
		QToolBar.__init__(self)
		self.myserver=server_get()

		self.setToolButtonStyle( Qt.ToolButtonTextUnderIcon)
		self.setIconSize(QSize(42, 42))
		
		self.cluster_button = QAction(QIcon_load("not_connected"), wrap_text(_("Connect to cluster"),8), self)
		self.cluster_button.triggered.connect(self.callback_cluster_connect)
		self.addAction(self.cluster_button)

		self.cluster_get_data = QAction(QIcon_load("server_get_data"), wrap_text(_("Cluster get data"),8), self)
		self.cluster_get_data.triggered.connect(self.callback_cluster_get_data)
		self.addAction(self.cluster_get_data)
		self.cluster_get_data.setEnabled(False)

		self.cluster_get_info = QAction(QIcon_load("server_get_info"), wrap_text(_("Cluster get info"),8), self)
		#self.cluster_get_info.triggered.connect(self.callback_cluster_get_info)
		self.addAction(self.cluster_get_info)
		self.cluster_get_info.setEnabled(False)

		self.cluster_copy_src = QAction(QIcon_load("server_copy_src"), wrap_text(_("Copy src to cluster"),8), self)
		self.cluster_copy_src.triggered.connect(self.callback_cluster_copy_src)
		self.addAction(self.cluster_copy_src)
		self.cluster_copy_src.setEnabled(False)

		self.cluster_make = QAction(QIcon_load("server_make"), wrap_text(_("Make on cluster"),6), self)
		self.cluster_make.triggered.connect(self.callback_cluster_make)
		self.addAction(self.cluster_make)
		self.cluster_make.setEnabled(False)

		self.cluster_clean = QAction(QIcon_load("server_clean"), wrap_text(_("Clean cluster"),8), self)
		self.cluster_clean.triggered.connect(self.callback_cluster_clean)
		self.addAction(self.cluster_clean)
		self.cluster_clean.setEnabled(False)

		self.cluster_off = QAction(QIcon_load("off"), wrap_text(_("Kill all cluster code"),8), self)
		self.cluster_off.triggered.connect(self.callback_cluster_off)
		self.addAction(self.cluster_off)
		self.cluster_off.setEnabled(False)


		self.cluster_sync = QAction(QIcon_load("sync"),  _("Sync"), self)
		self.cluster_sync.triggered.connect(self.callback_cluster_sync)
		self.addAction(self.cluster_sync)
		self.cluster_sync.setEnabled(False)

		self.cluster_stop = QAction(QIcon_load("pause"),  _("Stop"), self)
		self.cluster_stop.triggered.connect(self.callback_cluster_stop)
		self.addAction(self.cluster_stop)
		self.cluster_stop.setEnabled(False)


		self.cluster_jobs = QAction(QIcon_load("server_jobs"),  _("Get jobs"), self)
		self.cluster_jobs.triggered.connect(self.callback_cluster_jobs)
		self.addAction(self.cluster_jobs)
		self.cluster_jobs.setEnabled(False)

		self.cluster_view_button = QAction(QIcon_load("server"),  wrap_text(_("Configure cluster"),8), self)
		self.cluster_view_button.triggered.connect(self.callback_cluster_view_button)
		self.addAction(self.cluster_view_button)
		self.cluster_view_button.setEnabled(False)

		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		self.addWidget(spacer)
		
		self.help = QAction(QIcon_load("internet-web-browser"), _("Help"), self)
		self.addAction(self.help)
		
	def callback_cluster_make(self):
		self.myserver.cluster_make()

	def callback_cluster_stop(self):
		self.myserver.killall()

	def callback_cluster_clean(self):
		self.myserver.cluster_clean()

	def callback_cluster_off(self):
		self.myserver.cluster_quit()
		self.cluster_gui_update()

	def callback_cluster_sync(self):
		self.myserver.copy_src_to_cluster_fast()


	def callback_cluster_jobs(self):
		self.myserver.cluster_list_jobs()
		self.jview.load_data(self.myserver.cluster_jobs)

	def callback_cluster_sleep(self):
		self.myserver.sleep()

	def callback_cluster_poweroff(self):
		self.myserver.poweroff()

	def callback_cluster_print_jobs(self):
		self.myserver.print_jobs()

	def callback_wol(self, widget, data):
		self.myserver.wake_nodes()

	def callback_cluster_connect(self):

		if self.myserver.connect()==False:
			error_dlg(self,_("Can not connect to cluster."))

		self.update()
		if self.myserver.cluster==True:
			status_icon_stop(True)
			self.status_window.show()
		else:
			status_icon_stop(False)
			self.status_window.hide()
			
	def update(self):
		if self.myserver.cluster==True:
			self.cluster_button.setIcon(QIcon_load("connected"))
			self.cluster_clean.setEnabled(True)
			self.cluster_make.setEnabled(True)
			self.cluster_copy_src.setEnabled(True)
			self.cluster_get_info.setEnabled(True)
			self.cluster_get_data.setEnabled(True)
			self.cluster_off.setEnabled(True)
			self.cluster_sync.setEnabled(True)
			self.cluster_jobs.setEnabled(True)
			self.cluster_stop.setEnabled(True)
			self.cluster_view_button.setEnabled(True)

		else:
			self.cluster_button.setIcon(QIcon_load("not_connected"))
			self.cluster_clean.setEnabled(False)
			self.cluster_make.setEnabled(False)
			self.cluster_copy_src.setEnabled(False)
			self.cluster_get_info.setEnabled(False)
			self.cluster_get_data.setEnabled(False)
			self.cluster_off.setEnabled(False)
			self.cluster_sync.setEnabled(False)
			self.cluster_jobs.setEnabled(False)
			self.cluster_stop.setEnabled(False)
			self.cluster_view_button.setEnabled(False)

	def setEnabled(self,val):
		print("")
		#self.undo.setEnabled(val)

	def callback_cluster_get_data(self, widget, data=None):
		self.myserver.cluster_get_data()

	def callback_cluster_copy_src(self, widget, data=None):
		self.myserver.copy_src_to_cluster()
		
	def callback_cluster_view_button(self, widget, data=None):

		if self.hpc_window.get_property("visible")==True:
			self.hpc_window.hide()
		else:
			self.hpc_window.show()
