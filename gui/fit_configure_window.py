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



import os
from tab import tab_class
from window_list import windows
from cal_path import get_image_file_path

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget
from PyQt5.QtGui import QPainter,QIcon

#python modules
import webbrowser

#windows
from tab import tab_class
from tab_lang import language_tab_class
from duplicate import duplicate
from fit_vars import fit_vars

from PyQt5.QtCore import pyqtSignal

articles = []
mesh_articles = []

class fit_configure_window(QWidget):

	changed = pyqtSignal()
	
	def callback_tab_changed(self):
		self.changed.emit()

	def __init__(self):
		QWidget.__init__(self)
		self.setFixedSize(900, 600)
		self.setWindowIcon(QIcon(os.path.join(get_image_file_path(),"cog.png")))

		self.setWindowTitle(_("Fit configure")+" (https://www.gpvdm.com)") 
		

		self.main_vbox = QVBoxLayout()

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))

		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		toolbar.addWidget(spacer)


		self.undo = QAction(QIcon(os.path.join(get_image_file_path(),"help.png")), _("Help"), self)
		self.undo.setStatusTip(_("Close"))
		self.undo.triggered.connect(self.callback_help)
		toolbar.addAction(self.undo)

		self.main_vbox.addWidget(toolbar)

		

		self.notebook = QTabWidget()

		self.notebook.setMovable(True)

		self.main_vbox.addWidget(self.notebook)

		files=["fit.inp"]
		description=[_("Configure minimizer")]

		for i in range(0,len(files)):
			tab=tab_class()
			tab.init(files[i],description[i])
			self.notebook.addTab(tab,description[i])

		self.duplicate_window=duplicate()
		self.notebook.addTab(self.duplicate_window,_("Duplicate window"))

		self.fit_vars_window=fit_vars()
		self.notebook.addTab(self.fit_vars_window,_("Fit variable window"))

		self.setLayout(self.main_vbox)
		self.win_list=windows()
		self.win_list.load()
		self.win_list.set_window(self,"fit_config_window")

		#self.connect("delete-event", self.callback_close_window) 

		#self.hide()

	def callback_help(self,widget):
		webbrowser.open('http://www.gpvdm.com/man/index.html')

	def closeEvent(self, event):
		self.win_list.update(self,"fit_config_window")
		#self.hide()



		#help_window().help_set_help(["duplicate.png",_("<big><b>The fitting variables window</b></big><br> Use this window to select the variables use to perform the fit.")])
		#help_window().help_set_help(["vars.png",_("<big><b>The fitting variables window</b></big><br> Use this window to select the variables use to perform the fit.")])
