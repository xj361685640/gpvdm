# -*- coding: utf-8 -*-
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
from cal_path import get_image_file_path

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

from ribbon_device import ribbon_device
from ribbon_simulations import ribbon_simulations
from ribbon_configure import ribbon_configure
from ribbon_information import ribbon_information
from ribbon_home import ribbon_home

from about import about_dlg


class ribbon(QTabWidget):
	def goto_page(self,page):
		self.blockSignals(True)
		for i in range(0,self.count()):
				if self.tabText(i)==page:
					self.setCurrentIndex(i)
					break
		self.blockSignals(False)
		
	def file(self):
		toolbar = QToolBar()
		toolbar.setToolButtonStyle( Qt.ToolButtonTextUnderIcon)
		toolbar.setIconSize(QSize(42, 42))
		
		self.home_new = QAction(QIcon(os.path.join(get_image_file_path(),"new.png")), _("Make a new simulation"), self)
		self.home_new.setText("New\nsimulation")
		toolbar.addAction(self.home_new)

		self.home_open = QAction(QIcon(os.path.join(get_image_file_path(),"open.png")), _("Open\nsimulation"), self)
		toolbar.addAction(self.home_open)

		self.home_export = QAction(QIcon(os.path.join(get_image_file_path(),"export.png")), _("Export\ndata"), self)
		toolbar.addAction(self.home_export)

		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		toolbar.addWidget(spacer)

		self.home_help = QAction(QIcon(os.path.join(get_image_file_path(),"man.png")), _("Help"), self)
		toolbar.addAction(self.home_help)

		return toolbar
	


	
	def cluster(self):
		toolbar = QToolBar()
		toolbar.setToolButtonStyle( Qt.ToolButtonTextUnderIcon)
		toolbar.setIconSize(QSize(42, 42))

		#self.hpc_toolbar=hpc_class(self.my_server)
		#self.addToolBarBreak()
		#toolbar_hpc = self.addToolBar(self.hpc_toolbar)
		
		return toolbar

			
	def readStyleSheet(self,fileName):
		file = QFile(fileName)
		if file.open(QIODevice.ReadOnly) :
			css = file.readAll()
			file.close()
		return css

	def update(self):
		self.device.update()
		self.simulations.update()
		self.configure.update()
		self.information.update()
		self.home.update()

	def callback_about_dialog(self):
		dlg=about_dlg()
		dlg.exec_()

	def __init__(self):
		QTabWidget.__init__(self)
		self.setMaximumHeight(120)
		#self.setStyleSheet("QWidget {	background-color:cyan; }")

		#self.setFixedSize(-1, 300)
		#self.setWindowIcon(QIcon(os.path.join(get_image_file_path(),"jv.png")))
		#self.setWindowTitle(_("Steady state simulation")+"  (https://www.gpvdm.com)")

		self.about = QToolButton(self)
		self.about.setText(_("About"))
		self.about.pressed.connect(self.callback_about_dialog)

		self.setCornerWidget(self.about)

		w=self.file()
		self.addTab(w,_("File"))
		
		self.home=ribbon_home()
		self.addTab(self.home,_("Home"))
		
		self.simulations=ribbon_simulations()
		self.addTab(self.simulations,_("Simulations"))
		
		self.configure=ribbon_configure()
		self.addTab(self.configure,_("Configure"))
		
		self.device=ribbon_device()
		self.addTab(self.device,_("Device"))

		if enable_betafeatures()==True:
			self.tb_cluster=self.cluster()
			self.addTab(self.tb_cluster,_("Cluster"))

		self.information=ribbon_information()
		self.addTab(self.information,_("Information"))

		#self.setStyleSheet("QWidget {	background-color:cyan; }") 
		aaa=self.readStyleSheet(os.path.join(get_css_path(),"style.css"))
		aaa=str(aaa,'utf-8')
		#print(aaa.decode("utf-8") ,"QWidget {	background-color:cyan; }")
		self.setStyleSheet(aaa)

