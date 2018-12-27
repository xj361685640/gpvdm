#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2016 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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

## @package optics
#  The main optics window
#


import os
from inp import inp_update_token_value
from inp import inp_get_token_value
from plot_gen import plot_gen
from icon_lib import icon_get
import zipfile
import glob
from scan_item import scan_item_add
from tab import tab_class
import webbrowser
from progress import progress_class
from help import my_help_class

#path
from cal_path import get_materials_path
from cal_path import get_exe_command

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QHBoxLayout,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QSystemTrayIcon,QMenu, QComboBox, QMenuBar, QLabel
from PyQt5.QtGui import QIcon

#windows
from band_graph import band_graph
from plot_widget import plot_widget
from error_dlg import error_dlg
from fx_selector import fx_selector

from server import server_get

from global_objects import global_object_run
from global_objects import global_object_delete
from cal_path import get_sim_path

from optics_ribbon import optics_ribbon

from css import css_apply

class class_optical(QWidget):

	edit_list=[]

	line_number=[]

	file_name=""
	name=""
	visible=1

	def __init__(self):
		QWidget.__init__(self)
		self.setWindowIcon(icon_get("optics"))

		self.setMinimumSize(1000, 600)

		self.ribbon=optics_ribbon()

		self.edit_list=[]
		self.line_number=[]
		self.articles=[]
		input_files=[]
		input_files.append(os.path.join(get_sim_path(),"light_dump","light_2d_photons.dat"))
		input_files.append(os.path.join(get_sim_path(),"light_dump","light_2d_photons_asb.dat"))
		input_files.append(os.path.join(get_sim_path(),"light_dump","reflect.dat"))

		plot_labels=[]
		plot_labels.append(_("Photon distribution"))
		plot_labels.append(_("Photon distribution absorbed"))
		plot_labels.append(_("Reflection"))


		self.setGeometry(300, 300, 600, 600)
		self.setWindowTitle(_("Optical simulation editor")+" (https://www.gpvdm.com)")    

		self.setWindowIcon(icon_get("optics"))

		self.main_vbox=QVBoxLayout()

		self.ribbon.run.triggered.connect(self.callback_run)

		self.ribbon.fx_box.cb.currentIndexChanged.connect(self.mode_changed)

		self.ribbon.help.triggered.connect(self.callback_help)

		self.ribbon.tb_save.triggered.connect(self.callback_save)
		
		self.main_vbox.addWidget(self.ribbon)


		self.progress_window=progress_class()

		self.notebook = QTabWidget()
		css_apply(self.notebook,"tab_default.css")
		self.notebook.setMovable(True)


		self.fig_photon_density = band_graph()
		self.fig_photon_density.set_data_file("light_1d_photons_tot_norm.dat")
		self.fig_photon_density.init()
		self.notebook.addTab(self.fig_photon_density,_("Photon density"))

		self.fig_photon_abs = band_graph()
		self.fig_photon_abs.set_data_file("light_1d_photons_tot_abs_norm.dat")
		self.fig_photon_abs.init()
		self.notebook.addTab(self.fig_photon_abs,_("Photon absorbed"))

		self.fig_gen_rate = band_graph()
		self.fig_gen_rate.set_data_file("light_1d_Gn.dat")
		self.fig_gen_rate.init()
		self.notebook.addTab(self.fig_gen_rate,_("Generation rate"))

		widget=tab_class()
		widget.init(os.path.join(get_sim_path(),"light.inp"),_("Optical setup"))
		self.notebook.addTab(widget,_("Optical setup"))


		self.plot_widgets=[]
		self.progress_window.start()
		for i in range(0,len(input_files)):
			self.plot_widgets.append(plot_widget())
			self.plot_widgets[i].init(enable_toolbar=False)
			self.plot_widgets[i].set_labels([plot_labels[0]])
			self.plot_widgets[i].load_data([input_files[i]],os.path.splitext(input_files[i])[0]+".oplot")

			self.plot_widgets[i].do_plot()
			#self.plot_widgets[i].show()
			self.notebook.addTab(self.plot_widgets[i],plot_labels[i])


		self.fig_photon_density.draw_graph()
		self.fig_photon_abs.draw_graph()
		self.fig_gen_rate.draw_graph()
		self.progress_window.stop()


		self.main_vbox.addWidget(self.notebook)


		self.setLayout(self.main_vbox)

		return


	def callback_save(self):
		tab = self.notebook.currentWidget()
		tab.save_image()

	def onclick(self, event):
		for i in range(0,len(self.layer_end)):
			if (self.layer_end[i]>event.xdata):
				break
		pwd=get_sim_path()
		plot_gen([os.path.join(pwd,"materials",self.layer_name[i],"alpha.omat")],[],None,"")


	def closeEvent(self, event):
		global_object_delete("optics_force_redraw")
		self.hide()
		event.accept()

	def optics_sim_finished(self):
		inp_update_token_value("dump.inp", "#dump_optics",self.dump_optics)
		#inp_update_token_value("dump.inp", "#dump_optics_verbose",self.dump_optics_verbose)
		self.force_redraw()

	def force_redraw(self):

		self.fig_photon_density.my_figure.clf()
		self.fig_photon_density.draw_graph()
		self.fig_photon_density.canvas.draw()

		self.fig_photon_abs.my_figure.clf()
		self.fig_photon_abs.draw_graph()
		self.fig_photon_abs.canvas.draw()

		self.fig_gen_rate.my_figure.clf()
		self.fig_gen_rate.draw_graph()
		self.fig_gen_rate.canvas.draw()


		for i in range(0,len(self.plot_widgets)):
			self.plot_widgets[i].update()
			
		self.ribbon.update()
		
	def callback_run(self):
		self.my_server=server_get()
		self.dump_optics=inp_get_token_value("dump.inp", "#dump_optics")
		#self.dump_optics_verbose=inp_get_token_value("dump.inp", "#dump_optics_verbose")

		inp_update_token_value("dump.inp", "#dump_optics","true")
		#inp_update_token_value("dump.inp", "#dump_optics_verbose","true")
		#pwd=os.getcwd()
		#os.chdir(get_sim_path())
		#cmd = get_exe_command()+' --simmode opticalmodel@optics'
		#print(cmd)
		#ret= os.system(cmd)
		#os.chdir(pwd)
		self.my_server.clear_cache()
		self.my_server.add_job(get_sim_path(),"--simmode opticalmodel@optics")
		self.my_server.set_callback_when_done(self.optics_sim_finished)
		self.my_server.start()

		
		#inp_update_token_value("dump.inp", "#dump_optics","true")
		#inp_update_token_value("dump.inp", "#dump_optics_verbose","true")
		

	def mode_changed(self):
		cb_text=self.ribbon.fx_box.get_text()
		if cb_text=="all":
			self.fig_photon_density.set_data_file("light_1d_photons_tot_norm.dat")
			self.fig_photon_abs.set_data_file("light_1d_photons_tot_abs_norm.dat")
		else:
			self.fig_photon_density.set_data_file("light_1d_"+cb_text+"_photons_norm.dat")
			self.fig_photon_abs.set_data_file("light_1d_"+cb_text+"_photons_abs.dat")
		self.force_redraw()
		self.update()


	def callback_help(self, widget, data=None):
		webbrowser.open('https://www.gpvdm.com/man/index.html')

