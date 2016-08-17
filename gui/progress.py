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



from PyQt5.QtCore import QSize, Qt
from PyQt5.QtWidgets import QWidget, QVBoxLayout,QLabel,QDesktopWidget
from gpvdm_progress import gpvdm_progress

#from help import my_help_class

class progress_class(QWidget):

	def __init__(self):
		QWidget.__init__(self)
		self.setWindowFlags(Qt.FramelessWindowHint|Qt.WindowStaysOnTopHint)
		self.setFixedSize(400, 70)

		main_vbox = QVBoxLayout()

		self.progress = gpvdm_progress()
		main_vbox.addWidget(self.progress)

		#self.spin=gtk.Spinner()
		#self.spin.set_size_request(32, 32)
		#self.spin.show()
		#self.spin.stop()

		#main_hbox.pack_start(self.spin, False, False, 0)


		#main_vbox = gtk.VBox(False, 5)
		#main_vbox.show()
		#main_vbox.pack_start(main_hbox, True, True, 0)

		#self.progress_array = []
		#for i in range(0,10):
		#	self.progress_array.append(gtk.ProgressBar(adjustment=None))
		#	self.progress_array[i].hide()
		#	self.progress_array[i].set_size_request(-1, 15)
		#	self.progress_array[i].modify_bg(gtk.STATE_PRELIGHT, gtk.gdk.color_parse("green"))
		#	main_vbox.pack_end(self.progress_array[i], True, False, 0)

		self.label=QLabel()
		self.label.setText("Running...")

		main_vbox.addWidget(self.label)

		self.setLayout(main_vbox)


	def set_fraction(self,fraction):
		self.progress.setValue(fraction)

	def pulse(self):
		self.progress.pulse()
		
	def start(self):
		shape=QDesktopWidget().screenGeometry()

		w=shape.width()
		h=shape.height()
		win_w=self.frameGeometry().width()
		win_h=self.frameGeometry().height()

		x=w-win_w
		y=0
		self.move(x,y)
		self.show()

	def stop(self):
		self.hide()
		#self.spin.stop()
		#my_help_class.help_show()

	def set_text(self,text):
		text=text
		l=len(text)
		if l>50:
			l=l-50
			text=text[l:]

		self.label.setText(text)

