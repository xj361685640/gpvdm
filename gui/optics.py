#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2016 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
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
from inp import inp_update_token_value
from inp import inp_get_token_value
from plot_gen import plot_gen
from cal_path import get_image_file_path
import zipfile
import glob
from scan_item import scan_item_add
from tab import tab_class
import webbrowser
from progress import progress_class
from help import my_help_class

#path
from cal_path import get_materials_path
from cal_path import get_plugins_path
from cal_path import get_exe_command

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QHBoxLayout,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QSystemTrayIcon,QMenu, QComboBox, QMenuBar, QLabel
from PyQt5.QtGui import QIcon

#windows
from band_graph import band_graph
from plot_widget import plot_widget
from gui_util import error_dlg
from fx_selector import fx_selector


def find_models():
	ret=[]
	path=get_plugins_path()

	for file in glob.glob(os.path.join(path,"*")):
		file_name=os.path.basename(file)
		if file_name.startswith("light_"):
			if file_name.endswith(".dll") or file_name.endswith(".so"):
				ret.append(os.path.splitext(os.path.basename(file_name[6:]))[0])

	return ret

def find_light_source():
	ret=[]

	path=get_materials_path()


	for file in glob.glob(os.path.join(path,"*.spectra")):
		ret.append(os.path.splitext(os.path.basename(file))[0])

	return ret

def find_materials():
	ret=[]

	path=get_materials_path()

	for file in glob.glob(os.path.join(path,"*")):
		if os.path.isdir(file)==True:
			ret.append(os.path.splitext(os.path.basename(file))[0])

	return ret

