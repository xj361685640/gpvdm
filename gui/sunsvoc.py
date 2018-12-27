#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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

## @package sunsvoc
#  An editor for simple simulations
#

import os
from tab import tab_class
from icon_lib import icon_get

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget
from PyQt5.QtGui import QPainter,QIcon

#python modules
import webbrowser

from help import help_window
from QWidgetSavePos import QWidgetSavePos
from css import css_apply

articles = []
mesh_articles = []

class sunsvoc(QWidgetSavePos):

	def changed_click(self):
		if self.notebook.tabText(self.notebook.currentIndex()).strip()==_("JV simulation"):
			help_window().help_set_help(["jv.png",_("<big><b>JV simulation</b></big><br>This controls the jv curve simulation. You can set paramets such as start voltage, stop voltage and voltage step.")])

		if self.notebook.tabText(self.notebook.currentIndex()).strip()==_("Diode equation"):
			help_window().help_set_help(["jv.png",_("<big><b>Diode equation</b></big><br>The model can also use the standard diode equtaion to simulate solar cells (rather than a complex model).  Use this window to configure the simulation.")])
			
		if self.notebook.tabText(self.notebook.currentIndex()).strip()==_("Suns v.s. Voc"):
			help_window().help_set_help(["jv.png",_("<big><b>Suns v.s. Voc</b></big><br>Configure the sun v.s. Voc plot.")])

	def callback_help(self):
		webbrowser.open('http://www.gpvdm.com/man/index.html')

	def __init__(self):
		QWidgetSavePos.__init__(self,"sunsvoc")
		self.setFixedSize(900, 600)
		self.setWindowIcon(icon_get("sunsvoc"))

		self.setWindowTitle(_("Steady state simulation")+"  (https://www.gpvdm.com)") 
		

		self.main_vbox = QVBoxLayout()

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))

		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		toolbar.addWidget(spacer)


		self.help = QAction(icon_get("help"), _("Help"), self)
		self.help.setStatusTip(_("Close"))
		self.help.triggered.connect(self.callback_help)
		toolbar.addAction(self.help)

		self.main_vbox.addWidget(toolbar)


		self.notebook = QTabWidget()
		css_apply(self.notebook,"tab_default.css")
		self.notebook.setMovable(True)

		self.main_vbox.addWidget(self.notebook)


		files=["sun_voc.inp"]
		description=[_("Suns v.s. Voc")]


		for i in range(0,len(files)):
			tab=tab_class()
			tab.init(files[i],description[i])
			self.notebook.addTab(tab,description[i])


		self.setLayout(self.main_vbox)
		
		self.notebook.currentChanged.connect(self.changed_click)



