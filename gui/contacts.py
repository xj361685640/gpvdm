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
import os
from numpy import *
from inp import inp_load_file
import webbrowser
from inp_util import inp_search_token_value
from cal_path import get_image_file_path
from window_list import windows

from epitaxy import epitaxy_get_layers
from epitaxy import epitaxy_get_dos_file
from epitaxy import epitaxy_get_width
from inp import inp_update
from scan_item import scan_item_add

from contacts_io import segment
from contacts_io import contacts_save
from contacts_io import contacts_get_array
from contacts_io import contacts_clear
from contacts_io import contacts_print

import i18n
_ = i18n.language.gettext

(
CONTACT_START,
CONTACT_WIDTH,
CONTACT_DEPTH,
CONTACT_VOLTAGE
) = range(4)

from contacts_io import contacts_load
from contacts_io import contacts_print

class contacts_window(gtk.Window):

	visible=1

	def update_contact_db(self):
		contacts_clear()
		for item in self.mesh_model:
			contacts_append(float(item[CONTACT_START]),float(item[CONTACT_WIDTH]),float(item[CONTACT_DEPTH]),float(item[CONTACT_VOLTAGE]))	


	def on_add_item_clicked(self, button):
		new_item = [_("start"),_("width"),"depth","voltage"]

		selection = self.treeview.get_selection()
		model, iter = selection.get_selected()

		path = model.get_path(iter)[0]

		iter = model.insert(path)
		model.set (iter,
		    COLUMN_NAME, new_item[COLUMN_NAME],
		    COLUMN_THICKNES, new_item[COLUMN_THICKNES]
		)
		self.save_data()

	def on_remove_item_clicked(self, button):

		selection = self.treeview.get_selection()
		model, iter = selection.get_selected()

		if iter:
			model.remove(iter)

			self.save_data()

	def save_data(self):
		self.update_contact_db()
		contacts_save()


	def on_cell_edited_start(self, cell, path, new_text, model):
		model[path][CONTACT_START] = new_text
		self.save_data()

	def on_cell_edited_width(self, cell, path, new_text, model):
		model[path][CONTACT_WIDTH] = new_text
		self.save_data()


	def on_cell_edited_depth(self, cell, path, new_text, model):
		model[path][CONTACT_DEPTH] = new_text
		self.save_data()

	def on_cell_edited_voltage(self, cell, path, new_text, model):
		model[path][CONTACT_VOLTAGE] = new_text
		self.save_data()

	def callback_help(self, widget, data=None):
		webbrowser.open('http://www.gpvdm.com/man/index.html')


	def create_model(self):
		store = gtk.ListStore(str,str,str,str)

		store.clear()
		contacts_load()
		contacts_print()

		for c in contacts_get_array():
			iter = store.append()

			print ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>",c.start

			store.set (iter,
			  CONTACT_START, str(c.start),
			  CONTACT_WIDTH, str(c.width),
			  CONTACT_DEPTH, str(c.depth),
			  CONTACT_VOLTAGE, str(c.voltage)
			)


		return store

	def create_columns(self, treeview):


		model=treeview.get_model()

		renderer = gtk.CellRendererText()
		renderer.connect("edited", self.on_cell_edited_start, model)
		renderer.set_property('editable', False)
		column = gtk.TreeViewColumn(_("Start"), renderer, text=CONTACT_START)
		column.set_sort_column_id(CONTACT_START)
		treeview.append_column(column)

		renderer = gtk.CellRendererText()
		renderer.connect("edited", self.on_cell_edited_width, model)
		renderer.set_property('editable', False)
		column = gtk.TreeViewColumn(_("Width"), renderer, text=CONTACT_WIDTH)
		column.set_sort_column_id(CONTACT_WIDTH)
		treeview.append_column(column)

		renderer = gtk.CellRendererText()
		renderer.connect("edited", self.on_cell_edited_depth, model)
		column = gtk.TreeViewColumn("Depth", renderer, text=CONTACT_DEPTH)
		renderer.set_property('editable', False)
		column.set_sort_column_id(CONTACT_DEPTH)
		treeview.append_column(column)

		renderer = gtk.CellRendererText()
		renderer.connect("edited", self.on_cell_edited_voltage, model)
		column = gtk.TreeViewColumn("Voltage", renderer, text=CONTACT_VOLTAGE)
		renderer.set_property('editable', False)
		column.set_sort_column_id(CONTACT_VOLTAGE)
		treeview.append_column(column)


	def callback_close(self, widget, data=None):
		self.win_list.update(self,"contact")
		self.hide()
		return True

	def init(self):
		self.win_list=windows()
		self.set_title(_("Electrical contact editor (www.gpvdm.com)"))
		self.set_icon_from_file(os.path.join(get_image_file_path(),"contact.png"))
		self.win_list.set_window(self,"contacts")
		self.set_size_request(600,500)
		self.main_vbox=gtk.VBox()
		self.add(self.main_vbox)

		self.main_vbox.show()
		self.show_key=True
		self.hbox=gtk.HBox()
		self.edit_list=[]
		self.line_number=[]
		gui_pos=0

		self.list=[]

		gui_pos=gui_pos+1

		tooltips = gtk.Tooltips()

		toolbar = gtk.Toolbar()
		#toolbar.set_orientation(gtk.ORIENTATION_VERTICAL)
		toolbar.set_style(gtk.TOOLBAR_ICONS)
		toolbar.set_size_request(-1, 70)

		self.store = self.create_model()
		self.treeview = gtk.TreeView(self.store)
		self.treeview.show()

		image = gtk.Image()
   		image.set_from_file(os.path.join(get_image_file_path(),"add.png"))
		add = gtk.ToolButton(image)
		add.connect("clicked", self.on_add_item_clicked)
		tooltips.set_tip(add, _("Add contact"))
		toolbar.insert(add, -1)


		image = gtk.Image()
   		image.set_from_file(os.path.join(get_image_file_path(),"minus.png"))
		remove = gtk.ToolButton(image)
		remove.connect("clicked", self.on_remove_item_clicked)
		tooltips.set_tip(remove, _("Delete contact"))
		toolbar.insert(remove, -1)

		sep = gtk.SeparatorToolItem()
		sep.set_draw(False)
		sep.set_expand(True)
		toolbar.insert(sep, -1)
		sep.show()

		image = gtk.Image()
		image.set_from_file(os.path.join(get_image_file_path(),"help.png"))
		help = gtk.ToolButton(image)
		toolbar.insert(help, -1)
		help.connect("clicked", self.callback_help)
		help.show()

		toolbar.show_all()
		self.main_vbox.pack_start(toolbar, False, False, 0)

		self.treeview.set_rules_hint(True)

		self.create_columns(self.treeview)

		self.main_vbox.pack_start(self.treeview, True, True, 0)

		self.statusbar = gtk.Statusbar()
		self.statusbar.show()
		self.main_vbox.pack_start(self.statusbar, False, False, 0)

		self.connect("delete-event", self.callback_close)

