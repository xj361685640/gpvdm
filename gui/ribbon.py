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


class c_information(QToolBar):
	def __init__(self):
		QToolBar.__init__(self)
		self.setToolButtonStyle( Qt.ToolButtonTextUnderIcon)
		self.setIconSize(QSize(42, 42))


		self.license = QAction(QIcon(os.path.join(get_image_file_path(),"license.png")), _("License")+"\n"	, self)
		self.addAction(self.license)		

		self.ref = QAction(QIcon(os.path.join(get_image_file_path(),"ref.png")), _("How to\ncite"), self)
		self.addAction(self.ref)

		self.hints = QAction(QIcon(os.path.join(get_image_file_path(),"hints.png")), _("Hints\nWindow"), self)
		self.addAction(self.hints)

		#self.about = QAction(QIcon(os.path.join(get_image_file_path(),"help.png")), _("About")+"\n", self)
		#self.addAction(self.about)


		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		self.addWidget(spacer)
		
		self.youtube = QAction(QIcon(os.path.join(get_image_file_path(),"youtube.png")), _("Youtube\nchannel"), self)
		self.addAction(self.youtube)

		self.man = QAction(QIcon(os.path.join(get_image_file_path(),"man.png")), _("Help")+"\n", self)
		self.addAction(self.man)



class c_device(QToolBar):
	def __init__(self):
		QToolBar.__init__(self)
		self.setToolButtonStyle( Qt.ToolButtonTextUnderIcon)
		self.setIconSize(QSize(42, 42))

		self.doping = QAction(QIcon(os.path.join(get_image_file_path(),"doping.png")), _("Doping"), self)
		self.addAction(self.doping)
		
		self.materials = QAction(QIcon(os.path.join(get_image_file_path(),"organic_material.png")), _("Materials\ndatabase"), self)
		self.addAction(self.materials)
	
		self.cost = QAction(QIcon(os.path.join(get_image_file_path(),"cost.png")), _("Calculate\nthe cost"), self)
		self.addAction(self.cost)
		
		self.contacts = QAction(QIcon(os.path.join(get_image_file_path(),"contact.png")), _("Contacts"), self)
		self.addAction(self.contacts)
		
class c_home(QToolBar):
	def __init__(self):
		QToolBar.__init__(self)
		self.setToolButtonStyle( Qt.ToolButtonTextUnderIcon)
		self.setIconSize(QSize(42, 42))
		

		self.undo = QAction(QIcon(os.path.join(get_image_file_path(),"undo.png")), _("Undo"), self)
		self.addAction(self.undo)

		self.addSeparator()

		self.run = QAction(QIcon(os.path.join(get_image_file_path(),"play.png")), _("Run\nsimulation"), self)
		self.addAction(self.run)

		self.stop = QAction(QIcon(os.path.join(get_image_file_path(),"pause.png")), _("Stop\nsimulation"), self)
		self.addAction(self.stop)

		self.addSeparator()
		
		self.scan = QAction(QIcon(os.path.join(get_image_file_path(),"scan.png")), _("Parameter\nscan"), self)
		self.addAction(self.scan)


		#self.addSeparator()
		self.fit = QAction(QIcon(os.path.join(get_image_file_path(),"fit.png")), _("Fit\ndata"), self)
		self.addAction(self.fit)
		self.fit.setVisible(False)
		
		self.addSeparator()
		
		self.plot = QAction(QIcon(os.path.join(get_image_file_path(),"plot.png")), _("Plot\nFile"), self)
		self.addAction(self.plot)

		self.time = QAction(QIcon(os.path.join(get_image_file_path(),"plot_time.png")), _("Examine results\nin time domain"), self)
		self.addAction(self.time)

		self.addSeparator()

		self.sun=tb_item_sun()
		self.addWidget(self.sun)

		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		self.addWidget(spacer)

		self.help = QAction(QIcon(os.path.join(get_image_file_path(),"man.png")), _("Help"), self)
		self.addAction(self.help)

class c_configure(QToolBar):
	def __init__(self):
		QToolBar.__init__(self)
		self.setToolButtonStyle( Qt.ToolButtonTextUnderIcon)
		self.setIconSize(QSize(42, 42))

		self.configwindow = QAction(QIcon(os.path.join(get_image_file_path(),"cog.png")), _("Configure"), self)
		self.addAction(self.configwindow)
		
		self.dump = dump_io(self)
		self.addAction(self.dump)

		self.mesh = QAction(QIcon(os.path.join(get_image_file_path(),"mesh.png")), _("Electrical\nmesh"), self)
		self.addAction(self.mesh)

	
class c_simulations(QToolBar):
	def __init__(self):
		QToolBar.__init__(self)
		self.setToolButtonStyle( Qt.ToolButtonTextUnderIcon)
		self.setIconSize(QSize(42, 42))

		self.time = QAction(QIcon(os.path.join(get_image_file_path(),"time.png")), _("Time domain\nsimulation editor."), self)
		self.addAction(self.time )


		self.fx = QAction(QIcon(os.path.join(get_image_file_path(),"spectrum.png")), _("Frequency domain\nsimulation editor"), self)
		self.addAction(self.fx)


		self.jv = QAction(QIcon(os.path.join(get_image_file_path(),"jv.png")), _("Steady state\nsimulation editor"), self)
		self.addAction(self.jv)


		self.qe = QAction(QIcon(os.path.join(get_image_file_path(),"qe.png")), _("Quantum\nefficiency"), self)
		self.addAction(self.qe)
		self.qe.setVisible(False)

		self.mode=tb_item_sim_mode()
		self.addWidget(self.mode)
		
		self.optics = QAction(QIcon(os.path.join(get_image_file_path(),"optics.png")), _("Optical\nSimulation"), self)
		self.addAction(self.optics)

		self.lasers = QAction(QIcon(os.path.join(get_image_file_path(),"lasers.png")), _("Laser\neditor"), self)
		self.addAction(self.lasers)
		
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
	
	def __init__(self):
		QTabWidget.__init__(self)
		self.setMaximumHeight(120)
		#self.setStyleSheet("QWidget {	background-color:cyan; }")

		#self.setFixedSize(-1, 300)
		#self.setWindowIcon(QIcon(os.path.join(get_image_file_path(),"jv.png")))
		#self.setWindowTitle(_("Steady state simulation")+"  (https://www.gpvdm.com)")

		self.about = QToolButton(self)
		self.about.setText("About")

		self.setCornerWidget(self.about)

		w=self.file()
		self.addTab(w,_("File"))
		
		self.home=c_home()
		self.addTab(self.home,_("Home"))
		
		self.simulations=c_simulations()
		self.addTab(self.simulations,_("Simulations"))
		
		self.configure=c_configure()
		self.addTab(self.configure,_("Configure"))
		
		self.device=c_device()
		self.addTab(self.device,"Device")

		if enable_betafeatures()==True:
			self.tb_cluster=self.cluster()
			self.addTab(self.tb_cluster,"Cluster")

		self.information=c_information()
		self.addTab(self.information,"Information")

	def init(self):
		#self.setStyleSheet("QWidget {	background-color:cyan; }") 
		aaa=self.readStyleSheet(os.path.join(get_css_path(),"style.css"))
		aaa=str(aaa,'utf-8')
		#print(aaa.decode("utf-8") ,"QWidget {	background-color:cyan; }")
		self.setStyleSheet(aaa)
