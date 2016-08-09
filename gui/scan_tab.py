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





import gc
import os
from inp import inp_get_token_value
from plot import check_info_file
from used_files_menu import used_files_menu
from plot_gen import plot_gen
from plot_state import plot_state
from cmp_class import cmp_class
from config import config
from token_lib import tokens


from scan_plot import scan_gen_plot_data
from server import server_find_simulations_to_run

from plot_io import plot_save_oplot_file
from gpvdm_open import gpvdm_open
from cal_path import get_exe_command
from help import my_help_class
from cal_path import get_image_file_path

from util import str2bool

#scan_io
from scan_io import scan_clean_dir
from scan_io import scan_clean_unconverged
from scan_io import scan_clean_simulation_output
from scan_io import scan_nested_simulation
from scan_io import scan_push_to_hpc
from scan_io import scan_import_from_hpc
from scan_io import scan_plot_fits

#scan_tree
from scan_tree import tree_gen
from scan_tree import tree_load_program
from scan_tree import tree_load_model
from scan_tree import tree_load_config
from scan_tree import tree_load_flat_list
from scan_tree import tree_save_flat_list

#scan_item
from scan_item import scan_items_get_file
from scan_item import scan_items_get_token
from scan_item import scan_items_get_list
from scan_item import scan_item_save


#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QMenuBar,QStatusBar, QMenu, QTableWidget, QAbstractItemView
from PyQt5.QtGui import QPainter,QIcon,QCursor

#window
#from plot_dlg import plot_dlg_class
#from scan_select import select_param
#from notes import notes
from gui_util import tab_add
from gui_util import tab_move_down
from gui_util import tab_remove
from gui_util import tab_get_value
from gui_util import error_dlg
from gui_util import yes_no_dlg

import i18n
_ = i18n.language.gettext

