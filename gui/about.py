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
from PyQt5.QtWidgets import QDialog,QListWidgetItem,QListView
from PyQt5.QtGui import QPixmap
from PyQt5.QtCore import QSize
from PyQt5.QtGui import QIcon

from cal_path import get_materials_dirs
from ref import get_ref_text

class about_dlg():
	def __init__(self):
		#QDialog.__init__(self)
		self.window = loadUi(os.path.join(get_ui_path(),"about.ui"))
		self.window.ver.setText(_("Version ")+ver())
		self.window.li.setText("Written by Roderick MacKenzie 2012-2017, published under GPL v2.0")
		self.window.setWindowIcon(QIcon(os.path.join(get_image_file_path(),"image.jpg")))
		self.window.setWindowTitle(_("About")+" (https://www.gpvdm.com)")

		self.window.materials.setIconSize(QSize(32,32))
		self.window.materials.setViewMode(QListView.ListMode)
		self.window.materials.setSpacing(8)
		self.window.materials.setWordWrap(True)
		gridsize=self.window.materials.size()
		#gridsize.setWidth(80)
		#gridsize.setHeight(80)

		self.window.materials.setGridSize(gridsize)
		self.mat_icon = QIcon(QPixmap(os.path.join(get_image_file_path(),"organic_material.png")))
		self.fill_store()

		self.window.show()
		pixmap = QPixmap(os.path.join(get_image_file_path(),"image.jpg"))
		self.window.image.setPixmap(pixmap)

		
	def fill_store(self):
		self.window.materials.clear()

		all_files=get_materials_dirs()
		for fl in all_files:
			text=get_ref_text(os.path.join(fl,"n.inp"),html=False)
			if text!=None:
				itm = QListWidgetItem(os.path.basename(fl)+" "+text)
				itm.setIcon(self.mat_icon)
				itm.setToolTip(text)
				self.window.materials.addItem(itm)

