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

from scan_select import select_param
from token_lib import tokens
from scan_item import scan_items_get_list

from scan_item import scan_item_save
from scan_plot import scan_gen_plot_data
from scan_io import scan_clean_dir
from scan_io import scan_clean_unconverged
from scan_io import scan_clean_simulation_output
from scan_io import scan_nested_simulation
from server import server_find_simulations_to_run
from scan_io import scan_plot_fits

from plot_io import plot_save_oplot_file
from scan_io import scan_push_to_hpc
from scan_io import scan_import_from_hpc
from cal_path import get_exe_command
from cal_path import get_image_file_path
from scan_item import scan_items_get_file
from scan_item import scan_items_get_token

from window_list import windows

from util import str2bool

import i18n
_ = i18n.language.gettext

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QTableWidget,QAbstractItemView, QMenuBar
from PyQt5.QtGui import QPainter,QIcon

from gui_util import tab_add
from gui_util import tab_remove
from gui_util import tab_get_value

from inp import inp_load_file

class duplicate(QWidget):


	def callback_add_item(self):
		tab_add(self.tab,["File","token",_("value")])
		self.save_combo()

	def callback_delete_item(self):
		tab_remove(self.tab)
		self.save_combo()

	def save_combo(self):
		return

		a = open(self.file_name, "w")

		for i in range(0,self.tab.rowCount()):
			a.write(str(tab_get_value(self.tab,i, 1))+"\n")
			a.write(str(tab_get_value(self.tab,i, 0))+"\n")
			a.write(str(tab_get_value(self.tab,i, 2))+"\n")

		a.write("#end\n")

		a.close()


	def tab_changed(self):
		self.save_combo()
		

	def create_model(self):
		self.tab.clear()
		self.tab.setColumnCount(4)
		self.tab.setSelectionBehavior(QAbstractItemView.SelectRows)
		self.tab.setHorizontalHeaderLabels([_("src File"), _("src Token"), _("dest File"), _("dest Token")])
		#self.tab.setColumnWidth(1, 400)
		self.file_name="duplicate.inp"

		lines=[]
		pos=0

		if inp_load_file(lines,self.file_name)==True:
			mylen=len(lines)
			while(1):
				data=lines[pos]
				if data=="#end":
					break
				pos=pos+1
				data=data.split(" ")
				tab_add(self.tab,[data[0],data[1],data[2],data[3]])

				if pos>mylen:
					break

	def __init__(self):
		QWidget.__init__(self)
		self.setWindowTitle(_("Fit variables duplicate window")+" - https://www.gpvdm.com")   
		self.setWindowIcon(QIcon(os.path.join(get_image_file_path(),"duplicate.png")))
		self.setFixedSize(900, 700)

		self.win_list=windows()
		self.win_list.load()
		self.win_list.set_window(self,"fit_duplicate_window")
		
		self.vbox=QVBoxLayout()

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))

		self.tb_save = QAction(QIcon(os.path.join(get_image_file_path(),"add.png")), _("Add"), self)
		self.tb_save.triggered.connect(self.callback_add_item)
		toolbar.addAction(self.tb_save)

		self.tb_save = QAction(QIcon(os.path.join(get_image_file_path(),"minus.png")), _("Minus"), self)
		self.tb_save.triggered.connect(self.callback_delete_item)
		toolbar.addAction(self.tb_save)

		self.vbox.addWidget(toolbar)

		self.tab = QTableWidget()
		self.tab.resizeColumnsToContents()

		self.tab.verticalHeader().setVisible(False)
		self.create_model()

		self.tab.cellChanged.connect(self.tab_changed)

		self.vbox.addWidget(self.tab)


		self.setLayout(self.vbox)

	def closeEvent(self, event):
		self.win_list.update(self,"fit_duplicate_window")
		self.hide()