class scan_vbox(QWidget):


	def rename(self,new_name):
		self.sim_name=os.path.basename(new_name)
		self.sim_dir=new_name

		self.status_bar.showMessage(self.sim_dir)
		self.load()
		#self.plotted_graphs.init(self.sim_dir,self.callback_last_menu_click)

	def callback_notes(self, widget, data=None):

		note=notes()
		note.init(self.sim_dir)
		note.show()

	def callback_move_down(self):
		tab_move_down(self.tab)


	def callback_insert_command(self, widget, data=None):
		a=tab.selectionModel().selectedRows()

		if len(a)>0:
			a=a[0].row()
			tab_set_value(self.tab,a,1,"ret=str(round(random.uniform(1.0, 9.9),2))+\"e-\"+str(randint(1, 9))")
			tab_set_value(self.tab,a,2,"python_code")


	def add_line(self,data):
		my_help_class.help_set_help(["forward.png",_("<big><b>The scan window</b></big><br> Now using the drop down menu in the prameter to change 'column', select the device parameter you wish to vary, an example may be dos0/Electron Mobility. Now enter the values you would like it to scan oveer in the  'Values', an example could be '1e-3 1e-4 1e-5 1e-6'.  And hit the double arrorw to run the simulation.")])

		tab_add(tab,data)

		self.save_combo()
		self.rebuild_liststore_op_type()

	def callback_add_item(self, widget, data=None):
		self.add_line(["File","token",_("Select parameter"), "0.0 0.0", "scan",True])


	def callback_copy_item(self, widget, data=None):
		selection = self.treeview.get_selection()
		model, pathlist = selection.get_selected_rows()
		build=""
		for path in pathlist:
			tree_iter = model.get_iter(path)
			print "path=",tree_iter
			build=build+model.get_value(tree_iter,0)+","+model.get_value(tree_iter,1)+","+model.get_value(tree_iter,2)+","+str(model.get_value(tree_iter,3))+","+str(model.get_value(tree_iter,4))+","+str(model.get_value(tree_iter,5))+"\n"
			print build
		build=build[:-1]
		self.clipboard.set_text(build, -1)
		#tab_get_value(tab,y,x)

	def callback_paste_item(self, widget, data=None):
		text = self.clipboard.wait_for_text()
		if text != None:
			lines=text.rstrip().split('\n')
			for line in lines:
				array=line.rstrip().split(',')
				array[5]=str2bool(array[5])
				print array
				self.add_line(array)

	def callback_show_list(self, widget, data=None):
		self.select_param_window.update()
		self.select_param_window.show()

	def callback_delete_item(self):
		tab_remove(self.tab)
		self.save_combo()
		self.rebuild_liststore_op_type()

	def plot_results(self,plot_token):
		plot_token.key_units=self.get_units()

		plot_files, plot_labels, config_file = scan_gen_plot_data(plot_token,self.sim_dir)
		print plot_files, plot_labels
		plot_save_oplot_file(config_file,plot_token)
		plot_gen(plot_files,plot_labels,config_file)
		self.plot_open.set_sensitive(True)

		self.last_plot_data=plot_token
		return

	def get_units(self):
		token=""
		for i in range(0,self.tab.rowCount()):
			if self.liststore_combobox[i][2]=="scan":
				for ii in range(0,len(self.param_list)):
					if self.liststore_combobox[i][0]==self.param_list[ii].name:
						token=self.param_list[ii].token
				break
		if token!="":
			found_token=self.tokens.find(token)
			if type(found_token)!=bool:
				return found_token.units

		return ""


	def make_sim_dir(self):
		if os.path.isdir(self.sim_dir)==False:
			os.makedirs(self.sim_dir)


	def stop_simulation(self):
		self.myserver.killall()

	def clean_scan_dir(self):
		scan_clean_dir(self.sim_dir)

	def scan_clean_unconverged(self):
		scan_clean_unconverged(self.sim_dir)

	def scan_clean_simulation_output(self):
		scan_clean_simulation_output(self.sim_dir)

	def import_from_hpc(self):
		scan_import_from_hpc(self.sim_dir)

	def plot_fits(self):
		scan_plot_fits(self.sim_dir)

	def push_to_hpc(self):
		scan_push_to_hpc(self.sim_dir,False)

	def push_unconverged_to_hpc(self):
		scan_push_to_hpc(self.sim_dir,True)

	def nested_simulation(self):
		commands=scan_nested_simulation(self.sim_dir,os.path.join(os.path.expanduser('~'),"juan/hpc/final_graphs/orig/probe"))
		self.send_commands_to_server(commands,"")

	def simulate(self,run_simulation,generate_simulations,args):

		base_dir=os.getcwd()
		run=True

		if self.tab.rowCount() == 0:
			error_dlg(self,_("You have not selected any parameters to scan through.  Use the add button."))
			return


		if self.sim_name=="":
			error_dlg(self,_("No sim dir name"))
			return

		self.make_sim_dir()
		if generate_simulations==True:
			scan_clean_dir(self.sim_dir)


		for i in range(0,self.tab.rowCount()):
			found=False
			for ii in range(0,len(self.liststore_op_type)):
				if self.liststore_combobox[i][4]==self.liststore_op_type[ii][0]:
					found=True
			if found==False:
				run=False
				error_dlg(self,self.liststore_combobox[i][4]+_("Not valid"))
				break



		if run==True:
			print "Running"
			program_list=[]
			for i in range(0,self.tab.rowCount()):
				program_list.append([tab_get_value(self.tab,i,0),tab_get_value(self.tab,i,1),tab_get_value(self.tab,i,3),tab_get_value(self.tab,i,4)])

			print program_list
			tree_load_config(self.sim_dir)
			if generate_simulations==True:
				flat_simulation_list=[]
				if tree_gen(flat_simulation_list,program_list,base_dir,self.sim_dir)==False:
					error_dlg(self,_("Problem generating tree."))
					return

				print "flat list",flat_simulation_list
				tree_save_flat_list(self.sim_dir,flat_simulation_list)

			commands=tree_load_flat_list(self.sim_dir)
			print "loaded commands",commands
			if run_simulation==True:
				self.send_commands_to_server(commands,args)

		self.save_combo()
		os.chdir(base_dir)
		gc.collect()

	def send_commands_to_server(self,commands,args):
