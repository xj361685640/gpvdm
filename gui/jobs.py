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
#import commands
from cal_path import get_image_file_path
from search import find_fit_log
from search import find_fit_speed_log
from window_list import windows
from inp import inp_load_file
from inp_util import inp_search_token_value
from status_icon import status_icon_stop


actresses = [("n","name","done","status","target","ip","copystate","start","stop")]


class jobs_view(gtk.VBox):

	def init(self,jobs):

		self.set_size_request(400,800)

		self.sw = gtk.ScrolledWindow()
		self.sw.set_shadow_type(gtk.SHADOW_ETCHED_IN)
		self.sw.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)

		self.pack_start(self.sw, True, True, 0)

		treeView = gtk.TreeView(jobs)

		#treeView.connect("row-activated", self.on_activated)
		treeView.set_rules_hint(True)
		self.sw.add(treeView)
		self.sw.show()

		self.create_columns(treeView)
		self.statusbar = gtk.Statusbar()
		self.pack_start(self.sw, False, False, 0)        
		self.pack_start(self.statusbar, False, False, 0)

		self.show_all()


	def add_items(self):
		self.store.clear()
		for act in actresses:
			self.store.store.append([act[0], act[1], act[2], act[3],act[4], act[5], act[6], act[7]])


	def create_columns(self, treeView):

		rendererText = gtk.CellRendererText()
		column = gtk.TreeViewColumn("n", rendererText, text=0)
		column.set_sort_column_id(0)    
		treeView.append_column(column)

		rendererText = gtk.CellRendererText()
		column = gtk.TreeViewColumn("done", rendererText, text=1)
		column.set_sort_column_id(1)
		treeView.append_column(column)

		rendererText = gtk.CellRendererText()
		column = gtk.TreeViewColumn("status", rendererText, text=2)
		column.set_sort_column_id(2)
		treeView.append_column(column)

		rendererText = gtk.CellRendererText()
		column = gtk.TreeViewColumn("target", rendererText, text=3)
		column.set_sort_column_id(3)
		treeView.append_column(column)

		rendererText = gtk.CellRendererText()
		column = gtk.TreeViewColumn("ip", rendererText, text=4)
		column.set_sort_column_id(4)
		treeView.append_column(column)

		rendererText = gtk.CellRendererText()
		column = gtk.TreeViewColumn("copystate", rendererText, text=5)
		column.set_sort_column_id(5)
		treeView.append_column(column)

		rendererText = gtk.CellRendererText()
		column = gtk.TreeViewColumn("start", rendererText, text=6)
		column.set_sort_column_id(6)
		treeView.append_column(column)

		rendererText = gtk.CellRendererText()
		column = gtk.TreeViewColumn("stop", rendererText, text=7)
		column.set_sort_column_id(7)
		treeView.append_column(column)

	def on_activated(self, widget, row, col):

		model = widget.get_model()
		text = model[row][0] + ", " + model[row][1] + ", " + model[row][2]
		self.statusbar.push(0, text)



