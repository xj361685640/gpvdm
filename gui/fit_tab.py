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
from inp import inp_load_file
from inp_util import inp_search_token_value
from fit_window_plot import fit_window_plot
from fit_window_plot_real import fit_window_plot_real
from inp import inp_update_token_value
from cal_path import get_image_file_path
from tab import tab_class
from fit_patch import fit_patch
import shutil

import i18n
_ = i18n.language.gettext

(
SEG_LENGTH,
SEG_DT,
SEG_VOLTAGE_START,
SEG_VOLTAGE_STOP,
SEG_MUL,
SEG_SUN,
SEG_LASER
) = range(7)

mesh_articles = []

class fit_tab(gtk.VBox):

	def update(self):
		self.tmesh_real.update()
		self.tmesh.update()
	def init(self,index):
		self.tab_label=None


		self.index=index
		lines=[]

		if inp_load_file(lines,"fit"+str(self.index)+".inp")==True:
			self.tab_name=inp_search_token_value(lines, "#fit_name")
		else:
			self.tab_name=""


		self.title_hbox=gtk.HBox()

		self.title_hbox.set_size_request(-1, 25)
		self.label=gtk.Label(self.tab_name)
		self.label.set_justify(gtk.JUSTIFY_LEFT)
		self.title_hbox.pack_start(self.label, False, True, 0)

		self.close_button = gtk.Button()
		close_image = gtk.Image()
   		close_image.set_from_file(os.path.join(get_image_file_path(),"close.png"))
		close_image.show()
		self.close_button.add(close_image)
		self.close_button.props.relief = gtk.RELIEF_NONE

		self.close_button.set_size_request(25, 25)
		self.close_button.show()

		self.title_hbox.pack_end(self.close_button, False, False, 0)
		self.title_hbox.show_all()

		self.notebook=gtk.Notebook()
		self.notebook.show()

		self.tmesh = fit_window_plot()
		self.tmesh.init(self.index)
		self.notebook.append_page(self.tmesh, gtk.Label(_("Fit error")))

		self.tmesh_real = fit_window_plot_real()
		self.tmesh_real.init(self.index)
		self.notebook.append_page(self.tmesh_real, gtk.Label(_("Experimental data")))

		self.fit_patch = fit_patch()
		self.fit_patch.init(self.index)
		self.notebook.append_page(self.fit_patch, gtk.Label(_("Fit patch")))

		self.pack_start(self.notebook, False, False, 0)


		config=tab_class()
		config.show()
		self.notebook.append_page(config,gtk.Label("Configure fit"))
		config.visible=True
		config.init("fit"+str(self.index)+".inp",self.tab_name)
		config.label_name=self.tab_name
		self.show()

	def set_tab_caption(self,name):
		mytext=name
		if len(mytext)<10:
			for i in range(len(mytext),10):
				mytext=mytext+" "
		self.label.set_text(mytext)

	def rename(self,tab_name):
		inp_update_token_value("fit"+str(self.index)+".inp", "#fit_name", self.tab_name,1)
		self.set_tab_caption(self.tab_name)

	def import_data(self):
		dialog = gtk.FileChooserDialog(_("Import data in to gpvdm for fitting."),
                               None,
                               gtk.FILE_CHOOSER_ACTION_OPEN,
                               (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                                gtk.STOCK_OPEN, gtk.RESPONSE_OK))
		dialog.set_default_response(gtk.RESPONSE_OK)

		filter = gtk.FileFilter()
		filter.set_name(".dat")
		filter.add_pattern("*.dat")
		dialog.add_filter(filter)

		filter = gtk.FileFilter()
		filter.set_name(".csv")
		filter.add_pattern("*.csv")
		dialog.add_filter(filter)

		response = dialog.run()
		if response == gtk.RESPONSE_OK:
			print "importing file",dialog.get_filename()
			shutil.copy(dialog.get_filename(), os.path.join(os.getcwd(),"fit_data"+str(self.index)+".inp"))
			self.update()
		elif response == gtk.RESPONSE_CANCEL:
		    print _("Closed, no files selected")
		dialog.destroy()
