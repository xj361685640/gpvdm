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
import shutil
import webbrowser
from code_ctrl import enable_betafeatures
from util_zip import zip_lsdir
from util import strextract_interger
from global_objects import global_object_get
from icon_lib import icon_get

from global_objects import global_object_register
from server import server_get
from help import help_window

import i18n
_ = i18n.language.gettext

#inp
from inp import inp_isfile
from inp import inp_copy_file
from inp import inp_remove_file
from inp import inp_update_token_value
from fit_configure_window import fit_configure_window

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QStatusBar, QTableWidget, QAbstractItemView
from PyQt5.QtGui import QPainter,QIcon,QCursor

#windows
from gui_util import yes_no_dlg

from fit_tab import fit_tab
from QHTabBar import QHTabBar

from gui_util import dlg_get_text

from fit_progress import fit_progress
from inp import inp_get_token_value
from util import str2bool

from util import wrap_text
from cal_path import get_sim_path
from QWidgetSavePos import QWidgetSavePos

from css import css_apply
from cal_path import get_inp_file_path

from order_widget import order_widget

class fit_window(QWidgetSavePos):

	def update(self):
		for i in range(0,self.notebook.count()):
			tab = self.notebook.widget(i)
			tab.update()

	def callback_stop(self):
		my_server=server_get()
		my_server.force_stop()

	def callback_configure(self):
		if self.fit_configure_window==None:
			self.fit_configure_window=fit_configure_window("fit_config")
			
		help_window().help_set_help(["vars.png",_("<big><b>The fitting variables window</b></big><br> Use this window to select the variables use to perform the fit.")])
		if self.fit_configure_window.isVisible()==True:
			self.fit_configure_window.hide()
		else:
			self.fit_configure_window.show()

	def callback_help(self):
		webbrowser.open('https://www.gpvdm.com/man/index.html')

	def callback_add_page(self,file_name):
		new_tab=fit_tab(file_name)
		self.notebook.addTab(new_tab,new_tab.tab_name)

	def remove_invalid(self,input_name):
		return input_name.replace (" ", "_")

	def callback_import(self):
		tab = self.notebook.currentWidget()
		tab.import_data()

	def callback_view_toggle_tab(self):
		print("add code")
		#self.toggle_tab_visible(data)

	def load_tabs(self):
		self.order_widget.load_tabs()

		self.fit_progress=fit_progress()
		self.notebook.addTab(self.fit_progress,"Fit progress")


	def add_page(self,index):
		new_tab=fit_tab(index)
		self.notebook.addTab(new_tab,new_tab.tab_name)


	def rod(self):
		tab = self.notebook.currentWidget()
		tab.update()

	def callback_one_fit(self):
		my_server=server_get()
		my_server.clear_cache()
		my_server.add_job(get_sim_path(),"--1fit")
		my_server.set_callback_when_done(self.rod)
		my_server.start()

	def callback_do_fit(self):
		my_server=server_get()
		my_server.clear_cache()
		my_server.add_job(get_sim_path(),"--fit")
		my_server.start()

	def __init__(self,name):
		QWidgetSavePos.__init__(self,name)

		self.main_vbox = QVBoxLayout()

		#self.setFixedSize(900, 700)
		self.setWindowTitle(_("Fit window")+" https://www.gpvdm.com")   
		self.setWindowIcon(icon_get("fit"))

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))
		toolbar.setToolButtonStyle( Qt.ToolButtonTextUnderIcon)


		self.order_widget=order_widget()
		self.order_widget.new_text=_("New fit")
		self.order_widget.delete_text=_("Delete fit")
		self.order_widget.clone_text=_("Clone fit")
		self.order_widget.rename_text=_("Rename fit")
		self.order_widget.new_dlg_text=_("New fit name:")
		self.order_widget.base_file_name=["fit","fit_data","fit_patch","fit_math"]
		self.order_widget.new_tab_name=_("fit ")
		self.order_widget.clone_dlg_text=_("Clone the current fit to a new fit called:")
		self.order_widget.rename_dlg_text=_("Rename the fit to be called:")
		self.order_widget.delete_dlg_text=_("Should I remove the fit file ")
		self.order_widget.name_token="#fit_name"
		self.order_widget.init()
 
		toolbar.addWidget(self.order_widget)

		self.order_widget.added.connect(self.callback_add_page)


		toolbar.addSeparator()

		self.tb_configure= QAction(icon_get("preferences-system"), wrap_text(_("Configure"),4), self)
		self.tb_configure.triggered.connect(self.callback_configure)
		toolbar.addAction(self.tb_configure)

		self.import_data= QAction(icon_get("import"), _("Import data"), self)
		self.import_data.triggered.connect(self.callback_import)
		toolbar.addAction(self.import_data)

		toolbar.addSeparator()

		self.play= QAction(icon_get("media-playback-start"), wrap_text(_("Run a single fit"),4), self)
		self.play.triggered.connect(self.callback_one_fit)
		toolbar.addAction(self.play)
		
		self.play= QAction(icon_get("forward"),wrap_text( _("Start the fitting process"),4), self)
		self.play.triggered.connect(self.callback_do_fit)
		toolbar.addAction(self.play)

		self.pause= QAction(icon_get("media-playback-pause"), wrap_text(_("Stop the simulation"),4), self)
		self.pause.triggered.connect(self.callback_stop)
		toolbar.addAction(self.pause)

		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		toolbar.addWidget(spacer)


		self.help = QAction(icon_get("help"), _("Help"), self)
		self.help.triggered.connect(self.callback_help)
		toolbar.addAction(self.help)

		self.main_vbox.addWidget(toolbar)

		self.notebook = QTabWidget()
		self.order_widget.notebook_pointer=self.notebook

		css_apply(self.notebook,"style_h.css")
		self.notebook.setTabBar(QHTabBar())
		self.notebook.setTabPosition(QTabWidget.West)


		self.notebook.setMovable(True)

		self.load_tabs()

		self.main_vbox.addWidget(self.notebook)
		
		self.status_bar=QStatusBar()
		self.main_vbox.addWidget(self.status_bar)

		self.setLayout(self.main_vbox)

		self.fit_configure_window=None
		




