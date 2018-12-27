#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2016 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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

## @package tb_optical_model
#  A toolbar item to select the optical model.
#

import os
from inp import inp_update_token_value
from inp import inp_get_token_value
import zipfile
import glob
from tab import tab_class

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QSystemTrayIcon,QMenu, QComboBox, QMenuBar, QLabel
from PyQt5.QtGui import QIcon

from cal_path import get_sim_path
from cal_path import get_plugins_path

class tb_optical_model(QWidget):

	def __init__(self):
		QWidget.__init__(self)
		self.dump_dir=os.path.join(get_sim_path(),"light_dump")

		self.layout=QVBoxLayout()
		label=QLabel(_("Optical model")+":")
		self.layout.addWidget(label)

		self.cb = QComboBox()
		self.cb.activated.connect(self.on_cb_model_changed)
		self.layout.addWidget(self.cb)
		self.update()
		self.setLayout(self.layout)
		self.show()

	def get_text(self):
		out=self.cb.currentText()
		out=out.split(" ")[0]
		return out

	def file_name_set_start(self,start):
		self.start=start

	def file_name_set_end(self,end):
		self.end=end

	def find_models(self):
		ret=[]
		path=get_plugins_path()

		for file in glob.glob(os.path.join(path,"*")):
			file_name=os.path.basename(file)
			if file_name.startswith("light_"):
				if file_name.endswith(".dll") or file_name.endswith(".so"):
					ret.append(os.path.basename(file_name[6:]).split(".")[0])

		return ret

	def on_cb_model_changed(self):
		cb_text=self.cb.currentText()
		inp_update_token_value("light.inp", "#light_model", cb_text)
		
		
	def update(self):
		self.cb.blockSignals(True)

		self.cb.clear()
		models=self.find_models()
		if len(models)==0:
			error_dlg(self,_("I can't find any optical plugins, I think the model is not installed properly."))
			return

		for i in range(0, len(models)):
			self.cb.addItem(models[i])

		used_model=inp_get_token_value(os.path.join(get_sim_path(),"light.inp"), "#light_model")
		print(models,used_model)
		if models.count(used_model)==0:
			used_model="exp"
			inp_update_token_value(os.path.join(get_sim_path(),"light.inp"), "#light_model","exp")
			self.cb.setCurrentIndex(self.cb.findText(used_model))
		else:
			self.cb.setCurrentIndex(self.cb.findText(used_model))

		self.cb.blockSignals(False)

