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

from cal_path import get_image_file_path
from clone import gpvdm_clone
import os
from cal_path import get_device_lib_path
from import_archive import import_archive
from window_list import windows

import i18n
_ = i18n.language.gettext

class new_simulation(gtk.Dialog):

	# close the window and quit
	def delete_event(self, widget, event, data=None):
		self.win_list.update(self,"new_simulation")
		gtk.main_quit()
		return False


	def callback_close(self, widget, data=None):
		self.win_list.update(self,"new_simulation")
		self.response(False)

	def callback_next(self, widget, data=None):
		selection = self.treeview.get_selection()
		model, iter = selection.get_selected()

		if iter:
			path = model.get_path(iter)[0]
			print path
			print

		dialog = gtk.FileChooserDialog(_("Make new simulation directory"),
                               None,
                               gtk.FILE_CHOOSER_ACTION_OPEN,
                               (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                                gtk.STOCK_NEW, gtk.RESPONSE_OK))

		dialog.set_default_response(gtk.RESPONSE_OK)
		dialog.set_action(gtk.FILE_CHOOSER_ACTION_CREATE_FOLDER)

		filter = gtk.FileFilter()
		filter.set_name(_("All files"))
		filter.add_pattern("*")
		dialog.add_filter(filter)

		response = dialog.run()
		if response == gtk.RESPONSE_OK:
			if not os.path.exists(dialog.get_filename()):
				os.makedirs(dialog.get_filename())

			self.ret_path=dialog.get_filename()
			os.chdir(self.ret_path)
			gpvdm_clone(os.getcwd(),True)
			import_archive(os.path.join(get_device_lib_path(),self.liststore[path][2]),os.path.join(os.getcwd(),"sim.gpvdm"),False)
			self.response(True)
			#self.change_dir_and_refresh_interface(dialog.get_filename())
			print _("OK")

		elif response == gtk.RESPONSE_CANCEL:
			print _("Closed, no dir selected")

		dialog.destroy()

	def get_return_path(self):
		return self.ret_path

	def init(self):
		self.ret_path=""
		# Create a new window

		self.set_title("New simulation - gpvdm.com")

		self.set_size_request(-1, 500)

		self.win_list=windows()
		self.win_list.load()
		self.win_list.set_window(self,"new_simulation")

		#self.connect("delete-event", self.delete_event)

		# create a liststore with one string column to use as the model
		self.liststore = gtk.ListStore(gtk.gdk.Pixbuf, str,str)

		# create the TreeView using liststore

		self.hbox=gtk.HBox()

		self.treeview = gtk.TreeView(self.liststore)

		# create the TreeViewColumns to display the data

		self.tvcolumn1 = gtk.TreeViewColumn('Device type')

		# add a row with text and a stock item - color strings for
		# the background

		self.liststore.append([gtk.gdk.pixbuf_new_from_file(os.path.join(get_image_file_path(),"icon.png")), 'Organic solar cell',"p3htpcbm.gpvdm"])
		self.liststore.append([ gtk.gdk.pixbuf_new_from_file(os.path.join(get_image_file_path(),"oled.png")), "Organic LED","oled.gpvdm"])
		self.liststore.append([ gtk.gdk.pixbuf_new_from_file(os.path.join(get_image_file_path(),"si.png")), 'Crystalline silicon solar cell (new/beta)',"silicon.gpvdm"])
		self.liststore.append([ gtk.gdk.pixbuf_new_from_file(os.path.join(get_image_file_path(),"si.png")), 'CIGS Solar cell (new/beta)',"cigs.gpvdm"])
		self.liststore.append([ gtk.gdk.pixbuf_new_from_file(os.path.join(get_image_file_path(),"asi.png")), 'a-Si solar cell (new/beta)',"a-silicon.gpvdm"])
		self.liststore.append([ gtk.gdk.pixbuf_new_from_file(os.path.join(get_image_file_path(),"psi.png")), 'polycrystalline silicon (new/beta)',"silicon.gpvdm"])


		# add columns to treeview

		# create a CellRenderers to render the data
		cell = gtk.CellRendererPixbuf()
		self.image_column = gtk.TreeViewColumn("Pixbuf", cell)
		self.image_column.add_attribute(cell, "pixbuf", 0)
		self.treeview.append_column(self.image_column)


		self.cell1 = gtk.CellRendererText()



		# add the cells to the columns - 2 in the first
		self.tvcolumn1.pack_start(self.cell1, True)


		#self.tvcolumn.set_attributes(self.cell, text=0)
		self.tvcolumn1.set_attributes(self.cell1, text=1)


		self.treeview.append_column(self.tvcolumn1)

		# make treeview searchable
		self.treeview.set_search_column(0)

		# Allow sorting on the column
		#self.tvcolumn.set_sort_column_id(0)

		# Allow drag and drop reordering of rows
		self.treeview.set_reorderable(True)

		self.label = gtk.Label("<big><b>Which type of device would you like to simulate?</b></big>")
		self.label.set_use_markup(True)

		self.vbox.pack_start(self.label,False,False)
		self.vbox.pack_start(self.treeview,True,True)

		close_button = gtk.Button("Close")
		close_button.show()
		close_button.connect("clicked", self.callback_close, "Close")
		close_button.set_size_request(100, 40)

		ok_button = gtk.Button("Next")
		ok_button.show()
		ok_button.connect("clicked", self.callback_next, "Close")
		ok_button.set_size_request(100, 40)

		self.hbox.pack_end( ok_button,False,False,10)
		self.hbox.pack_end( close_button,False,False,10)

		self.vbox.pack_start(self.hbox,False,False)
		self.add(self.vbox)

		self.show_all()
