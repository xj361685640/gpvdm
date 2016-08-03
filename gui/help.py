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

import os
from cal_path import get_image_file_path
import webbrowser

from PyQt5.QtCore import QSize, Qt
from PyQt5.QtWidgets import QWidget, QVBoxLayout,QProgressBar,QLabel,QDesktopWidget

class help_data():
	def __init__(self,token,icon,text):
		self.token=token
		self.icon=icon
		self.text=text

class help_class(QWidget):
	def move_window(self):
		shape=QDesktopWidget().screenGeometry()

		w=shape.width()
		h=shape.height()
		win_w=self.frameGeometry().width()
		win_h=self.frameGeometry().height()

		x=w-win_w
		y=0
		self.move(x,y)

	def help_show(self):
		self.show()
		self.move_window()

	def toggle_visible(self):
		if self.isVisible()==True:
			#self.hide()
			self.setVisible(self, False)
		else:
#			self.show()
			self.setVisible(self, True)

		self.move_window()

	def __init__(self):
		QWidget.__init__(self)
		self.last=[]
		self.pos=-1

		self.move_window()
		self.vbox=gtk.VBox()
		self.vbox.show()
		self.add(self.vbox)

		self.box=[]
		self.image=[]
		self.label=[]
		for i in range(0,5):
			self.box.append(gtk.HBox())
			self.image.append(gtk.Image())
			self.label.append(gtk.Label())

			self.label[i].set_line_wrap(True)
			self.label[i].set_justify(gtk.JUSTIFY_LEFT)
			self.label[i].set_width_chars(30)
			self.label[i].set_alignment(0.1, 0)

			self.box[i].pack_start(self.image[i], False, False, 0)
			self.box[i].pack_start(self.label[i], True, True, 0)


		toolbar = gtk.Toolbar()
		toolbar.set_style(gtk.TOOLBAR_ICONS)
		toolbar.set_size_request(100, 50)
		toolbar.show()

		pos=0
		image = gtk.Image()
		#print get_image_file_path()
   		image.set_from_file(os.path.join(get_image_file_path(),"qe.png"))

		self.back = gtk.ToolButton(gtk.STOCK_GO_BACK)
		toolbar.insert(self.back, pos)
		self.back.show_all()
		self.back.set_sensitive(False)
		self.back.connect("clicked", self.callback_back)
		pos=pos+1

		self.forward = gtk.ToolButton(gtk.STOCK_GO_FORWARD)
		toolbar.insert(self.forward, pos)
		self.forward.set_sensitive(False)
		self.forward.show_all()
		self.forward.connect("clicked", self.callback_forward)
		pos=pos+1

		sep = gtk.SeparatorToolItem()
		sep.set_draw(False)
		sep.set_expand(True)
		sep.show()
		toolbar.insert(sep, pos)
		pos=pos+1

		image = gtk.Image()
   		image.set_from_file(os.path.join(get_image_file_path(),"www.png"))
		self.play = gtk.ToolButton(image)

		help_book = gtk.ToolButton(image)
		toolbar.insert(help_book, pos)
		help_book.connect("clicked", self.on_line_help)
		help_book.show_all()
		pos=pos+1

		close = gtk.ToolButton(gtk.STOCK_CLOSE)
		toolbar.insert(close, pos)
		close.connect("clicked", self.callback_close)
		close.show_all()
		pos=pos+1

		self.status_bar = gtk.Statusbar()
		self.context_id = self.status_bar.get_context_id("Statusbar example")
		self.status_bar.show()
		#self.tooltips.set_tip(self.qe_button, "Quantum efficiency")


		self.vbox.pack_start(toolbar, False, False, 0)

		for i in range(0,5):
			self.vbox.pack_start(self.box[i], True, True, 0)

		self.vbox.pack_start(self.status_bar, False, False, 0)




		self.set_border_width(10)
		self.set_title("Help")
		self.resize(300,100)
		self.set_decorated(False)
		self.set_border_width(0)

		self.show()


	def callback_close(self,widget):
		self.toggle_visible()

	def callback_forward(self,widget):
		self.pos=self.pos+1
		if self.pos>=len(self.last):
			self.pos=len(self.last)-1

		self.update()

	def callback_back(self,widget):
		self.pos=self.pos-1
		if self.pos<0:
			self.pos=0
		#print self.pos,self.last_icons
		self.update()

	def on_line_help(self,widget):
		webbrowser.open('http://www.gpvdm.com/man/index.html')

	def update(self):
		for i in range(0,5):
			self.box[i].hide_all()

		for i in range(0,len(self.last[self.pos])/2):
			self.image[i].set_from_file(os.path.join(get_image_file_path(),self.last[self.pos][i*2]))
			self.label[i].set_markup(self.last[self.pos][i*2+1]+"\n")
			self.box[i].show_all()
			self.image[i].show()

		self.forward.set_sensitive(True)
		self.back.set_sensitive(True)

		if self.pos==0:
			self.back.set_sensitive(False)

		if self.pos==len(self.last)-1:
			self.forward.set_sensitive(False)

		self.status_bar.push(self.context_id, str(self.pos)+"/"+str(len(self.last)-1))

	def help_set_help(self,array):
		add=True
		if len(self.last)!=0:
			if self.last[self.pos][1]==array[1]:
				add=False

		if add==True:
			self.pos=self.pos+1
			self.last.append(array)
		self.update()
		self.resize(300, 150)
		self.move_window()

	def help_append(self,array):
		self.last[self.pos-1]=self.last[self.pos-1] + array
		self.update()
		self.resize(300, 150)
		self.move_window()

my_help_class=help_class()
my_help_class.init()


