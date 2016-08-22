#!/usr/bin/env python2.7
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



import math
from layer_widget import layer_widget
from util import read_xyz_data
import os
from cal_path import get_materials_path
from inp import inp_load_file
from inp_util import inp_search_token_value
from util import str2bool
from tab_base import tab_base
from epitaxy import epitaxy_get_layers
from epitaxy import epitaxy_get_width
from epitaxy import epitaxy_get_mat_file
from epitaxy import epitaxy_get_electrical_layer
from help import help_window
from epitaxy import epitaxy_get_pl_file
from epitaxy import epitaxy_get_name
from display import display_widget
from win_lin import running_on_linux
from code_ctrl import enable_webupdates
from update import update_thread

from PyQt5.QtWidgets import QWidget,QHBoxLayout

import i18n
_ = i18n.language.gettext

class tab_main(QWidget,tab_base):

	label_name="tab_main"

	
	def update(self):
		self.three_d.recalculate()

	def __init__(self):
		QHBoxLayout.__init__(self)
		self.sun=1
		mainLayout = QHBoxLayout()

		self.three_d=display_widget()
		self.three_d.show()

		self.frame=layer_widget()

		self.frame.changed.connect(self.three_d.recalculate)
		
		mainLayout.addWidget(self.frame)
		mainLayout.addWidget(self.three_d)

		self.setLayout(mainLayout)

		if enable_webupdates()==True:
			print("Looking for updates")
			self.web_update=update_thread()
			self.web_update.got_data.connect(self.got_help)
			self.web_update.start()
		self.frame.tab.itemSelectionChanged.connect(self.layer_selection_changed)

	def layer_selection_changed(self):
		a=self.frame.tab.selectionModel().selectedRows()

		if len(a)>0:
			y=a[0].row()
		else:
			y=-1
		
		self.three_d.set_selected_layer(y)
		self.three_d.update()
		
	def help(self):
		help_window().help_set_help(["device.png",_("<big><b>The device structure tab</b></big>\n Use this tab to change the structure of the device, the layer thicknesses and to perform optical simulations.  You can also browse the materials data base and  edit the electrical mesh.")])

	def got_help(self,data):
		if data!="":
			help_window().help_append(["star.png",_("<big><b>Update available!</b></big><br>"+data)])
