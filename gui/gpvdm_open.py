#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2917 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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
#from global_objects import global_object_get
from plot_io import get_plot_file_info
from dat_file_class import dat_file

#qt
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import QSize, Qt, QTimer
from PyQt5.uic import loadUi
from PyQt5.QtWidgets import QApplication,QGraphicsScene,QListWidgetItem,QPushButton,QListView,QVBoxLayout,QDialog,QWidget,QListWidget,QHBoxLayout,QLineEdit
from PyQt5.QtGui import QPixmap

#cal_path
from cal_path import get_icon_path
from cal_path import get_ui_path

from help import help_window

from gui_util import error_dlg

from ref import get_ref_text

COL_PATH = 0
COL_PIXBUF = 1
COL_IS_DIRECTORY = 2

import i18n
_ = i18n.language.gettext

#util
from util import latex_to_html

class gpvdm_open(QDialog):
	show_inp_files=True
	show_directories=True

	def __init__(self,path):
		QWidget.__init__(self)
		self.file_path=""
		self.vbox=QVBoxLayout()
		self.setLayout(self.vbox)
		self.top_h_widget=QWidget()
		self.top_h_widget.setStyleSheet("margin: 0; padding: 0; ")
		self.top_hbox=QHBoxLayout()
		self.top_h_widget.setLayout(self.top_hbox)
		self.top_h_widget.setMaximumHeight(50)
		self.up=QPushButton()
		self.home=QPushButton()	
		self.resize(800,500)
		self.path=QLineEdit()
		self.path.setMinimumHeight(30)
		self.path.setStyleSheet("padding: 0; ")
		self.top_hbox.addWidget(self.up)
		self.top_hbox.addWidget(self.home)
		self.top_hbox.addWidget(self.path)
		self.setWindowTitle(_("Open file")+" https://www.gpvdm.com")
		self.setWindowIcon(QIcon(get_icon_path("folder")))
		self.listwidget=QListWidget()
		self.listwidget.setStyleSheet("margin: 0; padding: 0; ")
		self.vbox.addWidget(self.top_h_widget)
		self.vbox.addWidget(self.listwidget)
	
		self.up.setFixedSize(42,42)
		self.up.setStyleSheet("margin: 0; padding: 0; border: none;")
		self.home.setFixedSize(42,42)
		self.home.setStyleSheet("margin: 0; padding: 0; border: none ;")
		#self.window.center()

		icon = QPixmap(get_icon_path("go-up"))
		self.up.setIcon(QIcon(icon))
		self.up.clicked.connect(self.on_up_clicked)


		icon = QPixmap(get_icon_path("user-home"))
		self.home.setIcon(QIcon(icon))
		self.home.clicked.connect(self.on_home_clicked)


		self.dir = path
		self.root_dir= path

		self.path.setText(path)

		self.dir_icon = QIcon(QPixmap(get_icon_path("folder")))
		self.dat_icon = self.get_icon("dat")
		self.inp_icon = self.get_icon("inp")
		self.xls_icon = self.get_icon("xls")
		self.info_icon = self.get_icon("info")
		self.spectra_icon = self.get_icon("spectra")
		self.mat_icon = QIcon(QPixmap(get_icon_path("organic_material")))

		self.listwidget.setIconSize(QSize(48,48))
		self.listwidget.setViewMode(QListView.IconMode)
		self.listwidget.setSpacing(8)
		self.listwidget.setWordWrap(True)
		gridsize=self.listwidget.size()
		gridsize.setWidth(80)
		gridsize.setHeight(80)

		self.listwidget.setGridSize(gridsize)

		self.fill_store()

		self.listwidget.itemDoubleClicked.connect(self.on_item_activated)
		self.listwidget.itemClicked.connect(self.on_selection_changed)
		self.listwidget.setIconSize(QSize(48,48))
		self.resizeEvent=self.resizeEvent
		self.show()

	def resizeEvent(self,resizeEvent):
		self.fill_store()
		#self.window.listwidget.setIconSize(QSize(48,48))

	def get_icon(self, name):
		return QIcon(QPixmap(get_icon_path(name+"_file")))


	def get_filename(self):
		return self.file_path

	def fill_store(self):
		self.listwidget.clear()
		if os.path.isdir(self.dir)==False:
			error_dlg(self,_("The directory is gone, so I can't open it.  Did you delete it?")+" "+self.dir)
			return

		all_files=os.listdir(self.dir)
		all_files.sort()
		for fl in all_files:
			file_name=os.path.join(self.dir, fl)
			if os.path.isdir(file_name):
				if os.path.isfile(os.path.join(file_name,"mat.inp")):
					itm = QListWidgetItem( fl )
					itm.setIcon(self.mat_icon)
					self.listwidget.addItem(itm)
				else:
					show_dir=True

					#if fl=="materials":
					#	show_dir=False

					if os.path.isfile(os.path.join(file_name,"gpvdm_gui_config.inp"))==True:
						show_dir=False

					if show_dir==True:
						itm = QListWidgetItem( fl )
						itm.setIcon(self.dir_icon)
						self.listwidget.addItem(itm)

			else:
				#append=False
				if (file_name.endswith(".dat")==True):
					f = open(file_name, 'rb')
					text = f.readline()
					f.close()
					#text=text.encode('utf-8').strip()
					#print(text)
					#text=text.rstrip()
					if len(text)>0:
						if text[len(text)-1]==10:
							text=text[:-1]

					if text==b"#gpvdm":
						itm = QListWidgetItem( fl )
						itm.setIcon(self.dat_icon)
						self.listwidget.addItem(itm)

				if (file_name.endswith(".inp")==True) and self.show_inp_files==True:
					itm = QListWidgetItem( fl )
					itm.setIcon(self.inp_icon)
					self.listwidget.addItem(itm)

				if (file_name.endswith(".spectra")==True):
					itm = QListWidgetItem( fl )
					itm.setIcon(self.spectra_icon)
					self.listwidget.addItem(itm)
					
				if (file_name.endswith(".omat")==True):
					itm = QListWidgetItem( fl )
					itm.setIcon(self.mat_icon)
					self.listwidget.addItem(itm)

				if file_name.endswith(".xlsx")==True or file_name.endswith(".xls")==True:
					itm = QListWidgetItem( fl )
					itm.setIcon(self.xls_icon)
					self.listwidget.addItem(itm)

				if os.path.basename(file_name)=="sim_info.dat":
					itm = QListWidgetItem( fl )
					itm.setIcon(self.info_icon)
					self.listwidget.addItem(itm)

	def on_home_clicked(self, widget):
		self.dir = self.root_dir
		self.fill_store()


	def on_item_activated(self,item):
		full_path=os.path.join(self.dir, item.text())

		print(full_path,os.path.isfile(full_path))
		if os.path.isfile(full_path)==True:
			self.file_path=full_path
			self.accept()
		else:
			if os.path.isfile(os.path.join(full_path,"mat.inp"))==True:
				self.file_path=full_path
				self.accept()
			else:
				self.dir = full_path
				self.change_path()

	def on_selection_changed(self,item):
		if type(item)!=None:
			file_name=item.text()

			full_path=os.path.join(self.dir, file_name)

			if (file_name.endswith(".dat")==True):
				state=dat_file()
				get_plot_file_info(state,full_path)
				summary="<big><b>"+file_name+"</b></big><br><br>"+_("title")+": "+state.title+"<br>"+_("x axis")+": "+state.x_label+" ("+latex_to_html(state.x_units)+")<br>"+_("y axis")+": "+state.data_label+" ("+latex_to_html(state.data_units)+")<br><br><big><b>"+_("Double click to open")+"</b></big>"
				help_window().help_set_help(["dat_file.png",summary])

			if file_name.endswith("equilibrium"):
				state=dat_file()
				get_plot_file_info(state,full_path)
				summary="<big><b>"+_("equilibrium")+"</b></big><br><br>"+_("This contains the simulation output at 0V in the dark.")
				help_window().help_set_help(["folder.png",summary])

			if os.path.isdir(full_path)==True:
				if os.path.isfile(os.path.join(full_path,"mat.inp")):
					summary="<b><big>"+file_name+"</b></big><br>"
					ref_path=os.path.join(full_path,"n.ref")
					ref=get_ref_text(ref_path)
					if ref!=None:
						summary=summary+ref
					help_window().help_set_help(["32_organic_material.png",summary])
					#get_ref_text(file_name,html=True)


	def change_path(self):
		self.path.setText(self.dir)

		self.fill_store()
		sensitive = True
		#print(self.dir,self.root_dir)
		if self.dir == self.root_dir:
			sensitive = False

		self.up.setEnabled(sensitive)

	def on_up_clicked(self, widget):
		self.dir = os.path.dirname(self.dir)
		self.change_path()

