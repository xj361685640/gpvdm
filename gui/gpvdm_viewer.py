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

## @package gpvdm_viewer
#  A directory/file browser for gpvdm.
#


import os
from util import gpvdm_delete_file
from plot_io import get_plot_file_info
from dat_file_class import dat_file

#qt
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import QSize, Qt, QTimer
from PyQt5.uic import loadUi
from PyQt5.QtWidgets import QMenu,QAbstractItemView,QListWidgetItem,QPushButton,QListView,QWidget,QListWidget,QAction
from PyQt5.QtGui import QPixmap
from PyQt5.QtCore import pyqtSignal

#cal_path
from icon_lib import icon_get
from cal_path import get_ui_path

from help import help_window

from error_dlg import error_dlg

from ref import get_ref_text
from gui_util import dlg_get_text
from gui_util import yes_no_dlg


from clone import clone_material
from clone import clone_spectra
from cal_path import get_base_material_path
from cal_path import get_base_spectra_path

from inp import inp_get_token_value
from inp import inp_load_file
from inp import inp_get_token_value_from_list

from util import isfiletype
from win_lin import desktop_open

from util import str2bool

from plot_gen import plot_gen

from util_zip import zip_lsdir
from util_zip import read_lines_from_archive

import webbrowser
from info import sim_info

from materials_io import is_mat_file

from cal_path import get_inp_file_path
import psutil

from icon_lib import icon_get

from cal_path import get_home_path
from cal_path import get_desktop_path
from cal_path import get_videos_path
from cal_path import get_downloads_path
from cal_path import get_sim_path

from config_window import class_config_window
from cluster_config_window import cluster_config_window
from win_lin import running_on_linux

COL_PATH = 0
COL_PIXBUF = 1
COL_IS_DIRECTORY = 2

import i18n
_ = i18n.language.gettext

#util
from util import latex_to_html

class file_store():
	def __init__(self):
		self.file_name=""
		self.display_name=""
		self.icon=""
		self.hidden=False
		self.allow_navigation=False

class gpvdm_viewer(QListWidget):

	accept = pyqtSignal()
	reject = pyqtSignal()
	path_changed = pyqtSignal()

	def dragEnterEvent(self, event):
		#self.setText("<drop content>")
	#	print("c")
		#self.setBackgroundRole(QtGui.QPalette.Highlight)
		event.acceptProposedAction()
		#self.changed.emit(event.mimeData())

	def dragMoveEvent(self, event):
		#print("b")
		event.acceptProposedAction()

	def dropEvent(self, event):
		mimeData = event.mimeData()

		if mimeData.hasUrls():
			a=[url.path() for url in mimeData.urls()]
			print("d",a)

#		self.setBackgroundRole(QtGui.QPalette.Dark)
		event.acceptProposedAction()


	def dropMimeData(self, data, action, row, column, parent):
		print()
