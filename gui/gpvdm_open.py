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
from util import gpvdm_delete_file
#from global_objects import global_object_get
from plot_io import get_plot_file_info
from dat_file_class import dat_file

#qt
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import QSize, Qt, QTimer
from PyQt5.uic import loadUi
from PyQt5.QtWidgets import QMenu,QAbstractItemView,QApplication,QDialog,QGraphicsScene,QListWidgetItem,QPushButton,QListView,QVBoxLayout,QDialog,QWidget,QListWidget,QHBoxLayout,QLineEdit
from PyQt5.QtGui import QPixmap

#cal_path
from icon_lib import QIcon_load
from cal_path import get_ui_path

from help import help_window

from gui_util import error_dlg

from ref import get_ref_text
from gui_util import dlg_get_text
from gui_util import yes_no_dlg

from clone import clone_material
from clone import clone_spectra
from cal_path import get_base_material_path
from cal_path import get_base_spectra_path

from inp import inp_get_token_value
from util import isfiletype
from win_lin import desktop_open

COL_PATH = 0
COL_PIXBUF = 1
COL_IS_DIRECTORY = 2

import i18n
_ = i18n.language.gettext

#util
from util import latex_to_html

class gpvdm_open(QDialog):

	def __init__(self,path,show_inp_files=True):
		QWidget.__init__(self)
		self.menu_new_material_enabled=False
		self.menu_new_spectra_enabled=False
		self.show_inp_files=show_inp_files
		self.show_directories=True
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
		self.setWindowIcon(QIcon_load("folder"))
		self.listwidget=QListWidget()
		self.listwidget.setSelectionMode(QAbstractItemView.ExtendedSelection)
		self.listwidget.setStyleSheet("margin: 0; padding: 0; ")
		self.vbox.addWidget(self.top_h_widget)
		self.vbox.addWidget(self.listwidget)
	
		self.up.setFixedSize(42,42)
		self.up.setStyleSheet("margin: 0; padding: 0; border: none;")
		self.home.setFixedSize(42,42)
		self.home.setStyleSheet("margin: 0; padding: 0; border: none ;")
		#self.window.center()

		self.up.setIcon(QIcon_load("go-up"))
		self.up.clicked.connect(self.on_up_clicked)


		self.home.setIcon(QIcon_load("user-home"))
		self.home.clicked.connect(self.on_home_clicked)


		self.dir = path
		self.root_dir= path

		self.path.setText(path)

		self.dir_icon = QIcon_load("folder")
		self.dat_icon = self.get_icon("dat")
		self.inp_icon = QIcon_load("text-x-generic")
		self.xls_icon = QIcon_load("wps-office-xls")
		self.info_icon = self.get_icon("info")
		self.pdf_icon = QIcon_load("pdf")
		self.jpg_icon = QIcon_load("image-x-generic")
		self.spectra_icon = self.get_icon("spectra")
		self.mat_icon = QIcon_load("organic_material")

		self.listwidget.setIconSize(QSize(64,64))
		self.listwidget.setViewMode(QListView.IconMode)
		self.listwidget.setSpacing(8)
		self.listwidget.setWordWrap(True)
		gridsize=self.listwidget.size()
		gridsize.setWidth(80)
		gridsize.setHeight(80)

		self.listwidget.setGridSize(gridsize)

		self.fill_store()

		self.listwidget.itemDoubleClicked.connect(self.on_item_activated)
		self.listwidget.setContextMenuPolicy(Qt.CustomContextMenu)
		self.listwidget.itemSelectionChanged.connect(self.on_selection_changed)
		self.listwidget.customContextMenuRequested.connect(self.callback_menu)
		self.resizeEvent=self.resizeEvent
		self.show()

	def callback_menu(self,event):
		menu = QMenu(self)
		newmaterialAction=False
		newspectraAction=False
		newdirAction = menu.addAction(_("New directory"))
		if self.menu_new_material_enabled==True:
			newmaterialAction = menu.addAction(_("New material"))

		if self.menu_new_spectra_enabled==True:
			newspectraAction = menu.addAction(_("New spectra"))

		deleteAction = menu.addAction(_("Delete file"))
		renameAction = menu.addAction(_("Rename"))
		renameAction.setEnabled(False)
		deleteAction.setEnabled(False)
		if len(self.listwidget.selectedItems())==1:
			renameAction.setEnabled(True)
			
		if len(self.listwidget.selectedItems())>0:
			deleteAction.setEnabled(True)

		action = menu.exec_(self.mapToGlobal(event))

		if action == newdirAction:
			new_sim_name=dlg_get_text( _("New directory name:"), _("New directory"),"document-new")
			new_sim_name=new_sim_name.ret

			if new_sim_name!=None:
				name=os.path.join(self.dir,new_sim_name)
				os.mkdir(name)
		elif action == newmaterialAction:
			new_sim_name=dlg_get_text( _("New material name:"), _("New material name"),"organic_material")
			new_sim_name=new_sim_name.ret
			if new_sim_name!=None:
				new_material=os.path.join(self.dir,new_sim_name)
				clone_material(new_material,os.path.join(get_base_material_path(),"generic","generic_organic"))
		elif action == newspectraAction:
			new_sim_name=dlg_get_text( _("New spectra name:"), _("New spectra name"),"spectra_file")
			new_sim_name=new_sim_name.ret
			if new_sim_name!=None:
				new_material=os.path.join(self.dir,new_sim_name)
				clone_spectra(new_material,get_base_spectra_path())
		elif action == deleteAction:
			files=""
			for i in self.listwidget.selectedItems():
				files=files+os.path.join(self.dir,i.text())+"\n"
			ret=yes_no_dlg(self,_("Are you sure you want to delete the files ?")+"\n\n"+files)
			if ret==True:
				for i in self.listwidget.selectedItems():
					file_to_remove=os.path.join(self.dir,i.text())
					gpvdm_delete_file(file_to_remove)
		elif action == renameAction:
			old_name=self.listwidget.currentItem().text()
			new_sim_name=dlg_get_text( _("Rename:"), self.listwidget.currentItem().text(),"rename")
			new_sim_name=new_sim_name.ret

			if new_sim_name!=None:
				new_name=os.path.join(self.dir,new_sim_name)
				old_name=os.path.join(self.dir,old_name)
				print(old_name, new_name)
				os.rename(old_name, new_name)

		self.fill_store()

	def resizeEvent(self,resizeEvent):
		self.fill_store()
		#self.window.listwidget.setIconSize(QSize(48,48))

	def get_icon(self, name):
		return QIcon_load(name+"_file")


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
				gpvdm_file_type=inp_get_token_value(os.path.join(file_name,"mat.inp"), "#gpvdm_file_type")
				if gpvdm_file_type=="spectra":
					itm = QListWidgetItem( fl )
					itm.setIcon(self.spectra_icon)
					self.listwidget.addItem(itm)
				elif gpvdm_file_type=="mat":
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
					
				if (file_name.endswith(".omat")==True):
					itm = QListWidgetItem( fl )
					itm.setIcon(self.mat_icon)
					self.listwidget.addItem(itm)

				if file_name.endswith(".pdf")==True:
					itm = QListWidgetItem( fl )
					itm.setIcon(self.pdf_icon)
					self.listwidget.addItem(itm)

				if file_name.endswith(".jpg")==True:
					itm = QListWidgetItem( fl )
					itm.setIcon(self.jpg_icon)
					self.listwidget.addItem(itm)

				if os.path.basename(file_name)=="sim_info.dat":
					itm = QListWidgetItem( fl )
					itm.setIcon(self.info_icon)
					self.listwidget.addItem(itm)

	def on_home_clicked(self, widget):
		self.dir = self.root_dir
		self.change_path()


	def on_item_activated(self,item):
		full_path=os.path.join(self.dir, item.text())

		print(full_path,os.path.isfile(full_path))
		if os.path.isfile(full_path)==True:
			self.file_path=full_path
			if isfiletype(full_path,"xls")==True or isfiletype(full_path,"xlsx")==True:
				desktop_open(full_path)
				self.reject()
				return
			elif isfiletype(full_path,"jpg")==True:
				desktop_open(full_path)
				self.reject()
				return
			self.accept()
		else:
			if os.path.isfile(os.path.join(full_path,"mat.inp"))==True:
				self.file_path=full_path
				self.accept()
			else:
				self.dir = full_path
				self.change_path()

	def on_selection_changed(self):
		if len(self.listwidget.selectedItems())>0:
			item=self.listwidget.selectedItems()[0]
		else:
			return

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
					help_window().help_set_help(["organic_material",summary])
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

