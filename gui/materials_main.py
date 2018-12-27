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

## @package materials_main
#  Dialog to show information about a material.
#

import os
from tab import tab_class
from icon_lib import icon_get

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QDialog
from PyQt5.QtGui import QPainter,QIcon

#python modules
import webbrowser

from help import help_window

from equation import equation
from win_lin import desktop_open

from ref import ref_window
from ref import get_ref_text
from ref_io import ref

from gpvdm_open import gpvdm_open


from QWidgetSavePos import QWidgetSavePos
from plot_widget import plot_widget

from ribbon_materials import ribbon_materials
from import_data import import_data


articles = []
mesh_articles = []

class materials_main(QWidgetSavePos):

	def changed_click(self):
		if self.notebook.tabText(self.notebook.currentIndex()).strip()==_("Electrical parameters"):
			help_window().help_set_help(["tab.png",_("<big><b>Electrical parameters</b></big><br>Use this tab to configure the electrical parameters for the material.")])
			self.ribbon.tb_save.setEnabled(False)
			self.ribbon.import_data.setEnabled(False)

		if self.notebook.tabText(self.notebook.currentIndex()).strip()==_("Luminescence"):
			help_window().help_set_help(["tab.png",_("<big><b>Luminescence</b></big><br>Use this tab to edit the materials Luminescence.")])
			self.ribbon.tb_save.setEnabled(False)
			self.ribbon.import_data.setEnabled(False)

		if self.notebook.tabText(self.notebook.currentIndex()).strip()==_("Absorption"):
			text=get_ref_text(os.path.join(self.path,"alpha.ref"))
			if text==None:
				text=""
			help_window().help_set_help(["alpha.png",_("<big><b>Absorption</b></big><br>"+text)])
			self.ribbon.tb_save.setEnabled(True)
			self.ribbon.import_data.setEnabled(True)

		if self.notebook.tabText(self.notebook.currentIndex()).strip()==_("Refractive index"):
			text=get_ref_text(os.path.join(self.path,"n.ref"))
			if text==None:
				text=""
			help_window().help_set_help(["n.png",_("<big><b>Refractive index</b></big><br>"+text)])
			self.ribbon.tb_save.setEnabled(True)
			self.ribbon.import_data.setEnabled(True)

	def callback_cost(self):
		desktop_open(os.path.join(self.path,"cost.xlsx"))

	def callback_help(self):
		webbrowser.open("https://www.gpvdm.com/man/index.html")


	def __init__(self,path):
		QWidgetSavePos.__init__(self,"materials_main")
		self.path=path
		self.setFixedSize(900, 600)
		self.setWindowIcon(icon_get("organic_material"))

		self.setWindowTitle(_("Material editor")+" (https://www.gpvdm.com)"+" "+os.path.basename(self.path)) 
		

		self.main_vbox = QVBoxLayout()

		self.ribbon=ribbon_materials()
		
		self.ribbon.cost.triggered.connect(self.callback_cost)
		self.ribbon.folder_open.triggered.connect(self.callback_dir_open)
		self.ribbon.import_data.triggered.connect(self.import_data)
		self.ribbon.tb_ref.triggered.connect(self.callback_ref)

		self.ribbon.help.triggered.connect(self.callback_help)


		self.main_vbox.addWidget(self.ribbon)

		self.notebook = QTabWidget()

		self.notebook.setMovable(True)

		self.main_vbox.addWidget(self.notebook)

		#alpha=equation(self.path,"alpha_eq.inp","alpha_gen.omat","alpha.omat","#mat_default_file_alpha")
		#alpha.set_default_value("1e7")
		#alpha.set_ylabel(_("Absorption")+" (m^{-1})")
		#alpha.init()

		fname=os.path.join(self.path,"alpha.omat")
		self.alpha=plot_widget()
		self.alpha.init(enable_toolbar=False)
		self.alpha.set_labels([_("Absorption")])
		self.alpha.load_data([fname],os.path.splitext(fname)[0]+".oplot")

		self.alpha.do_plot()
		self.notebook.addTab(self.alpha,_("Absorption"))

		fname=os.path.join(self.path,"n.omat")
		self.n=plot_widget()
		self.n.init(enable_toolbar=False)
		self.n.set_labels([_("Refractive index")])
		self.n.load_data([fname],os.path.splitext(fname)[0]+".oplot")
		self.n.do_plot()

		self.notebook.addTab(self.n,_("Refractive index"))


		files=["dos.inp","pl.inp","mat.inp"]
		description=[_("Electrical parameters"),_("Luminescence"),_("Basic")]


		for i in range(0,len(files)):
			tab=tab_class()
			full_path=os.path.join(self.path,files[i])
			if os.path.isfile(full_path)==True:
				tab.init(os.path.join(self.path,files[i]),description[i])
				self.notebook.addTab(tab,description[i])
		self.setLayout(self.main_vbox)
		
		self.notebook.currentChanged.connect(self.changed_click)

	def import_data(self):
		file_name=None
		if self.notebook.tabText(self.notebook.currentIndex()).strip()==_("Absorption"):
			file_name="alpha.omat"

		if self.notebook.tabText(self.notebook.currentIndex()).strip()==_("Refractive index"):
			file_name="n.omat"

		if file_name!=None:
			output_file=os.path.join(self.path,file_name)
			config_file=os.path.join(self.path,file_name+"import.inp")
			self.im=import_data(output_file,config_file)
			self.im.run()
			self.update()

	def import_ref(self):
		file_name=None
		if self.notebook.tabText(self.notebook.currentIndex()).strip()==_("Absorption"):
			file_name="alpha.omat"

		if self.notebook.tabText(self.notebook.currentIndex()).strip()==_("Refractive index"):
			file_name="n.omat"

		if file_name!=None:
			output_file=os.path.join(self.path,file_name)
			config_file=os.path.join(self.path,file_name+"import.inp")
			self.im=import_data(output_file,config_file)
			self.im.run()
			self.update()

	def update(self):
		self.n.update()
		self.alpha.update()

	def callback_ref(self):
		file_name=None
		if self.notebook.tabText(self.notebook.currentIndex()).strip()==_("Absorption"):
			file_name="alpha.omat"

		if self.notebook.tabText(self.notebook.currentIndex()).strip()==_("Refractive index"):
			file_name="n.omat"

		if file_name!=None:
			self.ref_window=ref_window(os.path.join(self.path,file_name))
			self.ref_window.show()

	def callback_dir_open(self):
		dialog=gpvdm_open(self.path)
		dialog.show_inp_files=False
		ret=dialog.exec_()

		if ret==QDialog.Accepted:
			desktop_open(dialog.get_filename())
