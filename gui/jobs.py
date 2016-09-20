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



#import sys
import os
#import shutil
#import commands
from cal_path import get_image_file_path
from search import find_fit_log
from search import find_fit_speed_log
from window_list import windows
from inp import inp_load_file
from inp_util import inp_search_token_value
from status_icon import status_icon_stop


#qt
from PyQt5.QtCore import QSize, Qt
from PyQt5.QtGui import QIcon,QPalette
from PyQt5.QtWidgets import QWidget, QVBoxLayout,QProgressBar,QLabel,QDesktopWidget,QToolBar,QHBoxLayout,QAction, QSizePolicy, QTableWidget, QTableWidgetItem,QComboBox,QDialog,QAbstractItemView

from PyQt5.QtGui import QPixmap
from PyQt5.QtCore import pyqtSignal
from PyQt5.QtWidgets import QWidget

from gui_util import tab_add

class jobs_view(QWidget):

	def __init__(self):
		QWidget.__init__(self)

		self.main_vbox=QVBoxLayout()
	
		self.tab = QTableWidget()
		self.tab.resizeColumnsToContents()

		self.tab.verticalHeader().setVisible(False)

		self.create_model()

#		self.tab.cellChanged.connect(self.tab_changed)
		
		self.main_vbox.addWidget(self.tab)

		self.setLayout(self.main_vbox)
		self.show()


	def add_items(self):
		self.store.clear()
		actresses = [("n","name","done","status","target","ip","copystate","start","stop")]

		for act in actresses:
			self.store.store.append([act[0], act[1], act[2], act[3],act[4], act[5], act[6], act[7]])

	def load_data(self,jobs_list):
		self.create_model()
		for i in range(len(jobs_list)):
			tab_add(self.tab,jobs_list[i])

	def create_model(self):
		self.tab.clear()
		self.tab.setColumnCount(7)

		self.tab.setSelectionBehavior(QAbstractItemView.SelectRows)
		self.tab.setHorizontalHeaderLabels([_("n"), _("done"), _("status"), _("target"), _("ip"),_("copy state"),_("start"),_("stop")])

		self.tab.setRowCount(0)





