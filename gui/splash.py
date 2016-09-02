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
from ver import version
from notice import notice
import random
import time

from PyQt5.QtGui import QIcon,QTransform
from PyQt5.QtCore import QSize, Qt, QTimer
from PyQt5.uic import loadUi
from PyQt5.QtWidgets import QApplication,QGraphicsScene
from PyQt5.QtGui import QPixmap

#cal_path
from cal_path import get_image_file_path
from cal_path import get_ui_path

class splash_window():

	def center(self):
		frameGm = self.window.frameGeometry()
		screen = QApplication.desktop().screenNumber(QApplication.desktop().cursor().pos())
		centerPoint = QApplication.desktop().screenGeometry(screen).center()
		frameGm.moveCenter(centerPoint)
		self.window.move(frameGm.topLeft())

	def callback_destroy(self):
		self.window.close()


	def init(self):
		self.window = loadUi(os.path.join(get_ui_path(),"splash.ui"))
		self.window.setWindowFlags(Qt.FramelessWindowHint|Qt.WindowStaysOnTopHint)
		self.center()
		self.window.li.setText(notice()+"\n"+version())
		self.window.setModal(Qt.WindowModal)
		self.window.image.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
		self.window.image.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff)

		window_h=self.window.height()
		window_w=self.window.width()

		image=QPixmap(os.path.join(get_image_file_path(),"splash3.png"))
		image.scaledToHeight(window_h)

		w=image.width()
		h=image.height()
		x_max=w-window_h-window_w/2

		hour=float(time.strftime("%H"))*60
		m=float(time.strftime("%m"))
		tot=hour+m
		my_max=float(24*60)

		value=tot/my_max

		xpos=int(x_max*value)+window_w/2
		print("xpos=",xpos)
		scene=QGraphicsScene();
		scene.setSceneRect(xpos, 0, 0, h)
		self.window.image.setScene(scene)

		self.window.show()

		scene.addPixmap(image);

		QTimer.singleShot(1500, self.callback_destroy)

