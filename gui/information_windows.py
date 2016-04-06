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

import pygtk
pygtk.require('2.0')
import gtk
#import sys
import os
#import shutil
#import commands
#import subprocess
#import time
#import re
#import os
#from ver import ver_core
#from ver import ver_mat
#from ver import ver_gui
from tab_base import tab_base
from help import my_help_class
from cal_path import get_image_file_path

import i18n
_ = i18n.language.gettext

class information(gtk.HBox,tab_base):
	
	lines=[]
	edit_list=[]
	file_name=""
	line_number=[]
	save_file_name=""
	name="Welcome"

	def init(self):
		self.label = gtk.Label()

		self.text=_("<big><b>General-purpose photovoltaic device model</b>\n(<a href=\"http://www.gpvdm.com\" title=\"Click to find out more\">www.gpvdm.com</a>)\n\n To make a new simulation directory click <i>new</i> in the <i>file</i> menu\n or to open an existing simulation click on the <i>open</i> button.\n There is more help on the <a href=\"http://www.gpvdm.com/man/index.html\">man pages</a>.  Please report bugs to\nroderick.mackenzie@nottingham.ac.uk.\n\n Rod\n18/10/13\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n\n")
		#read_page=False


		self.label.set_markup(self.text+"</big>")
		self.label.set_alignment(0, 0.5)
		self.pack_start(self.label, True, True, 0)
		image = gtk.Image()
   		image.set_from_file(os.path.join(get_image_file_path(),"cell.jpg"))
		self.pack_start(image, False, False, 0)

   		image.show()
		self.label.show()


	def update(self,data):
		self.text=self.web.text
		self.label.set_markup(self.text+"</big>")
		#self.hide_all()

	def help(self):
		my_help_class.help_set_help(["icon.png",_("<big><b>Welcome to gpvdm</b></big>\n The window will provide you with information about new versions and bugs in gpvdm.")])


