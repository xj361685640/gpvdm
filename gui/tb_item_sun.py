#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2017 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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

## @package tb_item_sun
#  A toolbar item to select the sun's intensity.
#


#inp
import os
from inp import inp_update_token_value
from inp import inp_get_token_value

#from global_objects import global_object_get
#from global_objects import global_isobject

import i18n
_ = i18n.language.gettext

#qt
from PyQt5.QtWidgets import QMainWindow, QTextEdit, QAction, QApplication
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QSizePolicy,QVBoxLayout,QPushButton,QDialog,QFileDialog,QToolBar,QLabel,QComboBox
from PyQt5.QtCore import pyqtSignal

from cal_path import get_sim_path

class tb_item_sun(QWidget):

	changed = pyqtSignal()
	
	def call_back_light_changed(self):
		light_power=self.light.currentText()
		inp_update_token_value(os.path.join(get_sim_path(),"light.inp"), "#Psun", light_power)
		self.changed.emit()

	def update(self):
		self.light.blockSignals(True)
		self.light.clear()
		sun_values=["0.0","0.01","0.1","1.0","10"]

		token=inp_get_token_value(os.path.join(get_sim_path(),"light.inp"), "#Psun")
		if sun_values.count(token)==0:
			sun_values.append(token)

		for i in range(0,len(sun_values)):
			self.light.addItem(sun_values[i])

		all_items  = [self.light.itemText(i) for i in range(self.light.count())]
		for i in range(0,len(all_items)):
		    if all_items[i] == token:
		        self.light.setCurrentIndex(i)
		self.light.blockSignals(False)


	def __init__(self):
		QWidget.__init__(self)

		layout=QVBoxLayout()
		label=QLabel()
		label.setText(_("Light intensity")+" ("+_("Suns")+"):")
		layout.addWidget(label)

		self.light = QComboBox(self)
		self.light.setEditable(True)


		layout.addWidget(self.light)

		self.setLayout(layout)

		self.update()
		
		self.light.currentIndexChanged.connect(self.call_back_light_changed)
		self.light.editTextChanged.connect(self.call_back_light_changed)