class class_optical(QWidget):

	edit_list=[]

	line_number=[]

	file_name=""
	name=""
	visible=1

	def __init__(self):
		QWidget.__init__(self)
		find_models()

		self.setWindowIcon(QIcon(os.path.join(get_image_file_path(),"image.png")))

		self.setMinimumSize(1000, 600)

		self.edit_list=[]
		self.line_number=[]
		self.articles=[]
		input_files=[]
		input_files.append(os.path.join(os.getcwd(),"light_dump","light_2d_photons.dat"))
		input_files.append(os.path.join(os.getcwd(),"light_dump","light_2d_photons_asb.dat"))
		input_files.append(os.path.join(os.getcwd(),"light_dump","reflect.dat"))

		plot_labels=[]
		plot_labels.append("Photon distribution")
		plot_labels.append("Photon distribution absorbed")
		plot_labels.append("Reflection")


		self.setGeometry(300, 300, 600, 600)
		self.setWindowTitle(_("Optical simulation editor (www.gpvdm.com)"))    

		self.setWindowIcon(QIcon(os.path.join(get_image_file_path(),"optics.png")))

		self.main_vbox=QVBoxLayout()

		menubar = QMenuBar()

		file_menu = menubar.addMenu('&File')
		self.menu_refresh=file_menu.addAction(_("&Refresh"))
		self.menu_refresh.triggered.connect(self.update)

		self.menu_close=file_menu.addAction(_("&Close"))
		self.menu_close.triggered.connect(self.callback_close)

		self.main_vbox.addWidget(menubar)

		toolbar=QToolBar()

		toolbar.setIconSize(QSize(48, 48))

		self.run = QAction(QIcon(os.path.join(get_image_file_path(),"play.png")), _("Run"), self)
		self.run.triggered.connect(self.callback_run)
		toolbar.addAction(self.run)

		self.fx_box=fx_selector()
		self.fx_box.show_all=True
		self.fx_box.file_name_set_start("light_1d_") 
		self.fx_box.file_name_set_end("_photons_abs.dat")
		self.fx_box.update()

		self.fx_box.cb.currentIndexChanged.connect(self.mode_changed)
		toolbar.addWidget(self.fx_box)

		label=QLabel(_("Optical model:"))
		toolbar.addWidget(label)

		self.cb_model = QComboBox()
		toolbar.addWidget(self.cb_model)
		self.update_cb_model()
		
		self.cb_model.activated.connect(self.on_cb_model_changed)

		label=QLabel(_("Solar spectrum:"))
		toolbar.addWidget(label)
		self.light_source_model = QComboBox()
		self.update_light_source_model()
		toolbar.addWidget(self.light_source_model)
		self.light_source_model.activated.connect(self.on_light_source_model_changed)
		
		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		toolbar.addWidget(spacer)


		self.help = QAction(QIcon(os.path.join(get_image_file_path(),"help.png")), 'Help', self)
		self.help.triggered.connect(self.callback_help)
		toolbar.addAction(self.help)

		self.main_vbox.addWidget(toolbar)


		self.progress_window=progress_class()

		self.notebook = QTabWidget()

		self.notebook.setMovable(True)


		self.fig_photon_density = band_graph()
		self.fig_photon_density.set_data_file("light_1d_photons_tot_norm.dat")
		self.fig_photon_density.init()
		self.notebook.addTab(self.fig_photon_density,"Photon density")

		self.fig_photon_abs = band_graph()
		self.fig_photon_abs.set_data_file("light_1d_photons_tot_abs_norm.dat")
		self.fig_photon_abs.init()
		self.notebook.addTab(self.fig_photon_abs,"Photon absorbed")

		widget=tab_class()
		widget.init("light.inp","Optical setup")
		self.notebook.addTab(widget,"Optical setup")


		self.plot_widgets=[]
		self.progress_window.start()
		for i in range(0,len(input_files)):
			self.plot_widgets.append(plot_widget())
			self.plot_widgets[i].init()
			self.plot_widgets[i].set_labels([plot_labels[0]])
			self.plot_widgets[i].load_data([input_files[i]],os.path.splitext(input_files[i])[0]+".oplot")

			self.plot_widgets[i].do_plot()
			#self.plot_widgets[i].show()
			self.notebook.addTab(self.plot_widgets[i],plot_labels[i])


		self.fig_photon_density.draw_graph()
		self.fig_photon_abs.draw_graph()

		self.progress_window.stop()


		self.main_vbox.addWidget(self.notebook)


		self.setLayout(self.main_vbox)

		return


	def onclick(self, event):
		for i in range(0,len(self.layer_end)):
			if (self.layer_end[i]>event.xdata):
				break
		pwd=os.getcwd()
		plot_gen([os.path.join(pwd,"materials",self.layer_name[i],"alpha.omat")],[],None,"")


	def update_cb_model(self):
		self.cb_model.blockSignals(True)

		self.cb_model.clear()
		models=find_models()
		if len(models)==0:
			error_dlg(self,_("I can't find any optical plugins, I think the model is not installed properly."))
			return

		for i in range(0, len(models)):
			self.cb_model.addItem(models[i])

		used_model=inp_get_token_value("light.inp", "#light_model")
		print(models,used_model)
		if models.count(used_model)==0:
			used_model="exp"
			inp_update_token_value("light.inp", "#light_model","exp",1)
			self.cb_model.setCurrentIndex(self.cb_model.findText(used_model))
		else:
			self.cb_model.setCurrentIndex(self.cb_model.findText(used_model))

		scan_item_add("light.inp","#light_model","Optical model",1)

		self.cb_model.blockSignals(False)

	def update_light_source_model(self):
		self.light_source_model.blockSignals(True)
		models=find_light_source()
		for i in range(0, len(models)):
			self.light_source_model.addItem(models[i])

		used_model=inp_get_token_value("light.inp", "#sun")

		print("models================",models,used_model)
		if models.count(used_model)==0:
			used_model="sun"
			inp_update_token_value("light.inp", "#sun","sun",1)

		self.light_source_model.setCurrentIndex(self.light_source_model.findText(used_model))
		scan_item_add("light.inp","#sun","Light source",1)
		self.light_source_model.blockSignals(False)

	def callback_close(self):
		self.hide()
		return True


	def update(self):
		self.fig_photon_density.my_figure.clf()
		self.fig_photon_density.draw_graph()
		self.fig_photon_density.canvas.draw()

		self.fig_photon_abs.my_figure.clf()
		self.fig_photon_abs.draw_graph()
		self.fig_photon_abs.canvas.draw()

		for i in range(0,len(self.plot_widgets)):
			self.plot_widgets[i].update()


	def callback_run(self):
		dump_optics=inp_get_token_value("dump.inp", "#dump_optics")
		dump_optics_verbose=inp_get_token_value("dump.inp", "#dump_optics_verbose")

		inp_update_token_value("dump.inp", "#dump_optics","true",1)
		inp_update_token_value("dump.inp", "#dump_optics_verbose","true",1)
		cmd = get_exe_command()+' --simmode opticalmodel@optics'
		print(cmd)
		ret= os.system(cmd)

		inp_update_token_value("dump.inp", "#dump_optics",dump_optics,1)
		inp_update_token_value("dump.inp", "#dump_optics_verbose",dump_optics_verbose,1)
		
		self.update()
		self.fx_selector.update()

		inp_update_token_value("dump.inp", "#dump_optics","true",1)
		inp_update_token_value("dump.inp", "#dump_optics_verbose","true",1)
		

	def mode_changed(self):
		cb_text=self.fx_box.get_text()

		if cb_text=="all":
			self.fig_photon_density.set_data_file("light_1d_photons_tot_norm.dat")
			self.fig_photon_abs.set_data_file("light_1d_photons_tot_abs_norm.dat")
		else:
			self.fig_photon_density.set_data_file("light_1d_"+cb_text+"_photons_norm.dat")
			self.fig_photon_abs.set_data_file("light_1d_"+cb_text+"_photons_abs.dat")

		self.update()

	def on_cb_model_changed(self):
		cb_text=self.cb_model.currentText()
		inp_update_token_value("light.inp", "#light_model", cb_text,1)


	def on_light_source_model_changed(self):
		cb_text=self.light_source_model.currentText()
		cb_text=cb_text
		inp_update_token_value("light.inp", "#sun", cb_text,1)

	def callback_help(self, widget, data=None):
		webbrowser.open('http://www.gpvdm.com/man/index.html')

