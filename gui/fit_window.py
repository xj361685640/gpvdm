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
import shutil
from about import about_dialog_show
from gui_util import dlg_get_text
from window_list import windows
import webbrowser
from code_ctrl import enable_betafeatures
from inp import inp_update_token_value
from fit_tab import fit_tab
from util_zip import zip_lsdir
from inp import inp_isfile
from inp import inp_copy_file
from inp import inp_remove_file
from util import strextract_interger
from global_objects import global_object_get
from cal_path import get_image_file_path
from global_objects import global_object_register
from server import server_get

import i18n
_ = i18n.language.gettext

def fit_new_filename():
	for i in range(0,20):
		pulse_name="fit"+str(i)+".inp"
		if inp_isfile(pulse_name)==False:
			return i
	return -1

class fit_window(gtk.Window):

	def update(self):
		for item in self.notebook.get_children():
			item.update()

	def get_main_menu(self, window):
		accel_group = gtk.AccelGroup()
		item_factory = gtk.ItemFactory(gtk.MenuBar, "<main>", accel_group)

		item_factory.create_items(self.menu_items)
		if enable_betafeatures()==False:
			item_factory.delete_item(_("/Advanced"))

		window.add_accel_group(accel_group)

		self.item_factory = item_factory

		return item_factory.get_widget("<main>")

	def callback_close(self, widget, data=None):
		self.win_list.update(self,"fit_window")
		self.hide()
		return True

	def callback_help(self, widget, data=None):
		webbrowser.open('http://www.gpvdm.com/man/index.html')

	def callback_add_page(self, widget, data=None):
		new_sim_name=dlg_get_text( _("New fit name:"), _("fit ")+str(len(self.notebook.get_children())+1),image_name="new.png")

		if new_sim_name!=None:
			index=fit_new_filename()
			shutil.copy("fit0.inp","fit"+str(index)+".inp")
			shutil.copy("fit_data0.inp","fit_data"+str(index)+".inp")
			shutil.copy("fit_patch0.inp","fit_patch"+str(index)+".inp")
			inp_update_token_value("fit"+str(index)+".inp", "#fit_name", new_sim_name,1)
			self.add_page(index)

	def callback_remove_page(self,widget,name):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		self.toggle_tab_visible(tab.tab_name)

	def callback_copy_page(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		old_index=tab.index
		new_sim_name=dlg_get_text( _("Clone the current fit to a new fit called:"), tab.tab_name,image_name="clone.png")
		if new_sim_name!=None:
			new_sim_name=new_sim_name
			index=fit_new_filename()

			shutil.copy("fit"+str(old_index)+".inp","fit"+str(index)+".inp")
			shutil.copy("fit_data"+str(old_index)+".inp","fit_data"+str(index)+".inp")
			shutil.copy("fit_patch"+str(old_index)+".inp","fit_patch"+str(index)+".inp")

			inp_update_token_value("fit"+str(index)+".inp", "#fit_name", new_sim_name,1)
			self.add_page(index)


	def remove_invalid(self,input_name):
		return input_name.replace (" ", "_")

	def callback_import(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		tab.import_data()
			
	def callback_rename_page(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		new_sim_name=dlg_get_text( _("Rename the fit to be called:"), tab.tab_name,image_name="rename.png")

		if new_sim_name!=None:
			#new_sim_name=self.remove_invalid(new_sim_name)
			#new_dir=os.path.join(self.sim_dir,new_sim_name)
			#shutil.move(old_dir, new_dir)
			tab.rename(new_sim_name)
			#edit


	def callback_delete_page(self,widget,data):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		md = gtk.MessageDialog(None, 0, gtk.MESSAGE_QUESTION,  gtk.BUTTONS_YES_NO, _("Should I remove the fit file ")+tab.tab_name.split("@")[0])

		response = md.run()

		if response == gtk.RESPONSE_YES:
			inp_remove_file("fit"+str(tab.index)+".inp")
			inp_remove_file("fit_data"+str(tab.index)+".inp")
			inp_remove_file("fit_patch"+str(tab.index)+".inp")
			self.notebook.remove_page(pageNum)
			global_object_get("tb_item_sim_mode_update")()

		elif response == gtk.RESPONSE_NO:
			print _("Not deleting")
			#edit


		md.destroy()

	#def callback_view_toggle(self, widget, data):
		#self.toggle_tab_visible(widget.get_label())

	def callback_view_toggle_tab(self, widget, data):
		print "add code"
		#self.toggle_tab_visible(data)

	def load_tabs(self):

		file_list=zip_lsdir(os.path.join(os.getcwd(),"sim.gpvdm"))
		files=[]
		for i in range(0,len(file_list)):
			if file_list[i].startswith("fit") and file_list[i].endswith(".inp"):
				num=file_list[i][3:-4]
				if num.isdigit()==True:
					files.append(file_list[i])

		print "load tabs",files

		for i in range(0,len(files)):
			value=strextract_interger(files[i])
			if value!=-1:
				self.add_page(value)

	def clear_pages(self):
		for items in self.tab_menu.get_children():
			self.tab_menu.remove(items)

		for child in self.notebook.get_children():
    			self.notebook.remove(child)


	def add_page(self,index):
		new_tab=fit_tab()
		new_tab.init(index)
		new_tab.close_button.connect("clicked", self.callback_view_toggle_tab,new_tab.tab_name)

		self.notebook.append_page(new_tab,new_tab.title_hbox)
		self.notebook.set_tab_reorderable(new_tab,True)

	def switch_page(self,page, page_num, user_param1):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		self.status_bar.push(self.context_id, tab.tab_name)

	def rod(self):
		pageNum = self.notebook.get_current_page()
		tab = self.notebook.get_nth_page(pageNum)
		tab.update()

	def callback_one_fit(self, widget, data=None):
		my_server=server_get()
		my_server.clear_cache()
		my_server.add_job(os.getcwd(),"--1fit")
		my_server.set_callback_when_done(self.rod)
		my_server.start()

	def callback_do_fit(self, widget, data=None):
		my_server=server_get()
		my_server.clear_cache()
		my_server.add_job(os.getcwd(),"--fit")
		my_server.start()


	def init(self):
		self.win_list=windows()
		self.win_list.load()
		self.win_list.set_window(self,"fit_window")
		global_object_register("fit_graph_update",self.update)
		print "constructur"

		self.tooltips = gtk.Tooltips()

		self.set_border_width(2)
		self.set_title(_("Fit window - gpvdm"))

		self.status_bar = gtk.Statusbar()
		self.status_bar.show()
		self.context_id = self.status_bar.get_context_id("Statusbar example")

		box=gtk.HBox()
		box.add(self.status_bar)
		box.set_child_packing(self.status_bar, True, True, 0, 0)
		box.show()


		self.menu_items = (
		    ( _("/_File"),         None,         None, 0, "<Branch>" ),
		    ( _("/File/Close"),     None, self.callback_close, 0, None ),
		    ( _("/fits/_New"),     None, self.callback_add_page, 0, "<StockItem>", "gtk-new" ),
		    ( _("/fits/_Delete fit"),     None, self.callback_delete_page, 0, "<StockItem>", "gtk-delete" ),
		    ( _("/fits/_Rename fit"),     None, self.callback_rename_page, 0, "<StockItem>", "gtk-edit" ),
		    ( _("/fits/_Import"),     None, self.callback_import, 0, "<StockItem>", "gtk-edit" ),
		    ( _("/fits/_Clone fit"),     None, self.callback_copy_page, 0, "<StockItem>", "gtk-copy" ),
		    ( _("/_Help"),         None,         None, 0, "<LastBranch>" ),
		    ( _("/_Help/Help"),   None,         self.callback_help, 0, None ),
		    ( _("/_Help/About"),   None,         about_dialog_show, 0, "<StockItem>", "gtk-about" ),
		    )


		main_vbox = gtk.VBox(False, 3)

		menubar = self.get_main_menu(self)
		main_vbox.pack_start(menubar, False, False, 0)
		menubar.show()

		toolbar = gtk.Toolbar()
		toolbar.set_style(gtk.TOOLBAR_ICONS)
		toolbar.set_size_request(-1, 70)

		image = gtk.Image()
		image.set_from_file(os.path.join(get_image_file_path(),"new.png"))
		tb_new_scan = gtk.ToolButton(image)
		tb_new_scan.connect("clicked", self.callback_add_page)
		self.tooltips.set_tip(tb_new_scan, _("Add new fit."))

		toolbar.insert(tb_new_scan, -1)

		image = gtk.Image()
		image.set_from_file(os.path.join(get_image_file_path(),"delete.png"))
		delete = gtk.ToolButton(image)
		delete.connect("clicked", self.callback_delete_page,None)
		self.tooltips.set_tip(delete, _("Delete fit"))
		toolbar.insert(delete, -1)

		image = gtk.Image()
		image.set_from_file(os.path.join(get_image_file_path(),"clone.png"))
		copy = gtk.ToolButton(image)
		copy.connect("clicked", self.callback_copy_page,None)
		self.tooltips.set_tip(copy, _("Clone fit"))
		toolbar.insert(copy, -1)


		image = gtk.Image()
		image.set_from_file(os.path.join(get_image_file_path(),"rename.png"))
		rename = gtk.ToolButton(image)
		rename.connect("clicked", self.callback_rename_page,None)
		self.tooltips.set_tip(rename, _("Rename fit"))
		toolbar.insert(rename, -1)

		image = gtk.Image()
		image.set_from_file(os.path.join(get_image_file_path(),"import.png"))
		import_data = gtk.ToolButton(image)
		import_data.connect("clicked", self.callback_import,None)
		self.tooltips.set_tip(import_data, _("Import data"))
		toolbar.insert(import_data, -1)


		sep = gtk.SeparatorToolItem()
		sep.set_draw(True)
		sep.set_expand(False)
		toolbar.insert(sep, -1)

		image = gtk.Image()
		image.set_from_file(os.path.join(get_image_file_path(),"play.png"))
		self.play = gtk.ToolButton(image)
		self.tooltips.set_tip(self.play, _("Run a single fit"))
		toolbar.insert(self.play, -1)
		self.play.connect("clicked", self.callback_one_fit)

		image = gtk.Image()
		image.set_from_file(os.path.join(get_image_file_path(),"forward.png"))
		self.do_fit = gtk.ToolButton(image)
		self.tooltips.set_tip(self.do_fit, _("Start the fitting process"))
		toolbar.insert(self.do_fit, -1)
		self.do_fit.connect("clicked", self.callback_do_fit)

		sep = gtk.SeparatorToolItem()
		sep.set_draw(False)
		sep.set_expand(True)
		toolbar.insert(sep, -1)

		image = gtk.Image()
		image.set_from_file(os.path.join(get_image_file_path(),"help.png"))
		tb_help = gtk.ToolButton(image)
		tb_help.connect("clicked", self.callback_help)
		self.tooltips.set_tip(tb_help, "Help")
		toolbar.insert(tb_help, -1)

		toolbar.show_all()
		main_vbox.pack_start(toolbar, False, False, 0)

		main_vbox.set_border_width(1)
		self.add(main_vbox)
		main_vbox.show()


		self.notebook = gtk.Notebook()
		self.notebook.show()
		self.notebook.set_tab_pos(gtk.POS_LEFT)

		self.load_tabs()
		main_vbox.pack_start(self.notebook, True, True, 0)
		main_vbox.pack_start(box, False, False, 0)

		self.connect("delete-event", self.callback_close)
		self.notebook.connect("switch-page",self.switch_page)
		self.set_icon_from_file(os.path.join(get_image_file_path(),"fit.png"))

		self.hide()

