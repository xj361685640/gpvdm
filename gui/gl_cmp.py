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
from inp import inp_update_token_value
from inp import inp_write_lines_to_file
from inp import inp_load_file
from inp_util import inp_search_token_value
from plot_widget import plot_widget
from window_list import windows
from plot_state import plot_state
from plot_io import plot_load_info
from cal_path import get_exe_command
from cal_path import get_image_file_path
import webbrowser

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget
from PyQt5.QtGui import QPainter,QIcon

from snapshot_slider import snapshot_slider

class gl_cmp(QWidget):

	def __init__(self):
		QWidget.__init__(self)
		self.setWindowTitle(_("Results viewer")) 
		
		self.main_vbox = QVBoxLayout()

		self.slider=snapshot_slider(os.path.join(os.getcwd(),"snapshots"))

		self.main_vbox.addWidget(self.slider)

		self.setLayout(self.main_vbox)
