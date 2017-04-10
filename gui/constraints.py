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
from icon_lib import QIcon_load
from scan_item import scan_items_get_file
from scan_item import scan_items_get_token

from gpvdm_select import gpvdm_select

from window_list import windows

from util import str2bool

import i18n
_ = i18n.language.gettext

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QTableWidget,QAbstractItemView, QMenuBar, QTableWidgetItem
from PyQt5.QtGui import QPainter,QIcon

from gui_util import tab_add
from gui_util import tab_remove
from gui_util import tab_get_value

from inp import inp_save_lines
from inp import inp_load_file

from scan_item import scan_items_lookup_item

class constraints(QWidget):

	def insert_row(self,i,constraint_type,file_name,token,path,function,max_value,min_value,error):
		self.tab.blockSignals(True)
		self.tab.insertRow(i)

		item = QTableWidgetItem(constraint_type)
		self.tab.setItem(i,0,item)

		item = QTableWidgetItem(file_name)
		self.tab.setItem(i,1,item)

		item = QTableWidgetItem(token)
		self.tab.setItem(i,2,item)

		self.item = gpvdm_select()
		self.item.setText(path)
		self.item.button.clicked.connect(self.callback_show_list_src)
		self.tab.setCellWidget(i,3,self.item)


		item = QTableWidgetItem(function)
		self.tab.setItem(i,4,item)

		item = QTableWidgetItem(max_value)
		self.tab.setItem(i,5,item)

		item = QTableWidgetItem(min_value)
		self.tab.setItem(i,6,item)

		item = QTableWidgetItem(error)
		self.tab.setItem(i,7,item)
		
		self.tab.blockSignals(False)

	def callback_show_list_src(self):
		self.select_param_window_src.update()
		self.select_param_window_src.show()

	def callback_show_list_dest(self):
		self.select_param_window_dest.update()
		self.select_param_window_dest.show()
		
	def callback_add_item(self):
		self.insert_row(self.tab.rowCount(),_("Constraint type"),_("File"),_("Token"),_("Path"),_("Function"),_("Max"),_("Min"),_("Error"))
		self.save_combo()

	def callback_delete_item(self):
		tab_remove(self.tab)
		self.save_combo()

	def save_combo(self):
		lines=[]
		for i in range(0,self.tab.rowCount()):
			line=str(tab_get_value(self.tab,i, 0))+" "+str(tab_get_value(self.tab,i, 1))+" "+str(tab_get_value(self.tab,i, 2))+" "+str(tab_get_value(self.tab,i, 4))+" "+str(tab_get_value(self.tab,i, 5))+" "+str(tab_get_value(self.tab,i, 6))+" "+str(tab_get_value(self.tab,i, 7))
			lines.append(line)
		lines.append("#end")
		print("save as",self.file_name)
		inp_save_lines(self.file_name,lines)


	def tab_changed(self):
		self.save_combo()
		

	def create_model(self):
		self.tab.clear()
		self.tab.setColumnCount(8)
		self.tab.setSelectionBehavior(QAbstractItemView.SelectRows)
		self.tab.setHorizontalHeaderLabels([_("Constraint type"), _("File"), _("Token"),_("Path"),_("Function"), _("Max"), _("Min"),_("Error")])
		#self.tab.setColumnWidth(2, 200)
		#self.tab.setColumnWidth(5, 200)
		self.file_name="constraints.inp"

		lines=[]
		pos=0

		if inp_load_file(lines,self.file_name)==True:
			mylen=len(lines)
			while(1):

				if lines[pos]=="#end":
					break
				line=lines[pos].split()
				print(line)
				path=scan_items_lookup_item(line[1],line[2])
				self.insert_row(self.tab.rowCount(),line[0],line[1],line[2],path,line[3],line[4],line[5],line[6])

				pos=pos+1


	def __init__(self):
		QWidget.__init__(self)
		self.setWindowTitle(_("Fit constraints window")+" - https://www.gpvdm.com")   
		self.setWindowIcon(QIcon_load("constraints"))
		self.setFixedSize(900, 700)

		self.win_list=windows()
		self.win_list.load()
		self.win_list.set_window(self,"fit_constraints_window")
		
		self.vbox=QVBoxLayout()

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))

		self.tb_save = QAction(QIcon_load("list-add"), _("Add"), self)
		self.tb_save.triggered.connect(self.callback_add_item)
		toolbar.addAction(self.tb_save)

		self.tb_save = QAction(QIcon_load("list-remove"), _("Minus"), self)
		self.tb_save.triggered.connect(self.callback_delete_item)
		toolbar.addAction(self.tb_save)

		self.vbox.addWidget(toolbar)

		self.tab = QTableWidget()
		self.tab.resizeColumnsToContents()

		self.tab.verticalHeader().setVisible(False)
		self.create_model()

		self.tab.cellChanged.connect(self.tab_changed)

		self.select_param_window_src=select_param()
		self.select_param_window_src.init(self.tab)
		self.select_param_window_src.set_save_function(self.save_combo)
		self.select_param_window_src.file_name_tab_pos=1
		self.select_param_window_src.token_tab_pos=2
		self.select_param_window_src.path_tab_pos=3

		
		self.vbox.addWidget(self.tab)


		self.setLayout(self.vbox)

	def closeEvent(self, event):
		self.win_list.update(self,"fit_constraints_window")
		self.hide()
