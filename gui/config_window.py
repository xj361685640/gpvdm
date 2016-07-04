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



import pygtk
pygtk.require('2.0')
import gtk
#import sys
import os
#import shutil
#from numpy import *
#from matplotlib.figure import Figure
#from numpy import arange, sin, pi
#import gobject
from tab import tab_class
from window_list import windows
from cal_path import get_image_file_path

articles = []
mesh_articles = []

class class_config_window(gtk.Window):


	def init(self):
		self.set_size_request(900, 600)
		self.set_icon_from_file(os.path.join(get_image_file_path(),"cog.png"))
		self.set_title("Configure")

		main_vbox = gtk.VBox(False, 3)

		toolbar = gtk.Toolbar()
		toolbar.set_style(gtk.TOOLBAR_ICONS)
		toolbar.set_size_request(-1, 70)

		sep = gtk.SeparatorToolItem()
		sep.set_draw(False)
		sep.set_expand(True)
		toolbar.insert(sep, -1)

		image = gtk.Image()
		image.set_from_file(os.path.join(get_image_file_path(),"help.png"))
		delete = gtk.ToolButton(image)
		#delete.connect("clicked", self.callback_delete_page,None)
		#self.tooltips.set_tip(delete, _("Delete simulation"))
		toolbar.insert(delete, -1)


		main_vbox.pack_start(toolbar, False, True, 0)

		self.notebook = gtk.Notebook()
		main_vbox.pack_start(self.notebook, True, True, 0)
		self.notebook.set_tab_pos(gtk.POS_TOP)
		self.notebook.show()

		for child in self.notebook.get_children():
				self.notebook.remove(child)

		files=["math.inp","dump.inp"]
		description=["Math","Dump"]


		for i in range(0,len(files)):
			tab=tab_class()
			tab.show()
			tab.visible=True

			tab.init(files[i],description[i])
			tab.label_name=description[i]
			self.notebook.append_page(tab, gtk.Label(description[i]))

		self.win_list=windows()
		self.win_list.load()
		self.win_list.set_window(self,"config_window")

		self.add(main_vbox)

		self.connect("delete-event", self.callback_close_window) 

		self.hide()

	def callback_close_window(self, widget, data=None):
		self.win_list.update(self,"config_window")
		self.hide()
		return True



