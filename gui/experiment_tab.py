#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2016 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
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
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QTabWidget
from PyQt5.QtGui import QPainter,QIcon

class experiment_tab(QTabWidget):

	def update(self):
		self.tmesh.update()

	def __init__(self,index):
		QTabWidget.__init__(self)

		self.index=index
		lines=[]

		if inp_load_file(lines,"pulse"+str(self.index)+".inp")==True:
			self.tab_name=inp_search_token_value(lines, "#sim_menu_name")
		else:
			self.tab_name=""


		self.setTabsClosable(True)
		self.setMovable(True)

		self.tmesh = tab_time_mesh(self.index)
		self.addTab(self.tmesh,_("time mesh"))


		self.circuit=circuit(self.index)

		self.addTab(self.circuit,_("Circuit"))

		tab=tab_class()
		tab.init("pulse"+str(self.index)+".inp",_("Configure"))
		self.addTab(tab,_("Configure"))


	def set_tab_caption(self,name):
		mytext=name
		if len(mytext)<10:
			for i in range(len(mytext),10):
				mytext=mytext+" "
		self.label.set_text(mytext)

	def rename(self,tab_name):
		self.tab_name=tab_name+"@"+self.tab_name.split("@")[1]
		inp_update_token_value("pulse"+str(self.index)+".inp", "#sim_menu_name", self.tab_name,1)


