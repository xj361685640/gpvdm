#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
#
#	www.gpvdm.com
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
from inp import inp_load_file
from inp_util import inp_search_token_value
from tmesh import tab_time_mesh
from circuit import circuit
from inp import inp_update_token_value
from cal_path import get_image_file_path
from tab import tab_class


import i18n
_ = i18n.language.gettext

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget
from PyQt5.QtGui import QPainter,QIcon

class experiment_tab(QWidget):

	def update(self):
		self.tmesh.update()

	def init(self,index):
		self.tab_label=None


		self.index=index
		lines=[]

		if inp_load_file(lines,"pulse"+str(self.index)+".inp")==True:
			self.tab_name=inp_search_token_value(lines, "#sim_menu_name")
		else:
			self.tab_name=""

#
		self.title_hbox=gtk.HBox()

		self.title_hbox.set_size_request(-1, 25)
		self.label=gtk.Label()
		self.label.set_justify(gtk.JUSTIFY_LEFT)
		self.title_hbox.pack_start(self.label, False, True, 0)

		self.close_button = gtk.Button()
		close_image = gtk.Image()
   		close_image.set_from_file(os.path.join(get_image_file_path(),"close.png"))
		close_image.show()
		self.close_button.add(close_image)
		self.close_button.props.relief = gtk.RELIEF_NONE

		self.close_button.set_size_request(25, 25)
		self.close_button.show()

		self.title_hbox.pack_end(self.close_button, False, False, 0)
		self.title_hbox.show_all()

		self.notebook=gtk.Notebook()
		self.notebook.show()
		self.tmesh = tab_time_mesh()
		self.tmesh.init(self.index)

		self.notebook.append_page(self.tmesh, gtk.Label(_("time mesh")))

		self.pack_start(self.notebook, False, False, 0)

		self.circuit=circuit()
		self.circuit.init(self.index)

		self.notebook.append_page(self.circuit, gtk.Label(_("Circuit")))

		config=tab_class()
		config.show()
		self.notebook.append_page(config,gtk.Label("Config"))
		config.visible=True
		config.init("pulse"+str(self.index)+".inp",self.tab_name)
		config.label_name=self.tab_name
		self.show()

	def set_tab_caption(self,name):
		mytext=name
		if len(mytext)<10:
			for i in range(len(mytext),10):
				mytext=mytext+" "
		self.label.set_text(mytext)

	def rename(self,tab_name):
		self.tab_name=tab_name+"@"+self.tab_name.split("@")[1]
		inp_update_token_value("pulse"+str(self.index)+".inp", "#sim_menu_name", self.tab_name,1)
		self.set_tab_caption(self.tab_name.split("@")[0])