#		print(data)

	def __init__(self,path,show_inp_files=True,open_own_files=True):
		QWidget.__init__(self)
		self.setAcceptDrops(True)
		self.setDragEnabled(True)
		self.setDragDropMode(QAbstractItemView.DragDrop)
		self.open_own_files=open_own_files
		self.file_list=[]
		self.menu_new_material_enabled=False
		self.menu_new_spectra_enabled=False
		self.show_inp_files=show_inp_files
		self.show_directories=True
		self.file_path=""
		self.show_back_arrow=False

		self.setStyleSheet("margin: 0; padding: 0; ")

		self.show_hidden=False
		self.enable_menu=True
		self.path=""
		self.allow_navigation=False

		self.set_path(path)
		self.root_dir= self.path
		
	

		self.setIconSize(QSize(64,64))

		self.fill_store()

		self.itemDoubleClicked.connect(self.on_item_activated)
		self.setContextMenuPolicy(Qt.CustomContextMenu)
		self.itemSelectionChanged.connect(self.on_selection_changed)
		self.customContextMenuRequested.connect(self.callback_menu)
		self.resizeEvent=self.resizeEvent
		self.show()

	def set_back_arrow(self,data):
		self.show_back_arrow=data

	def set_show_hidden(self,data):
		self.show_hidden=data

	def set_multi_select(self):
		self.setSelectionMode(QAbstractItemView.ExtendedSelection)

	def set_enable_menu(self,data):
		self.enable_menu=data

	def set_directory_view(self,data):
		if data==True:
			self.setViewMode(QListView.IconMode)
			self.setSpacing(8)
			self.setWordWrap(True)
			self.setTextElideMode ( Qt.ElideNone)
			gridsize=self.size()
			gridsize.setWidth(100)
			gridsize.setHeight(90)

			self.setGridSize(gridsize)

	def callback_menu(self,event):
		if self.enable_menu==False:
			return
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
		if len(self.selectedItems())==1:
			renameAction.setEnabled(True)

		if len(self.selectedItems())>0:
			deleteAction.setEnabled(True)

		action = menu.exec_(self.mapToGlobal(event))

		if action == newdirAction:
			new_sim_name=dlg_get_text( _("New directory name:"), _("New directory"),"document-new")
			new_sim_name=new_sim_name.ret

			if new_sim_name!=None:
				name=os.path.join(self.path,new_sim_name)
				os.mkdir(name)
		elif action == newmaterialAction:
			new_sim_name=dlg_get_text( _("New material name:"), _("New material name"),"organic_material")
			new_sim_name=new_sim_name.ret
			if new_sim_name!=None:
				new_material=os.path.join(self.path,new_sim_name)
				clone_material(new_material,os.path.join(get_base_material_path(),"generic","generic_organic"))
		elif action == newspectraAction:
			new_sim_name=dlg_get_text( _("New spectra name:"), _("New spectra name"),"spectra_file")
			new_sim_name=new_sim_name.ret
			if new_sim_name!=None:
				new_material=os.path.join(self.path,new_sim_name)
				clone_spectra(new_material,get_base_spectra_path())
		elif action == deleteAction:
			files=""
			for i in self.selectedItems():
				files=files+os.path.join(self.path,i.text())+"\n"
			ret=yes_no_dlg(self,_("Are you sure you want to delete the files ?")+"\n\n"+files)
			if ret==True:
				for i in self.selectedItems():
					file_to_remove=os.path.join(self.path,i.text())
					gpvdm_delete_file(file_to_remove)
		elif action == renameAction:
			old_name=self.currentItem().text()
			new_sim_name=dlg_get_text( _("Rename:"), self.currentItem().text(),"rename")
			new_sim_name=new_sim_name.ret

			if new_sim_name!=None:
				new_name=os.path.join(self.path,new_sim_name)
				old_name=os.path.join(self.path,old_name)
				#print(old_name, new_name)
				os.rename(old_name, new_name)

		self.fill_store()

	def resizeEvent(self,resizeEvent):
		self.fill_store()

	def get_icon(self, name):
		return icon_get(name+"_file")

	def get_filename(self):
		return self.file_path


	def set_path(self,path):
		self.path=path		
		self.path_changed.emit()

	def add_back_arrow(self):
		if self.show_back_arrow==True:
			if self.path==self.root_dir and self.allow_navigation==False:
				return

			if self.path!="/gpvdmroot":
				itm = QListWidgetItem( ".." )
				itm.setIcon(icon_get('go-previous'))
				self.addItem(itm)

	def fill_store(self):


		#partitions = 

		#for p in partitions:
		#	print(p.mountpoint, psutil.disk_usage(p.mountpoint).percent)
		#print(expanduser("~"))

		self.file_list=[]

		#print(self.path)
		if self.path=="/gpvdmroot":
			itm=file_store()
			itm.file_name="simulation_dir"
			itm.icon="si"
			itm.display_name=_("Simulation")
			self.file_list.append(itm)

			itm=file_store()
			itm.file_name="home_dir"
			itm.icon="user-home"
			itm.display_name=_("Home")
			self.file_list.append(itm)

			if get_desktop_path()!=False:
				itm=file_store()
				itm.file_name="desktop_dir"
				itm.icon="desktop"
				itm.display_name=_("Desktop")
				self.file_list.append(itm)

			if get_downloads_path()!=False:
				itm=file_store()
				itm.file_name="downloads_dir"
				itm.icon="folder-download"
				itm.display_name=_("Downloads")
				self.file_list.append(itm)

			itm=file_store()
			itm.file_name="gpvdm_configure"
			itm.icon="cog"
			itm.display_name=_("Configure")
			self.file_list.append(itm)

			for p in psutil.disk_partitions():
				name=p.mountpoint
				if running_on_linux()==True:
					name=os.path.basename(name)

				if name=="":
					name="/"
				itm=file_store()
				itm.file_name="mount_point::::"+p.mountpoint
				itm.icon="drive-harddisk"
				itm.display_name=name
				self.file_list.append(itm)
		elif self.path=="/gpvdmroot/gpvdm_configure":
			itm=file_store()
			itm.file_name="gpvdm_cluster_config"
			itm.icon="server"
			itm.display_name=_("Cluster")
			self.file_list.append(itm)

			itm=file_store()
			itm.file_name="gpvdm_language_config"
			itm.icon="internet-chat"
			itm.display_name=_("Language")
			self.file_list.append(itm)

			itm=file_store()
			itm.file_name="gpvdm_solver_config"
			itm.icon="accessories-calculator"
			itm.display_name=_("Solver")
			self.file_list.append(itm)

			itm=file_store()
			itm.file_name="gpvdm_led_config"
			itm.icon="oled"
			itm.display_name=_("LED")
			self.file_list.append(itm)

			itm=file_store()
			itm.file_name="gpvdm_dump_config"
			itm.icon="hdd_custom"
			itm.display_name=_("Output files")
			self.file_list.append(itm)

			itm=file_store()
			itm.file_name="gpvdm_gui_config"
			itm.icon="applications-interfacedesign"
			itm.display_name=_("GUI configuration")
			self.file_list.append(itm)

			itm=file_store()
			itm.file_name="gpvdm_thermal_config"
			itm.icon="thermal"
			itm.display_name=_("Thermal")
			self.file_list.append(itm)

			itm=file_store()
			itm.file_name="gpvdm_server_config"
			itm.icon="cpu"
			itm.display_name=_("Server")
			self.file_list.append(itm)


		else:
			path=self.path

			if os.path.isdir(path)==False:
				error_dlg(self,_("The directory is gone, so I can't open it.  Did you delete it?")+" "+path)
				return
			else:
				all_files=os.listdir(path)
				all_files.sort()

				for fl in all_files:
					#print(fl)
					file_name=os.path.join(path, fl)
					itm=file_store()

					if is_mat_file(file_name)==True:
						if fl.endswith(".zip"):
							fl=fl[:-4]
						itm.file_name=fl
						itm.icon="organic_material"

					#if it is a directory
					if os.path.isdir(file_name):
						gpvdm_file_type=inp_get_token_value(os.path.join(file_name,"mat.inp"), "#gpvdm_file_type")
						if gpvdm_file_type=="spectra":
							itm.file_name=fl
							itm.icon="spectra"

						else:
							show_dir=True

							if os.path.isfile(os.path.join(file_name,"gpvdm_gui_config.inp"))==True:
								show_dir=False

							if show_dir==True:
								itm.file_name=fl
								itm.icon="folder"


					else:
						#append=False
						ext=os.path.splitext(file_name)
						if len(ext)>1:
							ext=ext[1].lower()
						else:
							ext=""

						if (ext==".dat"):
							read_ok=False
							try:
								f = open(file_name, 'rb')
								text = f.readline()
								f.close()
								read_ok=True
							except:
								pass

							if read_ok==True:
								if len(text)>0:
									if text[len(text)-1]==10:
										text=text[:-1]

								if text==b"#gpvdm":
									itm.file_name=fl
									itm.icon="dat_file"

						elif (ext==".inp") and self.show_inp_files==True:
							itm.file_name=fl
							itm.icon="text-x-generic"
					
						elif (ext==".omat"):
							itm.file_name=fl
							itm.icon="omat"

						elif os.path.basename(file_name)=="sim_info.dat":
							itm.file_name=fl
							itm.icon="info"

						elif file_name.endswith("default.gpvdm")==False and file_name.endswith(".gpvdm"):
							lines=[]
							lines=inp_load_file("info.inp",archive=file_name)
							if lines!=False:

								itm.file_name=fl
								itm.display_name=inp_get_token_value_from_list(lines, "#info_name")+" ("+fl+")"
								icon_name=inp_get_token_value_from_list(lines, "#info_icon")
								itm.icon=icon_name
								itm.hidden=str2bool(inp_get_token_value_from_list(lines, "#info_hidden"))

								a=zip_lsdir(file_name,sub_dir="fs/") #,zf=None,sub_dir=None
								if len(a)!=0:
									for fname in a:
										lines=ret=read_lines_from_archive(file_name,"fs/"+fname)
										if lines!=False:
											web_link=inp_get_token_value_from_list(lines, "#web_link")
											name=inp_get_token_value_from_list(lines, "#name")
											sub_itm=file_store()
											sub_itm.icon="internet-web-browser"
											sub_itm.display_name=name
											sub_itm.file_name=web_link
											sub_itm.hidden=False
											self.file_list.append(sub_itm)

						if itm.icon=="":
							if icon_get(ext)!=False:	
								itm.icon=ext
								itm.file_name=fl
							else:
								itm.icon="misc"
								itm.file_name=fl

					if itm.display_name=="":
						itm.display_name=itm.file_name
					if file_name.endswith("sim.gpvdm")==False:
						self.file_list.append(itm)

			for i in range(0,len(self.file_list)):
				if self.file_list[i].file_name=="p3htpcbm.gpvdm":
					self.file_list.insert(0, self.file_list.pop(i))
					break

		self.paint()

	def paint(self):
		self.clear()

		self.add_back_arrow()

		for i in range(0,len(self.file_list)):
			draw=True
			if self.file_list[i].file_name=="":
				draw=False
			
			if self.file_list[i].hidden==True and self.show_hidden==False:
				draw=False
			
			if draw==True:
				itm = QListWidgetItem( self.file_list[i].display_name )
				a=icon_get(self.file_list[i].icon)
				#print(self.file_list[i].icon)
				itm.setIcon(a)

				#if self.file_list[i].display_name=="data.xlsx":
				#	print(itm.icon,a,self.file_list[i].icon)
				self.addItem(itm)

	def decode_name(self,text):
		fname=""
		for i in range(0,len(self.file_list)):
			if self.file_list[i].display_name==text:
				fname=self.file_list[i].file_name
				return fname

	def on_item_activated(self,item):
		text=item.text()
		if text=="..":
			if self.path==self.root_dir:
				self.set_path("/gpvdmroot")
			else:
				old_path=self.path
				self.set_path(os.path.dirname(self.path))
				print(self.path,old_path,os.path.dirname(self.path))
				if old_path==self.path:
					self.set_path("/gpvdmroot")
			self.fill_store()
			return

		decode=self.decode_name(text)
		if decode.startswith("http"):
			webbrowser.open(decode)
			return
		elif decode=="home_dir":
			self.set_path(get_home_path())
			self.fill_store()
			return
		elif decode=="desktop_dir":
			self.set_path(get_desktop_path())
			self.fill_store()
			return
		elif decode=="gpvdm_configure":
			self.set_path("/gpvdmroot/gpvdm_configure")
			self.fill_store()
			return

		elif decode=="music_dir":
			self.set_path(get_music_path())
			self.fill_store()
			return
		elif decode=="downloads_dir":
			self.set_path(get_downloads_path())
			self.fill_store()
			return
		elif decode=="simulation_dir":
			self.set_path(get_sim_path())
			self.fill_store()
			return			
		elif decode.startswith("mount_point")==True:
			point=decode.split("::::")
			self.set_path(point[1])
			self.fill_store()
			return
		elif decode=="gpvdm_cluster_config":
			self.win=cluster_config_window(self)
			self.win.show()
			return
		elif decode=="gpvdm_language_config":
			self.config_window=class_config_window()

			from tab_lang import language_tab_class

			self.config_window.files=[ ]
			self.config_window.description=[]
			self.config_window.init()
			lang_tab=language_tab_class()
			self.config_window.notebook.addTab(lang_tab,_("Language"))
			self.config_window.show()

			return
		elif decode=="gpvdm_solver_config":
			self.config_window=class_config_window()
			self.config_window.files=["math.inp"]
			self.config_window.description=[_("Solver configuration")]
			self.config_window.init()
			self.config_window.show()

			return
		elif decode=="gpvdm_led_config":
			self.config_window=class_config_window()
			self.config_window.files=["led.inp"]
			self.config_window.description=[_("LED")]
			self.config_window.init()
			self.config_window.show()
			return
		elif decode=="gpvdm_dump_config":
			self.config_window=class_config_window()
			self.config_window.files=["dump.inp"]
			self.config_window.description=[_("Output files")]
			self.config_window.init()
			self.config_window.show()
			return
		elif decode=="gpvdm_gui_config":
			self.config_window=class_config_window()
			self.config_window.files=["config.inp"]
			self.config_window.description=[_("GUI configuration"),]
			self.config_window.init()
			self.config_window.show()
			return
		elif decode=="gpvdm_thermal_config":
			self.config_window=class_config_window()
			self.config_window.files=["thermal.inp"]
			self.config_window.description=[_("Thermal")]
			self.config_window.init()
			self.config_window.show()
			return
		elif decode=="gpvdm_server_config":
			self.config_window=class_config_window()
			self.config_window.files=["server.inp"]
			self.config_window.description=[_("Server configuration")]
			self.config_window.init()
			self.config_window.show()
			return


		full_path=os.path.join(self.path,decode)
		if is_mat_file(full_path)==True:
			print("mat file!!")
			from materials_main import materials_main
			self.mat_window=materials_main(full_path)
			self.mat_window.show()
			return


		if os.path.isfile(full_path)==True:
			self.file_path=full_path
			if self.open_own_files==True:
				if os.path.basename(full_path)=="sim_info.dat":
					self.sim_info_window=sim_info(full_path)
					self.sim_info_window.show()
					self.reject.emit()
					return
				elif isfiletype(full_path,"dat")==True:
					plot_gen([full_path],[],"auto")
					self.reject.emit()
					return

				else:
					desktop_open(full_path)
					self.reject.emit()
					return

