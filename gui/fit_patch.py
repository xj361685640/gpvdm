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


import gc
import gtk
import os
from inp import inp_get_token_value
from plot import check_info_file
from used_files_menu import used_files_menu
from plot_dlg import plot_dlg_class
from plot_gen import plot_gen
from plot_state import plot_state

from cmp_class import cmp_class
from scan_select import select_param
from config import config
from token_lib import tokens
from scan_item import scan_items_get_list

from scan_item import scan_item_save
from scan_plot import scan_gen_plot_data
from scan_io import scan_clean_dir
from scan_io import scan_clean_unconverged
from scan_io import scan_clean_simulation_output
from scan_io import scan_nested_simulation
from server import server_find_simulations_to_run
from scan_io import scan_plot_fits

from plot_io import plot_save_oplot_file
from notes import notes
from scan_io import scan_push_to_hpc
from scan_io import scan_import_from_hpc
from cal_path import get_exe_command
from help import my_help_class
from cal_path import get_image_file_path
from scan_item import scan_items_get_file
from scan_item import scan_items_get_token
from util import str2bool

import i18n
_ = i18n.language.gettext

class fit_patch(gtk.VBox):

	icon_theme = gtk.icon_theme_get_default()


	def callback_add_item(self, widget, data=None):
		self.add_line(["File","token",_("Select parameter"), "0.0 0.0", "scan",True])


	def callback_copy_item(self, widget, data=None):
		selection = self.treeview.get_selection()
		model, pathlist = selection.get_selected_rows()
		build=""
		for path in pathlist:
			tree_iter = model.get_iter(path)
			print "path=",tree_iter
			build=build+model.get_value(tree_iter,0)+","+model.get_value(tree_iter,1)+","+model.get_value(tree_iter,2)+","+str(model.get_value(tree_iter,3))+","+str(model.get_value(tree_iter,4))+","+str(str(model.get_value(tree_iter,5)))+"\n"
			print build
		build=build[:-1]
		self.clipboard.set_text(build, -1)


	def callback_show_list(self, widget, data=None):
		self.select_param_window.update()
		self.select_param_window.show()

	def callback_delete_item(self, widget, data=None):
		selection = self.treeview.get_selection()
		model, pathlist = selection.get_selected_rows()

		iters = [model.get_iter(path) for path in pathlist]
		for iter in iters:
			model.remove(iter)

		self.save_combo()

	def save_combo(self):
		a = open(self.file_name, "w")

		for item in self.liststore_combobox:
			a.write(item[1]+"\n")
			a.write(item[0]+"\n")
			a.write(item[2]+"\n")

		a.write("#end\n")

		a.close()


	def combo_changed(self, widget, path, text, model):
		model[path][2] = text
		model[path][0] = scan_items_get_file(text)
		model[path][1] = scan_items_get_token(text)
		self.save_combo()


	def text_changed_file(self, widget, path, text, model):
		model[path][0] = text
		self.save_combo()

	def text_changed_token(self, widget, path, text, model):
		model[path][1] = text
		self.save_combo()

	def text_changed_value(self, widget, path, text, model):
		model[path][2] = text
		self.save_combo()


	def reload_liststore(self):
		self.liststore_combobox.clear()
		self.file_name="fit_patch"+str(self.index)+".inp"

		if os.path.isfile(self.file_name)==True:
			f=open(self.file_name)
			config = f.readlines()
			f.close()

			for ii in range(0, len(config)):
				config[ii]=config[ii].rstrip()

			pos=0
			mylen=len(config)
			while(1):
				t=config[pos]
				if t=="#end":
					break
				pos=pos+1

				f=config[pos]
				if f=="#end":
					break
				pos=pos+1

				v=config[pos]
				if v=="#end":
					break
				pos=pos+1

				self.liststore_combobox.append([f,t,v])

				if pos>mylen:
					break


		
	def callback_close(self,widget):
		self.hide()


	def set_visible(self,value):
		if value==True:
			self.visible=True
			self.config.set_value("#visible",True)
			self.show()
		else:
			self.visible=False
			self.config.set_value("#visible",False)
			self.hide()

	def init(self,index):
		self.index=index
		self.tokens=tokens()
		self.config=config()
		self.clipboard = gtk.clipboard_get(gtk.gdk.SELECTION_CLIPBOARD)

		self.param_list=scan_items_get_list()
		self.liststore_op_type = gtk.ListStore(str)


		toolbar = gtk.Toolbar()
		toolbar.set_style(gtk.TOOLBAR_ICONS)
		toolbar.set_size_request(-1, 50)
		pos=0

		image = gtk.Image()
   		image.set_from_file(os.path.join(get_image_file_path(),"add.png"))
		add = gtk.ToolButton(image)
		add.connect("clicked", self.callback_add_item)
		#self.tooltips.set_tip(add, _("Add parameter to scan"))
		toolbar.insert(add, -1)


		image = gtk.Image()
   		image.set_from_file(os.path.join(get_image_file_path(),"minus.png"))
		remove = gtk.ToolButton(image)
		remove.connect("clicked", self.callback_delete_item)
		#self.tooltips.set_tip(remove, _("Delete item"))
		toolbar.insert(remove, -1)


		sep = gtk.SeparatorToolItem()
		sep.set_draw(True)
		sep.set_expand(False)
		toolbar.insert(sep, -1)

		toolbar.show_all()
		self.pack_start(toolbar, False, False, 0)

		liststore_manufacturers = gtk.ListStore(str)
		for i in range(0,len(self.param_list)):
		    liststore_manufacturers.append([self.param_list[i].name])

		self.liststore_combobox = gtk.ListStore(str, str,str)

		self.reload_liststore()


		self.treeview = gtk.TreeView(self.liststore_combobox)
		self.treeview.get_selection().set_mode(gtk.SELECTION_MULTIPLE)


		self.select_param_window=select_param()
		self.select_param_window.init(self.liststore_combobox,self.treeview)

		column_file = gtk.TreeViewColumn(_("File"))
		column_file.set_visible(True)

		column_token = gtk.TreeViewColumn(_("Token"))
		column_token.set_visible(True)

		column_value = gtk.TreeViewColumn(_("Values"))
		column_token.set_visible(True)

		cellrenderer_file = gtk.CellRendererText()
		cellrenderer_file.set_property("editable", True)
		cellrenderer_file.connect("edited", self.text_changed_file, self.liststore_combobox)
		column_file.pack_start(cellrenderer_file, False)
		column_file.set_min_width(100)
		column_file.add_attribute(cellrenderer_file, "text", 0)

		cellrenderer_token = gtk.CellRendererText()
		cellrenderer_token.set_property("editable", True)
		cellrenderer_token.connect("edited", self.text_changed_token, self.liststore_combobox)
		column_token.pack_start(cellrenderer_token, False)
		column_token.set_min_width(100)
		column_token.add_attribute(cellrenderer_token, "text", 1)


		cellrenderer_value = gtk.CellRendererText()
		cellrenderer_value.set_property("editable", True)
		cellrenderer_value.connect("edited", self.text_changed_value, self.liststore_combobox)
		column_value.pack_start(cellrenderer_value, False)
		column_value.set_min_width(100)
		column_value.add_attribute(cellrenderer_value, "text", 2)


		self.treeview.append_column(column_file)
		self.treeview.append_column(column_token)
		self.treeview.append_column(column_value)

		scrolled_window = gtk.ScrolledWindow()
		scrolled_window.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_ALWAYS)
		scrolled_window.add(self.treeview)
		scrolled_window.set_size_request(1000, 500)

		self.pack_start(scrolled_window, True, True, 0)
		self.treeview.show()
		self.show_all()


