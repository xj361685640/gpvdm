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
#qt
from PyQt5.QtWidgets import QMainWindow, QTextEdit, QAction, QApplication
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import QSize, Qt,QFile,QIODevice
from PyQt5.QtWidgets import QWidget,QSizePolicy,QVBoxLayout,QHBoxLayout,QPushButton,QDialog,QFileDialog,QToolBar,QMessageBox, QLineEdit, QToolButton
from PyQt5.QtWidgets import QTabWidget

class ribbon(QTabWidget):
	def home(self):
		toolbar = QToolBar()
		toolbar.setToolButtonStyle( Qt.ToolButtonTextUnderIcon)
		toolbar.setIconSize(QSize(42, 42))
		
		self.home_new = QAction(QIcon(os.path.join(get_image_file_path(),"new.png")), _("Make a new simulation"), self)
		self.home_new.setText("New\nsimulation")
		#new_sim.triggered.connect(self.callback_new)
		toolbar.addAction(self.home_new)

		self.home_open = QAction(QIcon(os.path.join(get_image_file_path(),"open.png")), _("Open\nsimulation"), self)
		#open_sim.triggered.connect(self.callback_open)
		toolbar.addAction(self.home_open)


		toolbar.addSeparator()

		self.home_undo = QAction(QIcon(os.path.join(get_image_file_path(),"undo.png")), _("Undo"), self)
		toolbar.addAction(self.home_undo)

		toolbar.addSeparator()

		self.home_run = QAction(QIcon(os.path.join(get_image_file_path(),"play.png")), _("Run\nsimulation"), self)
		toolbar.addAction(self.home_run)

		self.home_stop = QAction(QIcon(os.path.join(get_image_file_path(),"pause.png")), _("Stop\nsimulation"), self)
		toolbar.addAction(self.home_stop)

		toolbar.addSeparator()
		
		self.home_scan = QAction(QIcon(os.path.join(get_image_file_path(),"scan.png")), _("Parameter\nscan"), self)

		toolbar.addAction(self.home_scan)




		#self.param_scan = QAction(QIcon(os.path.join(get_image_file_path(),"scan.png")), _("Parameter\nscan"), self)
		#self.param_scan.triggered.connect(self.callback_scan)
		#toolbar.addAction(self.param_scan)
		#self.param_scan.setEnabled(False)


			
		self.home_fit = QAction(QIcon(os.path.join(get_image_file_path(),"fit.png")), _("Run a fit command"), self)
		#self.tb_run_fit.triggered.connect(self.callback_run_fit)
		toolbar.addAction(self.home_fit)
		#self.tb_run_fit.setEnabled(True)

		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		toolbar.addWidget(spacer)

		self.home_help = QAction(QIcon(os.path.join(get_image_file_path(),"help.png")), _("Help"), self)
		toolbar.addAction(self.home_help)

		return toolbar


	def simulations(self):
		toolbar = QToolBar()
		toolbar.setToolButtonStyle( Qt.ToolButtonTextUnderIcon)
		toolbar.setIconSize(QSize(42, 42))

		self.simulations_time = QAction(QIcon(os.path.join(get_image_file_path(),"time.png")), _("Time domain\nsimulation editor."), self)
		toolbar.addAction(self.simulations_time )


		self.simulations_fx = QAction(QIcon(os.path.join(get_image_file_path(),"spectrum.png")), _("Frequency domain\nsimulation editor"), self)
		toolbar.addAction(self.simulations_fx)


		self.simulations_jv = QAction(QIcon(os.path.join(get_image_file_path(),"jv.png")), _("Steady state\nsimulation editor"), self)
		toolbar.addAction(self.simulations_jv)

		self.simulations_mode=tb_item_sim_mode()
		toolbar.addWidget(self.simulations_mode)
		
		if enable_betafeatures()==True:
			self.simulations_qe = QAction(QIcon(os.path.join(get_image_file_path(),"qe.png")), _("Quantum efficiency"), self)
			toolbar.addAction(self.simulations_qe)
			self.simulations_qe.setEnabled(False)


		return toolbar

	def configure(self):
		toolbar = QToolBar()
		toolbar.setToolButtonStyle( Qt.ToolButtonTextUnderIcon)
		toolbar.setIconSize(QSize(42, 42))

		self.configure_configwindow = QAction(QIcon(os.path.join(get_image_file_path(),"cog.png")), _("Time domain\nsimulation editor."), self)
		toolbar.addAction(self.configure_configwindow)
		
		self.configure_dump = dump_io(self)
		toolbar.addAction(self.configure_dump)
		
		return toolbar
		
	def plot(self):
		toolbar = QToolBar()
		toolbar.setToolButtonStyle( Qt.ToolButtonTextUnderIcon)
		toolbar.setIconSize(QSize(42, 42))

		self.plot_plot = QAction(QIcon(os.path.join(get_image_file_path(),"plot.png")), _("Plot\nFile"), self)
#		self.plot_select.triggered.connect(self.callback_plot_select)
		toolbar.addAction(self.plot_plot)

		self.plot_time = QAction(QIcon(os.path.join(get_image_file_path(),"plot_time.png")), _("Examine results in time domain"), self)
		toolbar.addAction(self.plot_time)

		return toolbar

	def optics(self):
		toolbar = QToolBar()
		toolbar.setToolButtonStyle( Qt.ToolButtonTextUnderIcon)
		toolbar.setIconSize(QSize(42, 42))

		self.optics_editor = QAction(QIcon(os.path.join(get_image_file_path(),"optics.png")), _("Optics"), self)
		toolbar.addAction(self.optics_editor)

		self.optics_lasers = QAction(QIcon(os.path.join(get_image_file_path(),"lasers.png")), _("Lasers editor"), self)
#		self.laser_button.triggered.connect(self.callback_configure_lasers)
		toolbar.addAction(self.optics_lasers)

		self.optics_sun=tb_item_sun()
		toolbar.addWidget(self.optics_sun)
		
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
	
	def __init__(self):
		QTabWidget.__init__(self)
		#self.setStyleSheet("QWidget {	background-color:cyan; }")

		#self.setFixedSize(-1, 300)
		#self.setWindowIcon(QIcon(os.path.join(get_image_file_path(),"jv.png")))
		#self.setWindowTitle(_("Steady state simulation")+"  (https://www.gpvdm.com)")

		#self.plusButton = QPushButton("+")
		#self.plusButton.setFixedSize(QSize(22, 22))
		self.plusButton = QToolButton(self)
		self.plusButton.setText("+")

		self.setCornerWidget(self.plusButton)

		w=self.home()
		self.addTab(w,"Home")
		
		wa=self.simulations()
		self.addTab(wa,"Simulations")
		
		wa=self.configure()
		self.addTab(wa,"Configure")
		
		wa=self.optics()
		self.addTab(wa,"Optics")

		if enable_betafeatures()==True:
			self.tb_cluster=self.cluster()
			self.addTab(self.tb_cluster,"Cluster")

		wa=self.plot()
		self.addTab(wa,"Plot")
		
	def init(self):
		aaa=self.readStyleSheet('style.css')
		aaa=str(aaa,'utf-8')
		#print(aaa.decode("utf-8") ,"QWidget {	background-color:cyan; }")
		self.setStyleSheet(aaa)
