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
#import os
#import shutil
from tab_base import tab_base
#import commands
import webkit

class information(gtk.HBox,tab_base):

	def on_active(self, widge, data=None):
		url = self.url_bar.get_text()
		try:
		    url.index("://")
		except:
		    url = "http://"+url
		self.url_bar.set_text(url)
		self.web.open(url)

	def go_back(self, widget, data=None):
		self.web.go_back()

	def go_forward(self, widget, data=None):
		self.web.go_forward()

	def refresh(self, widget, data=None):
		self.web.reload()

	def update_buttons(self, widget, data=None):
		self.url_bar.set_text( widget.get_main_frame().get_uri() )
		self.back_button.set_sensitive(self.web.can_go_back())
		self.forward_button.set_sensitive(self.web.can_go_forward())


	def init(self):
		self.web = webkit.WebView()

		sw = gtk.ScrolledWindow()
		sw.add(self.web)


		toolbar = gtk.Toolbar()

		self.back_button = gtk.ToolButton(gtk.STOCK_GO_BACK)
		self.back_button.connect("clicked", self.go_back)

		self.forward_button = gtk.ToolButton(gtk.STOCK_GO_FORWARD)
		self.forward_button.connect("clicked", self.go_forward)

		refresh_button = gtk.ToolButton(gtk.STOCK_REFRESH)
		refresh_button.connect("clicked", self.refresh)

		toolbar.add(self.back_button)
		toolbar.add(self.forward_button)
		toolbar.add(refresh_button)

		self.url_bar = gtk.Entry()
		self.url_bar.connect("activate", self.on_active)

		box=gtk.HBox(True, 1)
		box.set_size_request(500,-1)
		box.show()
		box.pack_start(self.url_bar, True, True, 0)
		url_tb_item = gtk.ToolItem();
		url_tb_item.add(box);
		url_tb_item.show()
		toolbar.add(url_tb_item)

		self.web.connect("load_committed", self.update_buttons)


		vbox = gtk.VBox(False, 0)
		vbox.pack_start(toolbar, False, True, 0)
		vbox.pack_start(self.url_bar, False, True, 0)
		vbox.add(sw)

		self.show_all()


		self.web.open("http://www.gpvdm.com/welcome.html")
		self.add(vbox)


		self.show_all()



