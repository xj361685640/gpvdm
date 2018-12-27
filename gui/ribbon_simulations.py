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

## @package ribbon_simulations
#  The main time domain ribbon.
#


import os
from icon_lib import icon_get

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

from help import help_window

#experiments
from lasers import lasers
from experiment import experiment
from fxexperiment import fxexperiment
from sunsvoc import sunsvoc
from ideal_diode_editor import ideal_diode_editor
from qe import qe_window
from optics import class_optical
from global_objects import global_object_register
from measure import measure
from cost import cost

from jvexperiment import jvexperiment
from util import wrap_text
from fdtd import fdtd

class ribbon_simulations(QToolBar):
	def __init__(self):
		QToolBar.__init__(self)

		self.experiment_window=None
		self.fxexperiment_window=None
		self.jvexperiment_window=None
		self.sunsvocexperiment_window=None
		self.optics_window=False
		self.qe_window=None
		self.lasers_window=None
		self.measure_window=None
		self.solar_spectrum_window=None
		self.cost_window=None
		self.diode_window=None
		self.fdtd_window=None

		self.setToolButtonStyle( Qt.ToolButtonTextUnderIcon)
		self.setIconSize(QSize(42, 42))

		self.time = QAction(icon_get("time"), _("Time domain\nsimulation editor."), self)
		self.time.triggered.connect(self.callback_edit_experiment_window)
		self.addAction(self.time )


		#self.fx = QAction(icon_get("spectrum"), _("Frequency domain\nsimulation editor"), self)
		#self.fx.triggered.connect(self.callback_fxexperiment_window)
		#self.addAction(self.fx)


		self.jv = QAction(icon_get("jv"), _("Steady state\nsimulation editor"), self)
		self.jv.triggered.connect(self.callback_jv_window)
		self.addAction(self.jv)

		self.sunsvoc = QAction(icon_get("sunsvoc"), _("Suns Voc\nsimulation editor"), self)
		self.sunsvoc.triggered.connect(self.callback_sunsvoc_window)
		self.addAction(self.sunsvoc)

		self.diode = QAction(icon_get("diode"), wrap_text(_("Simple diode model"),8), self)
		self.diode.triggered.connect(self.callback_diode_window)
		self.addAction(self.diode)

		self.qe = QAction(icon_get("qe"), _("Quantum\nefficiency"), self)
		self.qe.triggered.connect(self.callback_qe_window)
		self.addAction(self.qe)
		self.qe.setVisible(False)

		self.addSeparator()
		self.mode=tb_item_sim_mode()
		self.addWidget(self.mode)
		self.addSeparator()

		self.optics = QAction(icon_get("optics"), _("Optical\nSimulation"), self)
		self.optics.triggered.connect(self.callback_optics_sim)
		self.addAction(self.optics)

		self.lasers = QAction(icon_get("lasers"), _("Laser\neditor"), self)
		self.lasers.triggered.connect(self.callback_configure_lasers)
		self.addAction(self.lasers)

		self.optics = QAction(icon_get("fdtd"), _("FDTD\nSimulation"), self)
		self.optics.triggered.connect(self.callback_fdtd)
		self.addAction(self.optics)

		self.measure = QAction(icon_get("measure"), _("Measure"), self)
		self.measure.triggered.connect(self.callback_configure_measure)
		self.addAction(self.measure)

		self.tb_cost = QAction(icon_get("cost"), _("Calculate\nthe cost"), self)
		self.tb_cost.triggered.connect(self.callback_cost)
		self.addAction(self.tb_cost)

	def update(self):
		if self.qe_window!=None:
			del self.qe_window
			self.qe_window=None

		if self.lasers_window!=None:
			del self.lasers_window
			self.lasers_window=None

		if self.experiment_window!=None:
			del self.experiment_window
			self.experiment_window=None

		if self.fxexperiment_window!=None:
			del self.fxexperiment_window
			self.fxexperiment_window=None

		if self.jvexperiment_window!=None:
			del self.jvexperiment_window
			self.jvexperiment_window=None

		if self.solar_spectrum_window!=None:
			del self.solar_spectrum_window
			self.solar_spectrum_window=None

		if self.cost_window!=None:
			del self.cost_window
			self.cost_window=None

		self.mode.update()

	def setEnabled(self,val):
		self.time.setEnabled(val)
		#self.fx.setEnabled(val)
		self.jv.setEnabled(val)
		self.qe.setEnabled(val)
		self.mode.setEnabled(val)
		self.optics.setEnabled(val)
		self.lasers.setEnabled(val)
		self.tb_cost.setEnabled(val)
		self.sunsvoc.setEnabled(val)
		self.measure.setEnabled(val)

	def callback_edit_experiment_window(self):

		if self.experiment_window==None:
			self.experiment_window=experiment()
			self.experiment_window.changed.connect(self.mode.update)
			
		help_window().help_set_help(["time.png",_("<big><b>The time mesh editor</b></big><br> To do time domain simulations one must define how voltage the light vary as a function of time.  This can be done in this window.  Also use this window to define the simulation length and time step.")])
		if self.experiment_window.isVisible()==True:
			self.experiment_window.hide()
		else:
			self.experiment_window.show()
 
	def callback_fxexperiment_window(self):

		if self.fxexperiment_window==None:
			self.fxexperiment_window=fxexperiment()
			self.fxexperiment_window.changed.connect(self.mode.update)
			
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

	def callback_configure_measure(self):

		if self.measure_window==None:
			self.measure_window=measure()

		help_window().help_set_help(["measure.png",_("<big><b>Measure window</b></big><br>Use this window to set up measurement points.  If for example you want to extract the value of current density from jv.dat at 0.2 Volts, set a measurement point for jv.dat to 0.2 V.  This will work with any file")])
		if self.measure_window.isVisible()==True:
			self.measure_window.hide()
		else:
			self.measure_window.show()
			

	def callback_jv_window(self):

		if self.jvexperiment_window==None:
			self.jvexperiment_window=jvexperiment()

		help_window().help_set_help(["jv.png",_("<big><b>JV simulation editor</b></big><br> Use this window to configure the Suns Voc simulations.")])
		if self.jvexperiment_window.isVisible()==True:
			self.jvexperiment_window.hide()
		else:
			self.jvexperiment_window.show()

	def callback_sunsvoc_window(self):

		if self.sunsvocexperiment_window==None:
			self.sunsvocexperiment_window=sunsvoc()

		help_window().help_set_help(["jv.png",_("<big><b>Suns voc simulation editor</b></big><br> Use this window to select the step size and parameters of the JV simulations.")])
		if self.sunsvocexperiment_window.isVisible()==True:
			self.sunsvocexperiment_window.hide()
		else:
			self.sunsvocexperiment_window.show()

	def callback_diode_window(self):

		if self.diode_window==None:
			self.diode_window=ideal_diode_editor()

		help_window().help_set_help(["diode.png",_("<big><b>Ideal diode editor</b></big><br> Use this window to configure the ideal diode model based on the simple diode equation.")])
		if self.diode_window.isVisible()==True:
			self.diode_window.hide()
		else:
			self.diode_window.show()

	def callback_fdtd(self):

		if self.fdtd_window==None:
			self.fdtd_window=fdtd()

		help_window().help_set_help(["fdtd.png",_("<big><b>FDTD</b></big><br> Use this window to setup a finite difference time domain simulation.")])
		if self.fdtd_window.isVisible()==True:
			self.fdtd_window.hide()
		else:
			self.fdtd_window.show()
			
	def callback_optics_sim(self, widget, data=None):
		help_window().help_set_help(["optics.png",_("<big><b>The optical simulation window</b></big><br>Use this window to perform optical simulations.  Click on the play button to run a simulation."),"media-playback-start",_("Click on the play button to run an optical simulation.  The results will be displayed in the tabs to the right.")])

		if self.optics_window==False:
			self.optics_window=class_optical()
			#self.notebook.changed.connect(self.optics_window.update)

		if self.optics_window.isVisible()==True:
			self.optics_window.hide()
		else:
			global_object_register("optics_force_redraw",self.optics_window.force_redraw)
			self.optics_window.show()

			
	def callback_qe_window(self, widget):
		if self.qe_window==None:
			self.qe_window=qe_window()

		if self.qe_window.isVisible()==True:
			self.qe_window.hide()
		else:
			self.qe_window.show()

	def callback_cost(self):
		help_window().help_set_help(["cost.png",_("<big><b>Costs window</b></big>\nUse this window to calculate the cost of the solar cell and the energy payback time.")])

		if self.cost_window==None:
			self.cost_window=cost()

		if self.cost_window.isVisible()==True:
			self.cost_window.hide()
		else:
			self.cost_window.show()
