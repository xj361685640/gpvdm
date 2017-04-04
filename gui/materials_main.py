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
from tab import tab_class
from window_list import windows
from icon_lib import QIcon_load

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget
from PyQt5.QtGui import QPainter,QIcon

#python modules
import webbrowser

from help import help_window

from equation import equation
from win_lin import desktop_open

from ref import get_ref_text

articles = []
mesh_articles = []

class materials_main(QWidget):

	def changed_click(self):
		if self.notebook.tabText(self.notebook.currentIndex()).strip()==_("Electrical parameters"):
			help_window().help_set_help(["tab.png",_("<big><b>Electrical parameters</b></big><br>Use this tab to configure the electrical parameters for the material.")])

		if self.notebook.tabText(self.notebook.currentIndex()).strip()==_("Luminescence"):
			help_window().help_set_help(["tab.png",_("<big><b>Luminescence</b></big><br>Use this tab to edit the materials Luminescence.")])

		if self.notebook.tabText(self.notebook.currentIndex()).strip()==_("Absorption"):
			text=get_ref_text(os.path.join(self.path,"alpha.ref"))
			if text==None:
				text=""
			help_window().help_set_help(["alpha.png",_("<big><b>Absorption</b></big><br>"+text)])

		if self.notebook.tabText(self.notebook.currentIndex()).strip()==_("Refractive index"):
			text=get_ref_text(os.path.join(self.path,"n.ref"))
			if text==None:
				text=""
			help_window().help_set_help(["n.png",_("<big><b>Refractive index</b></big><br>"+text)])

	def callback_cost(self):
		desktop_open(os.path.join(self.path,"cost.xlsx"))

	def callback_help(self):
		webbrowser.open("https://www.gpvdm.com/man/index.html")

	def __init__(self,path):
		QWidget.__init__(self)
		self.path=path
		self.setFixedSize(900, 600)
		self.setWindowIcon(QIcon_load("organic_material"))

		self.setWindowTitle(_("Material editor")+" (https://www.gpvdm.com)"+" "+os.path.basename(self.path)) 
		

		self.main_vbox = QVBoxLayout()

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))

		self.cost = QAction(QIcon_load("cost"), 'Hide', self)
		self.cost.setStatusTip(_("Cost of material"))
		self.cost.triggered.connect(self.callback_cost)
		toolbar.addAction(self.cost)
		
		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		toolbar.addWidget(spacer)


		self.help = QAction(QIcon_load("help"), 'Hide', self)
		self.help.setStatusTip(_("Help"))
		self.help.triggered.connect(self.callback_help)
		toolbar.addAction(self.help)

		self.main_vbox.addWidget(toolbar)


		self.notebook = QTabWidget()

		self.notebook.setMovable(True)

		self.main_vbox.addWidget(self.notebook)


		files=["dos.inp","pl.inp","mat.inp"]
		description=[_("Electrical parameters"),_("Luminescence"),_("Basic")]


		for i in range(0,len(files)):
			tab=tab_class()
			tab.init(os.path.join(self.path,files[i]),description[i])
			self.notebook.addTab(tab,description[i])

		alpha=equation(self.path,"alpha_eq.inp","alpha_gen.omat","alpha.omat","#mat_default_file_alpha")
		alpha.set_default_value("1e7")
		alpha.set_ylabel(_("Absorption")+" (m^{-1})")
		alpha.init()
		self.notebook.addTab(alpha,"Absorption")

		n=equation(self.path,"n_eq.inp","n_gen.omat","n.omat","#mat_default_file_n")
		n.set_default_value("3")
		n.set_ylabel(_("Refractive index")+" (au)")
		n.init()
		self.notebook.addTab(n,_("Refractive index"))


		self.setLayout(self.main_vbox)
		self.win_list=windows()
		self.win_list.load()
		self.win_list.set_window(self,"materials_window")
		
		self.notebook.currentChanged.connect(self.changed_click)

	def closeEvent(self, event):
		self.win_list.update(self,"materials_window")
		self.hide()
		event.accept()



