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

from gui_util import dlg_get_text
from scan_select import select_param
from token_lib import tokens
from scan_item import scan_items_get_list

from plot_io import plot_save_oplot_file
from scan_io import scan_push_to_hpc
from scan_io import scan_import_from_hpc
from cal_path import get_exe_command
from icon_lib import QIcon_load
from scan_item import scan_items_get_file
from scan_item import scan_items_get_token
from util import str2bool

from scan_item import scan_items_lookup_item
from gui_util import tab_move_down
from gui_util import tab_move_up

import i18n
_ = i18n.language.gettext

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QTableWidget,QAbstractItemView, QMenuBar,QTableWidgetItem
from PyQt5.QtGui import QPainter,QIcon

from gui_util import tab_add
from gui_util import tab_remove
from gui_util import tab_get_value

from inp import inp_save_lines_to_file
from inp import inp_load_file
from inp import inp_isfile
from inp import inp_copy_file
from inp import inp_update_token_value
from inp import inp_get_token_value
from inp import inp_remove_file

from gpvdm_select import gpvdm_select

from scan_select import select_param

from cal_path import get_sim_path
from QWidgetSavePos import QWidgetSavePos
from window_list import resize_window_to_be_sane
from measure_ribbon import measure_ribbon
from measure_tab import measure_tab
from QHTabBar import QHTabBar
from util_zip import zip_lsdir

from util import strextract_interger

from gui_util import yes_no_dlg

def measure_new_filename():
	for i in range(0,20):
		pulse_name="measure"+str(i)+".inp"
		if inp_isfile(os.path.join(get_sim_path(),pulse_name))==False:
			return i
	return -1

class measure(QWidgetSavePos):


	def load_tabs(self):

		file_list=zip_lsdir(os.path.join(get_sim_path(),"sim.gpvdm"))
		files=[]
		for i in range(0,len(file_list)):
			if file_list[i].startswith("measure") and file_list[i].endswith(".inp"):
				name=inp_get_token_value(file_list[i], "#measurement_name")
				files.append([name,file_list[i]])

		files.sort()

		for i in range(0,len(files)):
			value=strextract_interger(files[i][1])
			if value!=-1:
				self.add_page(value)

	def add_page(self,index):
		tab=measure_tab(index)
		name=inp_get_token_value(tab.file_name, "#measure_name")
		self.notebook.addTab(tab,name)

	def __init__(self):
		QWidgetSavePos.__init__(self,"measure_window")
		resize_window_to_be_sane(self,0.5,0.7)

		self.setWindowIcon(QIcon_load("measure"))
		self.setWindowTitle(_("Measurment editor")+" (https://www.gpvdm.com)")


		self.vbox=QVBoxLayout()

		self.ribbon=measure_ribbon()
		self.ribbon.tb_new.triggered.connect(self.callback_add_page)
		self.ribbon.tb_rename.triggered.connect(self.callback_rename_page)
		self.ribbon.tb_clone.triggered.connect(self.callback_copy_page)
		self.ribbon.tb_delete.triggered.connect(self.callback_delete_page)		


		self.vbox.addWidget(self.ribbon)

		self.notebook = QTabWidget()
		self.notebook.setTabBar(QHTabBar())
		self.notebook.setTabPosition(QTabWidget.West)
		self.notebook.setMovable(True)

		self.vbox.addWidget(self.notebook)

		self.load_tabs()

		self.setLayout(self.vbox)


	def callback_add_page(self):
		new_sim_name=dlg_get_text( _("New measurement name")+":", _("measurement ")+str(self.notebook.count()+1),"document-new.png")

		if new_sim_name.ret!=None:
			index=measure_new_filename()
			inp_copy_file(os.path.join(get_sim_path(),"measure"+str(index)+".inp"),os.path.join(get_sim_path(),"measure0.inp"))
			inp_update_token_value(os.path.join(get_sim_path(),"measure"+str(index)+".inp"), "#measure_name", new_sim_name.ret)
			self.add_page(index)

	def callback_copy_page(self):
		tab = self.notebook.currentWidget()

		old_index=tab.index
		new_sim_name=dlg_get_text( _("Clone the current measurement to a new measurement called:"), _("measurement ")+str(self.notebook.count()+1),"clone.png")
		new_sim_name=new_sim_name.ret
		if new_sim_name!=None:
			index=measure_new_filename()
			if inp_copy_file(os.path.join(get_sim_path(),"measure"+str(index)+".inp"),os.path.join(get_sim_path(),"measure"+str(old_index)+".inp"))==False:
				print(_("Error copying file")+"measure"+str(old_index)+".inp")
				return

			inp_update_token_value(os.path.join(get_sim_path(),"measure"+str(index)+".inp"), "#measure_name", new_sim_name)
			self.add_page(index)

	def remove_invalid(self,input_name):
		return input_name.replace (" ", "_")

	def callback_rename_page(self):
		tab = self.notebook.currentWidget()
		name=inp_get_token_value(tab.file_name, "#measure_name")

		new_sim_name=dlg_get_text( _("Rename the measurement to be called")+":", name,"rename.png")

		new_sim_name=new_sim_name.ret

		if new_sim_name!=None:
			tab.rename(new_sim_name)
			index=self.notebook.currentIndex() 
			self.notebook.setTabText(index, new_sim_name)


	def callback_delete_page(self):

		tab = self.notebook.currentWidget()
		name=inp_get_token_value(tab.file_name, "#measure_name")

		response=yes_no_dlg(self,_("Should I remove the measurment file ")+name)

		if response == True:
			inp_remove_file(os.path.join(get_sim_path(),tab.file_name))
			index=self.notebook.currentIndex() 
			self.notebook.removeTab(index)