#		self.myserver.init(self.sim_dir)

		for i in range(0, len(commands)):
			self.myserver.add_job(commands[i],args)
			print "Adding job"+commands[i]

		self.myserver.start()

	def callback_plot_results(self, widget, data=None):
		self.plot_results(self.last_plot_data)

	def callback_last_menu_click(self, widget, data):
		print "here one!"
		self.plot_results(data)

	def callback_reopen_xy_window(self, widget, data=None):

		if len(self.plotted_graphs)>0:
			pos=len(self.plotted_graphs)-1
			plot_data=plot_state()
			plot_data.file0=self.plotted_graphs[pos].file0
			plot_xy_window=plot_dlg_class(gtk.WINDOW_TOPLEVEL)
			plot_xy_window.my_init(plot_data)
			plot_now=plot_xy_window.my_run(plot_data)

			if plot_now==True:
				self.plot_results(plot_data)
				self.plotted_graphs.refresh()

	def callback_gen_plot_command(self):
		dialog=gpvdm_open(os.getcwd())
		ret=dialog.window.exec_()

		if ret==QDialog.Accepted:
			full_file_name=dialog.get_filename()
			dialog.destroy()
			#print cur_dir=os.getcwd()
			#print full_file_name
			file_name=os.path.basename(full_file_name)

			plot_data=plot_state()
			plot_data.path=self.sim_dir
			plot_data.example_file0=full_file_name
			plot_data.example_file1=full_file_name

			plot_now=False
			if check_info_file(file_name)==True:
				plot_data.file0=file_name
				plot_xy_window=plot_dlg_class(gtk.WINDOW_TOPLEVEL)
				plot_xy_window.my_init(plot_data)
				plot_now=plot_xy_window.my_run(plot_data)
			else:
				plot_data.file0=file_name
				plot_data.tag0=""
				plot_data.file1=""
				plot_data.tag1=""
				plot_now=True

			if plot_now==True:
				self.plot_results(plot_data)

				self.plotted_graphs.refresh()


	def save_combo(self):
		self.make_sim_dir()
		a = open(os.path.join(self.sim_dir,"gpvdm_gui_config.inp"), "w")
		a.write(str(self.tab.rowCount())+"\n")


		for i in range(0,self.tab.rowCount()):
			a.write(tab_get_value(self.tab,i,0)+"\n")
			a.write(tab_get_value(self.tab,i,1)+"\n")
			a.write(tab_get_value(self.tab,i,2)+"\n")
			a.write(tab_get_value(self.tab,i,3)+"\n")
			a.write(tab_get_value(self.tab,i,4)+"\n")
			a.write(tab_get_value(self.tab,i,5)+"\n")
		a.close()

		if os.path.isfile(os.path.join(self.sim_dir,"scan_config.inp"))==False:
			a = open(os.path.join(self.sim_dir,"scan_config.inp"), "w")
			a.write("#args\n")
			a.write("\n")
			a.write("#end\n")

			a.close()

	def combo_changed(self, widget, path, text, model):
		model[path][2] = text
		model[path][0] = scan_items_get_file(text)
		model[path][1] = scan_items_get_token(text)
		self.rebuild_liststore_op_type()
		self.save_combo()

	def combo_mirror_changed(self, widget, path, text, model):
		model[path][4] = text
		if model[path][4]!="constant":
			if model[path][4]!="scan":
				if model[path][4]!="python_code":
					model[path][3] = "mirror"
		self.save_combo()


	def toggled_cb( self, cell, path, model ):
		model[path][3] = not model[path][3]
		print model[path][2],model[path][3]
		self.save_combo()
		return

	def load(self):
		self.tab.clear()
		self.tab.setRowCount(0)
		self.tab.setHorizontalHeaderLabels([_("File"), _("Token"), _("Parameter to change"), _("Values"), _("Opperation")])
		tree_load_model(self.tab,self.sim_dir)

	def contextMenuEvent(self, event):
		self.popMenu.popup(QCursor.pos())


	def callback_close(self,widget):
		self.hide()

	def rebuild_liststore_op_type(self):
		self.liststore_op_type.clear()
		self.liststore_op_type.append(["scan"])
		self.liststore_op_type.append(["constant"])
		self.liststore_op_type.append(["python_code"])

		for i in range(0,len(self.liststore_combobox)):
			if self.liststore_combobox[i][0]!=_("Select parameter"):
				self.liststore_op_type.append([self.liststore_combobox[i][2]])

	def set_tab_caption(self,name):
		mytext=name
		if len(mytext)<10:
			for i in range(len(mytext),10):
				mytext=mytext+" "
		self.tab_label.set_text(mytext)

	def set_visible(self,value):
		if value==True:
			self.visible=True
			self.config.set_value("#visible",True)
			self.show()
		else:
			self.visible=False
			self.config.set_value("#visible",False)
			self.hide()

	def callback_run_simulation(self):
		args=inp_get_token_value(os.path.join("scan_config.inp",self.sim_dir),"#args")
		if args==None:
			args=""
		self.simulate(True,True,args)


	def callback_stop_simulation(self):
		self.stop_simulation()

	def callback_examine(self):
		mycmp=cmp_class()
		ret=mycmp.init(self.sim_dir,get_exe_command())
		if ret==False:
			error_dlg(self,_("Re-run the simulation with 'dump all slices' set to one to use this tool."))
			return

	def __init__(self,myserver,status_bar,scan_root_dir,sim_name):
		QWidget.__init__(self)

		self.main_vbox = QVBoxLayout()

		self.tokens=tokens()
		self.config=config()
		self.sim_name=sim_name
		self.myserver=myserver
		self.status_bar=status_bar
		self.param_list=scan_items_get_list()
		#self.tab_label=tab_label

		self.sim_dir=os.path.join(scan_root_dir,sim_name)
		self.tab_name=os.path.basename(os.path.normpath(self.sim_dir))

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))

		self.tb_add = QAction(QIcon(os.path.join(get_image_file_path(),"add.png")), _("Add parameter to scan"), self)
		self.tb_add.triggered.connect(self.callback_add_item)
		toolbar.addAction(self.tb_add)

		self.tb_minus = QAction(QIcon(os.path.join(get_image_file_path(),"minus.png")), _("Delete item"), self)
		self.tb_minus.triggered.connect(self.callback_delete_item)
		toolbar.addAction(self.tb_minus)

		self.tb_down = QAction(QIcon(os.path.join(get_image_file_path(),"down.png")), _("Move down"), self)
		self.tb_down.triggered.connect(self.callback_move_down)
		toolbar.addAction(self.tb_down)

		self.tb_notes = QAction(QIcon(os.path.join(get_image_file_path(),"down.png")), _("Notes"), self)
		self.tb_notes.triggered.connect(self.callback_notes)
		toolbar.addAction(self.tb_notes)

		self.tb_notes = QAction(QIcon(os.path.join(get_image_file_path(),"select.png")), _("Select parameter to change"), self)
		self.tb_notes.triggered.connect(self.callback_show_list)
		toolbar.addAction(self.tb_notes)

		self.tb_simulate = QAction(QIcon(os.path.join(get_image_file_path(),"forward.png")), _("Run simulation"), self)
		self.tb_simulate.triggered.connect(self.callback_run_simulation)
		toolbar.addAction(self.tb_simulate)

		self.tb_stop = QAction(QIcon(os.path.join(get_image_file_path(),"pause.png")), _("Stop the simulation"), self)
		self.tb_stop.triggered.connect(self.callback_stop_simulation)
		toolbar.addAction(self.tb_stop)

		self.tb_plot = QAction(QIcon(os.path.join(get_image_file_path(),"plot.png")), _("Find a file to plot"), self)
		self.tb_plot.triggered.connect(self.callback_gen_plot_command)
		toolbar.addAction(self.tb_plot)

		self.tb_plot_time = QAction(QIcon(os.path.join(get_image_file_path(),"plot_time.png")), _("Examine results in time domain"), self)
		self.tb_plot_time.triggered.connect(self.callback_examine)
		toolbar.addAction(self.tb_plot_time)

		self.tb_command = QAction(QIcon(os.path.join(get_image_file_path(),"command.png")), _("Insert python command"), self)
		self.tb_command.triggered.connect(self.callback_insert_command)
		toolbar.addAction(self.tb_command)

		self.main_vbox.addWidget(toolbar)

		self.tab = QTableWidget()
		self.tab.resizeColumnsToContents()

		self.tab.verticalHeader().setVisible(False)

		self.tab.clear()
		self.tab.setColumnCount(6)
		self.tab.setSelectionBehavior(QAbstractItemView.SelectRows)

		self.load()

		self.main_vbox.addWidget(self.tab)

		self.popMenu = QMenu(self)

		self.mp_show_list=QAction(_("Select parameter to scan"), self)
		self.mp_show_list.triggered.connect(self.callback_show_list)
		self.popMenu.addAction(self.mp_show_list)

		self.popMenu.addSeparator()

		self.mp_delete=QAction(_("Delete item"), self)
		self.mp_delete.triggered.connect(self.callback_delete_item)
		self.popMenu.addAction(self.mp_delete)

		self.mp_copy=QAction(_("Copy"), self)
		self.mp_copy.triggered.connect(self.callback_copy_item)
		self.popMenu.addAction(self.mp_copy)

		self.mp_paste=QAction(_("Paste"), self)
		self.mp_paste.triggered.connect(self.callback_paste_item)
		self.popMenu.addAction(self.mp_paste)

		self.popMenu.addSeparator()

		self.mp_add=QAction(_("Add item"), self)
		self.mp_add.triggered.connect(self.callback_add_item)
		self.popMenu.addAction(self.mp_add)

		self.mp_down=QAction(_("Move down"), self)
		self.mp_down.triggered.connect(self.callback_move_down)
		self.popMenu.addAction(self.mp_down)

		self.mp_down=QAction(_("Move down"), self)
		self.mp_down.triggered.connect(self.callback_move_down)
		self.popMenu.addAction(self.mp_down)

		self.popMenu.addSeparator()

		self.setLayout(self.main_vbox)