#				else:
#					self.accept.emit()	
			else:
				self.accept.emit()
				return
		else:
			if os.path.isfile(os.path.join(full_path,"mat.inp"))==True:
				self.file_path=full_path

				gpvdm_file_type=inp_get_token_value(os.path.join(full_path,"mat.inp"), "#gpvdm_file_type")
				if gpvdm_file_type=="spectra":
					from spectra_main import spectra_main
					self.mat_window=spectra_main(full_path)
					self.mat_window.show()

				self.accept.emit()
			else:
				self.set_path(full_path)
				#self.path = full_path
				self.fill_store()

	def on_selection_changed(self):

		if len(self.selectedItems())>0:
			item=self.selectedItems()[0]

			if type(item)!=None:
				file_name=self.decode_name(item.text())
				if file_name==None:
					return
				
				self.file_path=os.path.join(self.path, file_name)
	
			full_path=self.file_path

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

			#if os.path.isdir(full_path)==True:

			if is_mat_file(full_path)==True:

				summary="<b><big>"+file_name+"</b></big><br>"
				ref_path=os.path.join(full_path,"n.ref")
				ref=get_ref_text(ref_path)
				if ref!=None:
					summary=summary+ref
				help_window().help_set_help(["organic_material",summary])
				#get_ref_text(file_name,html=True)

