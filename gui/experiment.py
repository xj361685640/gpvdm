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

## @package experiment
#  The main experiment window, used for configuring time domain experiments.
#

import os
from gui_util import dlg_get_text
import webbrowser
from code_ctrl import enable_betafeatures
from inp import inp_update_token_value
from util_zip import zip_lsdir
from inp import inp_isfile
from inp import inp_copy_file
from inp import inp_remove_file
from util import strextract_interger
from global_objects import global_object_get
from icon_lib import icon_get
from global_objects import global_object_register
import i18n
_ = i18n.language.gettext

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QMenuBar,QStatusBar
from PyQt5.QtGui import QPainter,QIcon

#window
from experiment_tab import experiment_tab
from QHTabBar import QHTabBar
from gui_util import yes_no_dlg
from PyQt5.QtCore import pyqtSignal
from util import wrap_text
from QWidgetSavePos import QWidgetSavePos
from cal_path import get_sim_path

from inp import inp_get_token_value
from experiment_util import experiment_new_filename

from css import css_apply

from progress import progress_class
from process_events import process_events

from timedomain_ribbon import timedomain_ribbon
 
class experiment(QWidgetSavePos):

	changed = pyqtSignal()
	
	def update(self):
		for item in self.notebook.get_children():
			item.update()

	def callback_help(self):
		webbrowser.open('http://www.gpvdm.com/man/index.html')

	def callback_add_page(self):
		new_sim_name=dlg_get_text( _("New experiment name")+":", _("experiment ")+str(self.notebook.count()+1),"document-new.png")

		if new_sim_name.ret!=None:
			index=experiment_new_filename("pulse")
			inp_copy_file(os.path.join(get_sim_path(),"pulse"+str(index)+".inp"),os.path.join(get_sim_path(),"pulse0.inp"))
			inp_copy_file(os.path.join(get_sim_path(),"time_mesh_config"+str(index)+".inp"),os.path.join(get_sim_path(),"time_mesh_config0.inp"))
			inp_update_token_value(os.path.join(get_sim_path(),"pulse"+str(index)+".inp"), "#sim_menu_name", new_sim_name.ret+"@pulse")
			self.add_page(index)
			self.changed.emit()

	def callback_save(self):
		tab = self.notebook.currentWidget()
		tab.image_save()

	def callback_copy_page(self):
		tab = self.notebook.currentWidget()

		old_index=tab.index
		new_sim_name=dlg_get_text( _("Clone the current experiment to a new experiment called:"), tab.tab_name.split("@")[0],"clone.png")
		new_sim_name=new_sim_name.ret
		if new_sim_name!=None:
			new_sim_name=new_sim_name+"@"+tab.tab_name.split("@")[1]
			index=experiment_new_filename("pulse")
			if inp_copy_file(os.path.join(get_sim_path(),"pulse"+str(index)+".inp"),os.path.join(get_sim_path(),"pulse"+str(old_index)+".inp"))==False:
				print(_("Error copying file")+"pulse"+str(old_index)+".inp")
				return
			if inp_copy_file(os.path.join(get_sim_path(),"time_mesh_config"+str(index)+".inp"),os.path.join(get_sim_path(),"time_mesh_config"+str(old_index)+".inp"))==False:
				print(_("Error copying file")+"pulse"+str(old_index)+".inp")
				return

			inp_update_token_value(os.path.join(get_sim_path(),"pulse"+str(index)+".inp"), "#sim_menu_name", new_sim_name)
			self.add_page(index)
			self.changed.emit()

	def remove_invalid(self,input_name):
		return input_name.replace (" ", "_")

	def callback_rename_page(self):
		tab = self.notebook.currentWidget()

		new_sim_name=dlg_get_text( _("Rename the experiment to be called")+":", tab.tab_name.split("@")[0],"rename.png")

		new_sim_name=new_sim_name.ret

		if new_sim_name!=None:
			#new_sim_name=self.remove_invalid(new_sim_name)
			#new_dir=os.path.join(self.sim_dir,new_sim_name)
			#shutil.move(old_dir, new_dir)
			tab.rename(new_sim_name)
			index=self.notebook.currentIndex() 
			self.notebook.setTabText(index, new_sim_name)
			self.changed.emit()

	def callback_laser_start_time(self):
		tab = self.notebook.currentWidget()
		tab.tmesh.callback_laser()

	def callback_start_time(self):
		tab = self.notebook.currentWidget()
		tab.tmesh.callback_start_time()

	def callback_delete_page(self):

		tab = self.notebook.currentWidget()

		response=yes_no_dlg(self,_("Should I remove the experiment file ")+tab.tab_name.split("@")[0])


		if response == True:
			inp_remove_file(os.path.join(get_sim_path(),"pulse"+str(tab.index)+".inp"))
			inp_remove_file(os.path.join(get_sim_path(),"time_mesh_config"+str(tab.index)+".inp"))
			index=self.notebook.currentIndex() 
			self.notebook.removeTab(index)
			self.changed.emit()

	def load_tabs(self):

		progress_window=progress_class()
		progress_window.show()
		progress_window.start()

		process_events()

		file_list=zip_lsdir(os.path.join(get_sim_path(),"sim.gpvdm"))
		files=[]
		for i in range(0,len(file_list)):
			if file_list[i].startswith("pulse") and file_list[i].endswith(".inp"):
				name=inp_get_token_value(file_list[i], "#sim_menu_name")
				files.append([name,file_list[i]])

		files.sort()

		for i in range(0,len(files)):
			value=strextract_interger(files[i][1])
			if value!=-1:
				self.add_page(value)

			progress_window.set_fraction(float(i)/float(len(files)))
			progress_window.set_text(_("Loading")+" "+files[i][0])
			process_events()

		progress_window.stop()

	def clear_pages(self):
		self.notebook.clear()

	def add_page(self,index):
		tab=experiment_tab(index)
		self.notebook.addTab(tab,tab.tab_name.split("@")[0])

	def switch_page(self):
		tab = self.notebook.currentWidget()
		self.ribbon.tb_lasers.update("pulse"+str(tab.tmesh.index)+".inp")

		#pageNum = self.notebook.get_current_page()
		#tab = self.notebook.get_nth_page(pageNum)
		#self.status_bar.push(self.context_id, tab.tab_name)

	def __init__(self):
		QWidgetSavePos.__init__(self,"experiment")
		self.main_vbox = QVBoxLayout()


		self.setMinimumSize(1200, 700)
		self.setWindowTitle(_("Time domain experiment window")+" (https://www.gpvdm.com)") 
		self.setWindowIcon(icon_get("icon"))

		#toolbar=QToolBar()
		#toolbar.setToolButtonStyle( Qt.ToolButtonTextUnderIcon)
		#toolbar.setIconSize(QSize(48, 48))

		self.ribbon=timedomain_ribbon()

		self.ribbon.new.triggered.connect(self.callback_add_page)
		self.ribbon.delete.triggered.connect(self.callback_delete_page)
		self.ribbon.clone.triggered.connect(self.callback_copy_page)
		self.ribbon.rename.triggered.connect(self.callback_rename_page)
		self.ribbon.tb_save.triggered.connect(self.callback_save)


		self.ribbon.tb_laser_start_time.triggered.connect(self.callback_laser_start_time)

		self.ribbon.tb_start.triggered.connect(self.callback_start_time)

		#spacer = QWidget()
		#spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		#toolbar.addWidget(spacer)


		#self.help = QAction(icon_get("help"), _("Help"), self)
		#self.help.setStatusTip(_("Close"))
		#self.help.triggered.connect(self.callback_help)
		#toolbar.addAction(self.help)

		self.main_vbox.addWidget(self.ribbon)


		self.notebook = QTabWidget()
		css_apply(self.notebook ,"style_h.css")

		self.notebook.setTabBar(QHTabBar())
		self.notebook.setTabPosition(QTabWidget.West)
		self.notebook.setMovable(True)

		self.load_tabs()

		self.main_vbox.addWidget(self.notebook)


		self.status_bar=QStatusBar()
		self.main_vbox.addWidget(self.status_bar)


		self.setLayout(self.main_vbox)
		
		self.notebook.currentChanged.connect(self.switch_page)
		self.switch_page()

