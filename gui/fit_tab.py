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
from inp import inp_update_token_value
from cal_path import get_image_file_path
from tab import tab_class
from fit_patch import fit_patch
import shutil

import i18n
_ = i18n.language.gettext

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QMenuBar,QStatusBar, QMenu, QTableWidget, QAbstractItemView
from PyQt5.QtGui import QPainter,QIcon,QCursor

#windows
from fit_window_plot import fit_window_plot
from fit_window_plot_real import fit_window_plot_real

from gui_util import save_as_filter

mesh_articles = []

class fit_tab(QTabWidget):

	def update(self):
		self.tmesh_real.update()
		self.tmesh.update()

	def __init__(self,index):
		QTabWidget.__init__(self)
		lines=[]
		self.index=index

		if inp_load_file(lines,"fit"+str(self.index)+".inp")==True:
			self.tab_name=inp_search_token_value(lines, "#fit_name")
		else:
			self.tab_name=""

		self.setTabsClosable(True)
		self.setMovable(True)

		self.tmesh = fit_window_plot(self.index)
		self.addTab(self.tmesh,_("Fit error"))

		self.tmesh_real = fit_window_plot_real(self.index)
		self.addTab(self.tmesh_real,_("Experimental data"))

		self.fit_patch = fit_patch(self.index)
		self.addTab(self.fit_patch, _("Fit patch"))

		config=tab_class()
		config.init("fit"+str(self.index)+".inp",self.tab_name)
		self.addTab(config,_("Configure fit"))

		
	def init(self,index):
		return

	def set_tab_caption(self,name):
		mytext=name
		if len(mytext)<10:
			for i in range(len(mytext),10):
				mytext=mytext+" "
		self.label.set_text(mytext)

	def rename(self,tab_name):
		inp_update_token_value("fit"+str(self.index)+".inp", "#fit_name", self.tab_name,1)
		self.set_tab_caption(self.tab_name)

	def import_data(self):
		file_name=save_as_filter(parent,"dat (*.dat);;csv (*.csv)")
		if file_name!=None:
			print("importing file",file_name)
			shutil.copy(file_name, os.path.join(os.getcwd(),"fit_data"+str(self.index)+".inp"))
			self.update()
