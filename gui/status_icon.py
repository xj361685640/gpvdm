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

import sys
import os
from cal_path import get_image_file_path

from win_lin import running_on_linux

import i18n
_ = i18n.language.gettext

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QSystemTrayIcon,QMenu,QApplication
from PyQt5.QtGui import QIcon

from cluster import cluster

statusicon = None

class tray_icon(QSystemTrayIcon):

	def __init__(self,  parent=None):
		QSystemTrayIcon.__init__(self, QIcon(os.path.join(get_image_file_path(),"ball_green.png")), parent)
		menu = QMenu(parent)
		self.exitAction = menu.addAction("Exit")
		self.exitAction.triggered.connect(self.callback_exit)
		self.setContextMenu(menu)

	def callback_exit(self):
		QApplication.quit()

def status_icon_init():
	global statusicon
	statusicon=tray_icon()
	statusicon.show()


def status_icon_run(cluster):
	global statusicon
	if cluster==False:
		statusicon.setIcon(QIcon(os.path.join(get_image_file_path(),"ball_red.png")))
	else:
		statusicon.setIcon(QIcon(os.path.join(get_image_file_path(),"ball_red4.png")))	

def status_icon_stop(cluster):
	global statusicon
	if cluster==False:
		statusicon.setIcon(QIcon(os.path.join(get_image_file_path(),"ball_green.png")))
	else:
		statusicon.setIcon(QIcon(os.path.join(get_image_file_path(),"ball_green4.png")))

def status_icon_get():
	global statusicon
	return statusicon
