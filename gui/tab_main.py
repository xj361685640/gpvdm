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
from gl import glWidget

from PyQt5.QtWidgets import QWidget,QHBoxLayout

import i18n
_ = i18n.language.gettext

class tab_main(QWidget,tab_base):

	label_name="tab_main"

	def update(self,object):
		print("update gl")
		#self.darea.queue_draw()

	def __init__(self):
		QHBoxLayout.__init__(self)
		self.sun=1
		mainLayout = QHBoxLayout()

		self.three_d=glWidget(self)
		self.three_d.show()

		self.frame=layer_widget()

		self.frame.changed.connect(self.three_d.recalculate)

		mainLayout.addWidget(self.frame)
		mainLayout.addWidget(self.three_d)

		self.setLayout(mainLayout)

	def draw_photon(self,x_start,y_start):
		x=x_start
		y=y_start
		self.cr.set_source_rgb(0,1.0,0.0)
		self.cr.move_to(x, y)
		self.cr.set_line_width(2)
		while (y<y_start+101):
			self.cr.line_to(x+math.sin((y_start-y)/4)*10, y)
			y=y+0.1
		self.cr.stroke()

		self.cr.line_to(x+10, y)
		self.cr.line_to(x, y+20)
		self.cr.line_to(x-10, y)
		self.cr.fill()

	def draw_photon_up(self,x_start,y_start):
		x=x_start
		y=y_start
		self.cr.set_source_rgb(0.0,0.0,1.0)
		self.cr.move_to(x, y)
		self.cr.set_line_width(2)
		while (y>y_start-101):
			self.cr.line_to(x+math.sin((y_start-y)/4)*10, y)
			y=y-0.1
		self.cr.stroke()

		self.cr.line_to(x+10, y)
		self.cr.line_to(x, y-20)
		self.cr.line_to(x-10, y)
		self.cr.fill()


	def set_sun(self,sun):
		self.sun=sun

	def help(self):
		help_window().help_set_help(["device.png",_("<big><b>The device structure tab</b></big>\n Use this tab to change the structure of the device, the layer thicknesses and to perform optical simulations.  You can also browse the materials data base and  edit the electrical mesh.")])
