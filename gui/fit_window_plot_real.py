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

#import sys
import os
#import shutil
from numpy import *
from matplotlib.figure import Figure
#from numpy import arange, sin, pi
from matplotlib.backends.backend_gtkagg import FigureCanvasGTKAgg as FigureCanvas
#import gobject
from scan_item import scan_item_add
from inp import inp_load_file
from inp import inp_read_next_item
from gui_util import dlg_get_text
from inp import inp_get_token_value
#import matplotlib.mlab as mlab
from inp import inp_write_lines_to_file
import webbrowser
from util import time_with_units
from inp_util import inp_search_token_value
from cal_path import get_image_file_path
from scan_item import scan_remove_file
from code_ctrl import enable_betafeatures
from matplotlib_toolbar import NavigationToolbar
from util import read_xyz_data

import i18n
_ = i18n.language.gettext


mesh_articles = []


class fit_window_plot_real(gtk.VBox):
	lines=[]
	edit_list=[]

	line_number=[]
	save_file_name=""

	file_name=""
	name=""
	visible=1

	def update(self):
		self.draw_graph()
		self.fig.canvas.draw()

	def draw_graph(self):

		x=[]
		y=[]
		z=[]
		if read_xyz_data(x,y,z,"fit_data"+str(self.index)+".inp")==True:

			self.fig.clf()
			self.fig.subplots_adjust(bottom=0.2)
			self.fig.subplots_adjust(left=0.1)
			self.ax1 = self.fig.add_subplot(111)
			self.ax1.ticklabel_format(useOffset=False)

			self.ax1.set_xlabel(_("x"))
			self.ax1.set_ylabel(_("y"))

			voltage, = self.ax1.plot(x,y, 'ro-', linewidth=3 ,alpha=1.0)


	def save_image(self,file_name):
		self.fig.savefig(file_name)

	def callback_save(self, widget, data=None):
		dialog = gtk.FileChooserDialog(_("Save as.."),
                               None,
                               gtk.FILE_CHOOSER_ACTION_SAVE,
                               (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                                gtk.STOCK_SAVE, gtk.RESPONSE_OK))
		dialog.set_default_response(gtk.RESPONSE_OK)

		filter = gtk.FileFilter()
		filter.set_name(".jpg")
		filter.add_pattern("*.jpg")
		dialog.add_filter(filter)

		response = dialog.run()
		if response == gtk.RESPONSE_OK:
			file_name=dialog.get_filename()

			if os.path.splitext(file_name)[1]:
				self.save_image(file_name)
			else:
				filter=dialog.get_filter()
				self.save_image(file_name+filter.get_name())

		elif response == gtk.RESPONSE_CANCEL:
		    print _("Closed, no files selected")
		dialog.destroy()


	def init(self,index):
		self.index=index
		self.fig = Figure(figsize=(5,4), dpi=100)
		self.ax1=None
		self.show_key=True
		self.hbox=gtk.HBox()
		self.edit_list=[]
		self.line_number=[]

		self.list=[]
		print "index=",index


		canvas = FigureCanvas(self.fig)  # a gtk.DrawingArea
		#canvas.set_background('white')
		#canvas.set_facecolor('white')
		canvas.figure.patch.set_facecolor('white')
		canvas.set_size_request(500, 150)
		canvas.show()

		tooltips = gtk.Tooltips()

		toolbar = gtk.Toolbar()
		#toolbar.set_orientation(gtk.ORIENTATION_VERTICAL)
		toolbar.set_style(gtk.TOOLBAR_ICONS)
		toolbar.set_size_request(-1, 70)

		image = gtk.Image()
   		image.set_from_file(os.path.join(get_image_file_path(),"32_save.png"))
		save = gtk.ToolButton(image)
		tooltips.set_tip(save, _("Save image"))
		save.connect("clicked", self.callback_save)
		toolbar.insert(save, -1)
	

		plot_toolbar = NavigationToolbar(self.fig.canvas, self)
		plot_toolbar.show()
		box=gtk.HBox(True, 1)
		box.set_size_request(300,-1)
		box.show()
		box.pack_start(plot_toolbar, True, True, 0)
		tb_comboitem = gtk.ToolItem();
		tb_comboitem.add(box);
		tb_comboitem.show()
		toolbar.insert(tb_comboitem, -1)

		sep = gtk.SeparatorToolItem()
		sep.set_draw(False)
		sep.set_expand(True)
		toolbar.insert(sep, -1)
		sep.show()


		toolbar.show_all()
		self.pack_start(toolbar, False, True, 0)
		self.pack_start(toolbar, True, True, 0)



		canvas.set_size_request(700,400)
		self.pack_start(canvas, True, True, 0)



		self.statusbar = gtk.Statusbar()
		self.statusbar.show()
		self.pack_start(self.statusbar, False, False, 0)

		self.draw_graph()

		self.show()

