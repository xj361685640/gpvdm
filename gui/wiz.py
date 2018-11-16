#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2917 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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
from util import gpvdm_delete_file
#from global_objects import global_object_get
from plot_io import get_plot_file_info
from dat_file_class import dat_file

#qt
from PyQt5.QtCore import QSize, Qt
from PyQt5.QtWidgets import QMenu,QTabWidget,QAbstractItemView,QAction,QToolBar,QDialog,QVBoxLayout,QDialog,QWidget,QLineEdit

#cal_path
from icon_lib import icon_get
from cal_path import get_ui_path

from help import help_window

from error_dlg import error_dlg

from ref import get_ref_text
from gui_util import dlg_get_text
from gui_util import yes_no_dlg

from clone import clone_material
from clone import clone_spectra
from cal_path import get_base_material_path
from cal_path import get_base_spectra_path

from inp import inp_get_token_value
from util import isfiletype
from win_lin import desktop_open
from gpvdm_viewer import gpvdm_viewer

from util import wrap_text

COL_PATH = 0
COL_PIXBUF = 1
COL_IS_DIRECTORY = 2

import i18n
_ = i18n.language.gettext

#util
from util import latex_to_html

class wiz(QDialog):

	def __init__(self):
		QWidget.__init__(self)
		self.vbox=QVBoxLayout()
		self.setLayout(self.vbox)

		self.notebook = QTabWidget()
		self.notebook.setMovable(True)
		self.vbox.addWidget(self.notebook)

		self.a=QWidget()
		self.notebook.addTab(self.a,_("aaa"))

		self.b=QWidget()
		self.notebook.addTab(self.b,_("bbb"))


