#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012 Roderick C. I. MacKenzie
#
#	roderick.mackenzie@nottingham.ac.uk
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




import gtk
#import os
#from global_objects import global_object_get
#from plot_io import get_plot_file_info
#from plot_state import plot_state
#from util import latex_to_pygtk_subscript
#from help import my_help_class
#from cal_path import get_image_file_path

COL_PATH = 0
COL_PIXBUF = 1
COL_IS_DIRECTORY = 2

import i18n
_ = i18n.language.gettext

class sim_warnings(gtk.Dialog): 

	def callback_close(self, widget, data=None):
		self.response(True)
	    	return

	def init(self,text):
		self.set_default_response(gtk.RESPONSE_OK)
		self.set_title(_("Simulation report - gpvdm"))
		self.set_flags(gtk.DIALOG_DESTROY_WITH_PARENT)
		#self.add_buttons("OK",True,"Cancel",False)

		#self.set_size_request(800, 400)
		self.set_position(gtk.WIN_POS_CENTER)


		hbox = gtk.HBox(False, 0);
		hbox.show()


		self.text = gtk.TextView()
		self.buf=gtk.TextBuffer()
		self.buf.set_text(text)
		self.text.set_buffer(self.buf)
		self.text.show()
		self.text.set_size_request(700, 400)


		sw = gtk.ScrolledWindow()
		sw.set_shadow_type(gtk.SHADOW_ETCHED_IN)
		sw.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
		#vbox.pack_start(self.text, True, True, 0)


		button = gtk.Button("Close")
		button.show()
		#self.text.set_size_request(200, 80)
		button.connect("clicked", self.callback_close, "Close")
		hbox.pack_end( button,False,False,10)
		sw.add(self.text)

		self.vbox.pack_start( sw,True,True)
		self.vbox.pack_start(hbox)

		#self.vbox.add(sw)
		self.show_all()


