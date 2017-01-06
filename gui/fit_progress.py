#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2016 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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
from inp import inp_update_token_value
from cal_path import get_image_file_path
from fit_patch import fit_patch
import shutil

import i18n
_ = i18n.language.gettext

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QMenuBar,QStatusBar, QMenu, QTableWidget, QAbstractItemView
from PyQt5.QtGui import QPainter,QIcon,QCursor

#windows
from gui_util import save_as_filter
from plot_widget import plot_widget

mesh_articles = []

class fit_progress(QTabWidget):

	def update(self):
		self.plot_fit_speed.do_plot()

	def __init__(self):
		QTabWidget.__init__(self)

		self.setMovable(True)

		self.plot_fit_speed=plot_widget()
		self.plot_fit_speed.init()
		self.addTab(self.plot_fit_speed,_("Fit speed"))
		self.plot_fit_speed.set_labels(["Fit speed"])
		print("about to load data")
		self.plot_fit_speed.load_data(["fitlog_time_error.dat"],"fitlog_time_error.oplot")
		self.plot_fit_speed.do_plot()

	def rename(self,tab_name):
		return
