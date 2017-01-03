#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2017 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
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



import os
from ver import ver
from notice import notice
from cal_path import get_image_file_path
from cal_path import get_ui_path

import sys

from PyQt5.uic import loadUi
from PyQt5.QtWidgets import QDialog
from PyQt5.QtGui import QPixmap

from PyQt5.QtGui import QIcon

class about_dlg():
	def __init__(self):
		#QDialog.__init__(self)
		self.ui = loadUi(os.path.join(get_ui_path(),"about.ui"))
		self.ui.ver.setText(_("Version ")+ver())
		self.ui.li.setText("Written by Roderick MacKenzie 2012-2017, published under GPL v2.0")
		self.ui.setWindowIcon(QIcon(os.path.join(get_image_file_path(),"image.jpg")))
		self.ui.setWindowTitle(_("About (https://www.gpvdm.com)"))
		self.ui.show()
		pixmap = QPixmap(os.path.join(get_image_file_path(),"image.jpg"))
		self.ui.image.setPixmap(pixmap)



