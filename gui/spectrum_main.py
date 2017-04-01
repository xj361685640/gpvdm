#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2017 Edward Grant  eayeg3 at nottingham.ac.uk
#    Copyright (C) 2017 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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


import sys
from PyQt5.QtWidgets import QMenuBar, QWidget, QApplication, QAction,QDesktopWidget,QTabWidget,QVBoxLayout
from PyQt5.QtGui import QIcon

import os

from spectrum_planet import planet

class spectrum_main(QWidget):
	def __init__(self):
		super().__init__()
		self.resize(1200,600)
		self.vbox=QVBoxLayout()
		exitAction = QAction(QIcon(os.path.join(os.getcwd(), 'icons', 'exit.png')), 'Exit', self)
		exitAction.setShortcut('Ctrl+Q')
		exitAction.setStatusTip('Exit application')
		exitAction.triggered.connect(self.close)

		saveFigureAction =  QAction(QIcon(os.path.join(os.getcwd(), 'icons', 'document-save-as.png')), 'Save Figure', self)
		saveFigureAction.setShortcut('Ctrl+S')
		saveFigureAction.triggered.connect(self.save)

		copyFigureAction =  QAction(QIcon(os.path.join(os.getcwd(), 'icons', 'copy.png')), 'Copy Figure', self)
		copyFigureAction.setShortcut('Ctrl+C')
		copyFigureAction.triggered.connect(self.copy)

		# plotPreferencesAction =  QAction(QIcon(os.path.join(os.getcwd(), 'icons', 'prefs.png')), 'Plot Preferences', self)
		# plotPreferencesAction.triggered.connect(self.setPlotPreferences)

		self.menubar = QMenuBar(self)
		fileMenu = self.menubar.addMenu('&File')
		fileMenu.addAction(exitAction)
		fileMenu.addAction(saveFigureAction)
		fileMenu.addAction(copyFigureAction)
		self.vbox.addWidget(self.menubar)
		# optionsMenu = menubar.addMenu('&Options')
		# optionsMenu.addAction(plotPreferencesAction)

		self.setWindowTitle('Solar Spectrum Planetary Model')
		self.center()

		self.tabs = QTabWidget()

		self.vbox.addWidget(self.tabs)

		earth = planet()
		earth.set_earth(True)
		earth.init()
		self.tabs.addTab(earth,"Earth")

		mercury = planet()
		mercury.set_mercury(True)
		mercury.set_orbitalpoint(True)
		mercury.init()
		self.tabs.addTab(mercury,"Mercury")

		venus = planet()
		venus.set_venus(True)
		venus.set_orbitalpoint(True)
		venus.init()
		self.tabs.addTab(venus,"Venus")

		mars = planet()
		mars.set_mars(True)
		mars.set_orbitalpoint(True)
		mars.init()
		self.tabs.addTab(mars,"Mars")

		ceres = planet()
		ceres.set_ceres(True)
		ceres.set_orbitalpoint(True)
		ceres.init()
		self.tabs.addTab(ceres, "Ceres (Dwarf Planet)")

		europa = planet()
		europa.set_europa(True)
		europa.set_orbitalpoint(True)
		europa.init()
		self.tabs.addTab(europa, "Europa (moon of Jupiter)")

		halley = planet()
		halley.set_halley(True)
		halley.set_orbitalpoint(True)
		halley.init()
		self.tabs.addTab(halley, "Halley's Comet")

		pluto = planet()
		pluto.set_pluto(True)
		pluto.set_orbitalpoint(True)
		pluto.init()
		self.tabs.addTab(pluto, "Pluto")
		
		self.setLayout(self.vbox)


	# def setPlotPreferences(self):
	#     self.tabs.currentWidget().setPlotPreferences()

	def save(self):
		self.tabs.currentWidget().save()

	def copy(self):
		self.tabs.currentWidget().copy2clip()

	def center(self):
		qr = self.frameGeometry()
		cp = QDesktopWidget().availableGeometry().center()
		qr.moveCenter(cp)
		self.move(qr.topLeft())


if __name__ == '__main__':
	app = QApplication(sys.argv)
	win = spectrum_main()
	sys.exit(app.exec_())
