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
from PyQt5.QtWidgets import QWidget, QVBoxLayout,QHBoxLayout,QLabel,QDesktopWidget
from gpvdm_progress import gpvdm_progress
from spinner import spinner

#from help import my_help_class

class progress_class(QWidget):

	def __init__(self):
		QWidget.__init__(self)
		self.setWindowFlags(Qt.FramelessWindowHint|Qt.WindowStaysOnTopHint)
		self.setFixedSize(400, 70)

		main_vbox = QVBoxLayout()
		hbox= QHBoxLayout()
		hbox.setContentsMargins(0, 0, 0, 0)
		self.progress = gpvdm_progress()
		self.spinner=spinner()
		hbox.addWidget(self.progress, 0)
		hbox.addWidget(self.spinner, 0)
		w=QWidget()
		w.setLayout(hbox)
		main_vbox.addWidget(w,0)

		self.label=QLabel()
		self.label.setText(_("Running")+"...")

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

