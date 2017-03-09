# -*- coding: utf-8 -*-
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
from cal_path import get_materials_path

import sys

from PyQt5.uic import loadUi
from PyQt5.QtWidgets import QPushButton,QListWidget,QTextBrowser,QTabWidget,QWidget,QHBoxLayout,QLabel,QDialog,QVBoxLayout,QListWidgetItem,QListView
from PyQt5.QtGui import QPixmap
from PyQt5.QtCore import QSize
from PyQt5.QtGui import QIcon

from cal_path import find_materials
from ref import get_ref_text

class about_dlg(QDialog):
	def __init__(self):
		QDialog.__init__(self)
		self.main_hbox=QHBoxLayout()
		self.left_vbox=QVBoxLayout()
		self.main_vbox=QVBoxLayout()
		self.setFixedSize(650,480) 
		self.setWindowTitle(_("About")+" (https://www.gpvdm.com)")
		self.setWindowIcon(QIcon(os.path.join(get_image_file_path(),"image.jpg")))
		self.gpvdm=QLabel("<font size=40><b>gpvdm</b></font>")
		self.image=QLabel()
		self.written_by=QLabel(_("Written by Roderick MacKenzie 2012-2017, published under GPL v2.0"))
		self.written_by.setWordWrap(True)
		self.ver=QLabel(_("Version ")+ver())
		pixmap = QPixmap(os.path.join(get_image_file_path(),"image.jpg"))
		self.image.setPixmap(pixmap)
		self.left_vbox.addWidget(self.gpvdm)
		self.left_vbox.addWidget(self.image)
		self.left_vbox.addWidget(self.written_by)
		self.left_vbox.addWidget(self.ver)
		self.left=QWidget()
		self.left.setLayout(self.left_vbox)
		self.right=QTabWidget()
		self.right.setMinimumWidth(400)
		self.about=QTextBrowser()
		text=""
		text=text+_("gpvdm is a free general-purpose tool for simulation of light harvesting devices. It was originally written to simulate organic solar cells and OLEDs, but it has recently been extended to simulate other devices including silicon based devices. Currently the model can sumulate:")
		text=text+"<ul>"
		text=text+"<li>"+_("Organic solar cells")+"</li>"
		text=text+"<li>"+_("Organic LEDs")+"</li>"
		text=text+"<li>"+_("Crystalline silicon solar cells")+"</li>"
		text=text+"<li>"+_("a-Si solar cells")+"</li>"
		text=text+"<li>"+_("CIGS solar cells")+"</li>"
		text=text+"</ul> "
		text=text+_("The model solves both electron and hole drift-diffusion, and carrier continuity equations in position space to describe the movement of charge within the device. The model also solves Poisson's equation to calculate the internal electrostatic potential. Recombination and carrier trapping are described within the model using a Shockley-Read-Hall (SRH) formalism, the distribution of trap sates can be arbitrarily defined. All equations can be solved either in steady state or in time domain. A fuller description of the model can be found in the here, in the associated publications and in the manual.")
		text=text+"<br>"
		text=text+"<br>"
		text=text+"<center><a href=\"https://www.gpvdm.com\">https://www.gpvdm.com</a></center>"
		self.about.setText(text)
		self.right.addTab(self.about,_("About"))
		
		self.license=QTextBrowser()
		text=""
		text=text+_("General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall model")
		text=text+"<br>"
		text=text+"<br>"

		text=text+_("This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License v2.0, as published by the Free Software Foundation.")
		text=text+"<br>"
		text=text+"<br>"

		text=text+_("This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.")
		text=text+"<br>"
		text=text+"<br>"

		text=text+_("You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.")
		text=text+"<br>"
		text=text+"<br>"
		
		text=text+_("When you start gpvdm the software checks the gpvdm.com server for updates and bug reports.  To do this it transmits your opperating system type, gpvdm version number.  It also tells the gpvdm.com server if opengl is working on your pc.  By installing gpvdm you agree that the gpvdm software may transmit this information to the gpvdm.com server.  The communications  process between gpvdm software and the gpvdm.com server is described in more detail in the manual.")
		text=text+"<br>"
		text=text+"<br>"
		text=text+"roderick.mackenzie@nottingham.ac.uk"
		text=text+"<br>"
		self.license.setText(text)
		self.right.addTab(self.license,_("License"))
		
		self.translations=QTextBrowser()
		text=""

		text=text+"<big><b>Translations of gpvdm:</b></big>"
		text=text+"<br>"
		text=text+"<br>"
		text=text+"<center>"
		text=text+"<b>English</b>: Roderick C. I. MacKenzie"
		text=text+"<br>"
		text=text+"<b>Chinese</b>:Liu Di (刘迪) and Zhao Chenyao (赵辰尧)"
		text=text+"<br>"
		text=text+"<b>German</b>: Roderick C. I. MacKenzie"
		text=text+"<br>"
		text=text+"<b>Spanish</b>:Translator needed!"
		text=text+"<br>"
		text=text+"<b>Korean</b>:Translator needed!"
		text=text+"<br>"
		text=text+"<b>Persian</b>:Translator needed!"
		text=text+"<br>"
		text=text+"<b>Your language</b>: Translator needed!"
		text=text+"</center>"
		text=text+"<br>"
		text=text+"<br>"
		text=text+"<big>Would you like gpvdm translated into your language?</big>"
		text=text+"<br>"
		text=text+"<big>Would you like your name in the credits of gpvdm?</big>"
		text=text+"<br>"
		text=text+"<br>"
		text=text+"If so then please consider joining the gpvdm translation effort.  This is somthing you can put on your CV and it\'s a way to make sure that speakers of your language have access to high quality scientific tools for simulating solar cells."

		self.translations.setText(text)
		self.right.addTab(self.translations,_("Translations"))

		self.materials=QListWidget()
		self.right.addTab(self.materials,_("Materials"))

		self.main_hbox.addWidget(self.left)
		self.main_hbox.addWidget(self.right)
		self.widget_main_hbox=QWidget()
		self.widget_main_hbox.setLayout(self.main_hbox)
		self.main_vbox.addWidget(self.widget_main_hbox)
		
		self.hwidget=QWidget()

		self.closeButton = QPushButton(_("Close"))
		self.closeButton.clicked.connect(self.callback_close)
		hbox = QHBoxLayout()
		hbox.addStretch(1)
		hbox.addWidget(self.closeButton)

		self.hwidget.setLayout(hbox)
		
		self.main_vbox.addWidget(self.hwidget)

		self.setLayout(self.main_vbox)
		self.show()


		#QDialog.__init__(self)

		self.materials.setIconSize(QSize(32,32))
		self.materials.setViewMode(QListView.ListMode)
		self.materials.setSpacing(8)
		self.materials.setWordWrap(True)
		gridsize=self.materials.size()
		#gridsize.setWidth(80)
		gridsize.setHeight(40)

		self.materials.setGridSize(gridsize)
		self.mat_icon = QIcon(QPixmap(os.path.join(get_image_file_path(),"organic_material.png")))
		self.fill_store()

	def callback_close(self):
		self.close()
		
	def fill_store(self):
		self.materials.clear()
	
		all_files=find_materials()
		for fl in all_files:
			text=get_ref_text(os.path.join(get_materials_path(),fl,"n.omat"),html=False)
			if text!=None:
				itm = QListWidgetItem(os.path.basename(fl)+" "+text)
				itm.setIcon(self.mat_icon)
				itm.setToolTip(text)
				self.materials.addItem(itm)

