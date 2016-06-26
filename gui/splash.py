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
#import sys
import os
#import shutil
import glib
#from token_lib import tokens
#from numpy import *
#from util import pango_to_gnuplot
from cal_path import get_image_file_path
from ver import version
from notice import notice
import random
import time

class splash_window():
	def timer_cb(self):
		self.window.destroy()
		return False

	def destroy(self):
		self.window.destroy()

	def callback_destroy(self,widget):
		self.destroy()

	def show_cb(self,widget, data=None):
		glib.timeout_add(1500, self.timer_cb)

	def init(self):
		self.window = gtk.Window()
		self.window.connect("show", self.show_cb)

		self.window.set_decorated(False)
		self.window.set_border_width(0)
		self.window.set_keep_above(True)
		fixed = gtk.Fixed()
		#image = gtk.Image()
		image_file=os.path.join(get_image_file_path(),"splash2.png")

		# Create an Image object for a PNG file.
		#file_name = "/home/rod/test/jn15/images/splash2.png"
		pixbuf = gtk.gdk.pixbuf_new_from_file(image_file)
		w=pixbuf.get_width()
		dw=494
		xpos=w-dw

		h=float(time.strftime("%H"))*60
		m=float(time.strftime("%m"))
		tot=h+m
		my_max=float(24*60)
		value=tot/my_max

		xpos=int(xpos*value)

		cropped_buffer=pixbuf.subpixbuf(xpos,0,dw,pixbuf.get_height())
		pixmap, mask = cropped_buffer.render_pixmap_and_mask()
		image = gtk.Image()
		image.set_from_pixmap(pixmap, mask)
		image.show()


		label = gtk.Label()
		label.set_use_markup(gtk.TRUE)
		label.set_markup('<span color="black" size="88000"><b>gpvdm</b></span>')
		label.show()
		#image.set_from_file(image_file)
		fixed.put(image, 0, 0)
		fixed.put(label,40,40)

		label = gtk.Label()
		label.set_use_markup(gtk.TRUE)
		label.set_markup(notice()+"\n"+version())
		label.show()

		fixed.put(label,40,200)

		self.window.add(fixed)
		self.window.set_position(gtk.WIN_POS_CENTER)
		self.window.show_all()
		self.window.connect("destroy", self.callback_destroy)


