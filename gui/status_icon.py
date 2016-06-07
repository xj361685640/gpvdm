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
import sys
import os
from inp import inp_get_token_value
from cal_path import get_image_file_path

from win_lin import running_on_linux
import subprocess
from util import gui_print_path

from cal_path import get_exe_command
from help import my_help_class
from sim_warnings import sim_warnings
from inp_util import inp_search_token_value

import i18n
_ = i18n.language.gettext
from cluster import cluster

statusicon = gtk.StatusIcon()

def status_icon_init():
	global statusicon
	statusicon.set_from_file(os.path.join(get_image_file_path(),"ball_green.png"))
	#self.statusicon.set_from_stock(gtk.STOCK_YES)
	#self.statusicon.connect("popup-menu", self.right_click_event)
	statusicon.set_tooltip("gpvdm")

def status_icon_run(cluster):
	global statusicon
	if cluster==False:
		statusicon.set_from_file(os.path.join(get_image_file_path(),"ball_red.png"))
	else:
		statusicon.set_from_file(os.path.join(get_image_file_path(),"ball_red4.png"))	

def status_icon_stop(cluster):
	global statusicon
	if cluster==False:
		statusicon.set_from_file(os.path.join(get_image_file_path(),"ball_green.png"))
	else:
		statusicon.set_from_file(os.path.join(get_image_file_path(),"ball_green4.png"))
def status_icon_get():
	global statusicon
	return statusicon
