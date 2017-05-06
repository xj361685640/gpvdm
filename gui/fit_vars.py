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

from util import str2bool

import i18n
_ = i18n.language.gettext

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QTableWidget,QAbstractItemView, QMenuBar, QTableWidgetItem
from PyQt5.QtGui import QPainter,QIcon

from gui_util import tab_remove
from gui_util import tab_get_value

from inp import inp_load_file
from inp import inp_save_lines_to_file
from gpvdm_select import gpvdm_select

from open_save_dlg import open_as_filter
from gui_util import error_dlg
from cal_path import get_sim_path

class fit_vars(QWidget):

	def insert_row(self,i,f,t,p,v,section):
		self.tab.blockSignals(True)
		self.tab.insertRow(i)

		item = QTableWidgetItem(f)
		self.tab.setItem(i,0,item)

		item = QTableWidgetItem(t)
		self.tab.setItem(i,1,item)


		self.item = gpvdm_select()
		self.item.setText(p)
		self.item.button.clicked.connect(self.callback_show_list)

		self.tab.setCellWidget(i,2,self.item)

		item = QTableWidgetItem(v)
		self.tab.setItem(i,3,item)

		item = QTableWidgetItem(section)
		self.tab.setItem(i,4,item)

		self.tab.blockSignals(False)
		
	def callback_add_item(self):
		self.insert_row(self.tab.rowCount(),_("File"),_("token"),_("path"),"1","0")
		self.save_combo()
		
	def callback_open(self):
		file_name=open_as_filter(self,"dat (*.dat);;csv (*.csv);;txt (*.txt);;inp (*.inp);;omat (*.omat)")

		if file_name!=None:
			lines=[]
			inp_load_file(lines,file_name)
			begin=-1
			end=-1

			for i in range(0,len(lines)):
				if lines[i]=="#data":
					begin=i
				if lines[i]=="#end":
					end=i
				
			if begin==-1 or end==-1:
				error_dlg(self,_("The data file does not have a #begin or #end token indicating where the data starts and ends."))
				return

			rel_path=os.path.relpath(file_name, get_sim_path())
			self.insert_row(self.tab.rowCount(),rel_path,"#data",rel_path,str(end-begin),"1")
			self.save_combo()

	def callback_show_list(self):
		self.select_param_window.update()
		self.select_param_window.show()

	def callback_delete_item(self):
		tab_remove(self.tab)
		self.save_combo()

	def save_combo(self):
		lines=[]

		for i in range(0,self.tab.rowCount()):
			lines.append(str(tab_get_value(self.tab,i, 1)))
			lines.append(str(tab_get_value(self.tab,i, 0)))
			lines.append(str(tab_get_value(self.tab,i, 2)))
			lines.append(str(tab_get_value(self.tab,i, 3)))
			lines.append(str(tab_get_value(self.tab,i, 4)))

		lines.append("#ver")
		lines.append("1.0")
		lines.append("#end")

		inp_save_lines_to_file(self.file_name,lines)

	def tab_changed(self):
		self.save_combo()

	def create_model(self):
		self.tab.clear()
		self.tab.setColumnCount(5)
		self.tab.setSelectionBehavior(QAbstractItemView.SelectRows)
		self.tab.setHorizontalHeaderLabels([_("File"), _("Token"), _("Path"),_("Lines to edit"),_("Line section to edit")])
		self.tab.setColumnWidth(2, 400)
		self.file_name=os.path.join(get_sim_path(),"fit_vars.inp")

		lines=[]
		pos=0

		if inp_load_file(lines,self.file_name)==True:
			mylen=len(lines)
			line=0
			while(1):
				t=lines[pos]
				if t=="#end":
					break
				pos=pos+1

				f=lines[pos]
				if f=="#end":
					break
				pos=pos+1

				p=lines[pos]
				if p=="#end":
					break
				pos=pos+1

				v=lines[pos]
				if v=="#end":
					break
				pos=pos+1

				section=lines[pos]
				if section=="#end":
					break
				pos=pos+1
				
				self.insert_row(line,f,t,p,v,section)

				if pos>mylen:
					break

				line=line+1

	def __init__(self):
		QWidget.__init__(self)

		self.setWindowTitle(_("Fit vars window - gpvdm"))   
		self.setWindowIcon(QIcon_load("fit"))
		self.setFixedSize(900, 700)

		self.select_param_window=select_param()

		self.vbox=QVBoxLayout()

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))

		self.tb_save = QAction(QIcon_load("list-add"), _("Add line"), self)
		self.tb_save.triggered.connect(self.callback_add_item)
		toolbar.addAction(self.tb_save)

		self.tb_save = QAction(QIcon_load("list-remove"), _("Remove line"), self)
		self.tb_save.triggered.connect(self.callback_delete_item)
		toolbar.addAction(self.tb_save)

		self.tb_open = QAction(QIcon_load("document-open"), _("Open"), self)
		self.tb_open.triggered.connect(self.callback_open)
		toolbar.addAction(self.tb_open)
		
		self.vbox.addWidget(toolbar)

		self.tab = QTableWidget()
		self.tab.resizeColumnsToContents()

		self.tab.verticalHeader().setVisible(False)
		self.create_model()

		self.tab.cellChanged.connect(self.tab_changed)

		self.vbox.addWidget(self.tab)

		self.select_param_window.init(self.tab)
		self.select_param_window.set_save_function(self.save_combo)

		self.setLayout(self.vbox)
