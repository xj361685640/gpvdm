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



import os
from cal_path import get_image_file_path
from tb_pulse_load_type import tb_pulse_load_type

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QMenuBar,QStatusBar
from PyQt5.QtGui import QPainter,QIcon,QPixmap


class circuit(QWidget):

	def update(self,object):
		self.darea.queue_draw()

	def __init__(self,index):
		QWidget.__init__(self)

		vbox=QVBoxLayout()


		self.index=index

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))


		self.load_type=tb_pulse_load_type(self.index)
		#self.load_type.connect("changed", self.draw_callback)

		toolbar.addWidget(self.load_type)
		vbox.addWidget(toolbar)

		self.diode = QPixmap(os.path.join(get_image_file_path(),"diode.png"))
		self.ideal_diode = QPixmap(os.path.join(get_image_file_path(),"ideal_diode.png"))
		self.load = QPixmap(os.path.join(get_image_file_path(),"load.png"))
		self.ideal_load = QPixmap(os.path.join(get_image_file_path(),"ideal_load.png"))
		self.voc = QPixmap(os.path.join(get_image_file_path(),"voc.png"))

		self.darea = QWidget()

		vbox.addWidget(self.darea)

		self.setLayout(vbox)
		return



	def draw_callback(self,sender):
		self.darea.queue_draw()
		#self.cr = self.darea.window.cairo_create()
		#self.draw()

	def draw(self):
		x=40
		y=40
		self.cr.set_source_rgb(0.0,0.0,0.0)
		self.cr.set_font_size(14)


		if self.load_type.sim_mode.get_active_text()=="load":
			self.cr.move_to(x+130, y+120)
			self.cr.show_text("C=")

			self.cr.move_to(x+250, y+120)
			self.cr.show_text("Rshunt=")

			self.cr.move_to(x+250, y+25)
			self.cr.show_text("Rcontact=")


			self.cr.set_source_pixbuf(self.diode, x, y)
			self.cr.paint()

			self.cr.set_source_pixbuf(self.load, x+610, y+67)
			self.cr.paint()
			self.cr.move_to(x+550, y+150)
			self.cr.show_text("Rload=")
		elif self.load_type.sim_mode.get_active_text()=="ideal_diode_ideal_load":
			self.cr.set_source_pixbuf(self.ideal_diode, x, y)
			self.cr.paint()
			self.cr.set_source_pixbuf(self.ideal_load, x+610, y+67)
			self.cr.paint()
		else:
			self.cr.move_to(x+130, y+120)
			self.cr.show_text("C=")

			self.cr.move_to(x+250, y+120)
			self.cr.show_text("Rshunt=")

			self.cr.move_to(x+250, y+25)
			self.cr.show_text("Rcontact=")

			self.cr.set_source_pixbuf(self.diode, x, y)
			self.cr.paint()
			self.cr.set_source_pixbuf(self.voc, x+610, y+57)
			self.cr.paint()


	def expose(self, widget, event):
		self.cr = self.darea.window.cairo_create()
		self.draw()

