#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
#
#	https://www.gpvdm.com
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

from about import about_dlg

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QSystemTrayIcon,QMenu,QApplication
from PyQt5.QtGui import QIcon

from cluster import cluster

import webbrowser

statusicon = None

class tray_icon(QSystemTrayIcon):

	def __init__(self,  parent=None):
		QSystemTrayIcon.__init__(self, QIcon(os.path.join(get_image_file_path(),"ball_green.png")), parent)
		menu = QMenu(parent)
		self.menu_about = menu.addAction(_("About"))
		self.menu_about.triggered.connect(self.callback_about)
		self.menu_man = menu.addAction(_("Manual"))
		self.menu_man.triggered.connect(self.callback_man)

		self.menu_youtube = menu.addAction("&"+_("Youtube channel"))
		self.menu_youtube.triggered.connect(self.callback_youtube)

		self.exitAction = menu.addSeparator()		
		self.exitAction = menu.addAction(_("Quit"))
		
		self.exitAction.triggered.connect(self.callback_exit)
		self.setContextMenu(menu)

	def callback_exit(self):
		QApplication.quit()

	def callback_about(self):
		dlg=about_dlg()
		dlg.ui.exec_()

	def callback_man(self):
		webbrowser.open("https://www.gpvdm.com/man.html")
	def	callback_youtube(self):
		webbrowser.open("https://www.youtube.com/channel/UCbm_0AKX1SpbMMT7jilxFfA")


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
