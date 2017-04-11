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
from tab import tab_class
from window_list import windows
from icon_lib import QIcon_load

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QHBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QTextEdit,QComboBox,QLabel,QLineEdit,QDialog
from PyQt5.QtGui import QPainter,QIcon

#python modules
import webbrowser

#windows
from tab import tab_class
from tab_lang import language_tab_class

from PyQt5.QtCore import pyqtSignal

from open_save_dlg import open_as_filter

from dat_file import dat_file_read
from dat_file import dat_file

from plot_io import plot_load_info
from dat_file_class import dat_file

from plot_io import gen_header_from_token

from PyQt5.QtCore import pyqtSignal

from window_list import resize_window_to_be_sane
from mesh import mesh_get_xlen
from mesh import mesh_get_zlen

articles = []
mesh_articles = []

class graph_data_display(QWidget):
	changed = pyqtSignal()
	
	def populate_boxes(self):
		self.x_label=self.items[self.x_combo.currentIndex()][2]
		self.x_units=self.items[self.x_combo.currentIndex()][3]
		self.x_mul_to_si=self.items[self.x_combo.currentIndex()][4]
		self.x_disp_mul=self.items[self.x_combo.currentIndex()][5]

		self.y_label=self.items[self.y_combo.currentIndex()][2]
		self.y_units=self.items[self.y_combo.currentIndex()][3]
		self.y_mul_to_si=self.items[self.y_combo.currentIndex()][4]
		self.y_disp_mul=self.items[self.y_combo.currentIndex()][5]

		self.set_xlabel(self.items[self.x_combo.currentIndex()][2])
		self.set_ylabel(self.items[self.y_combo.currentIndex()][2])
		self.set_title(self.items[self.x_combo.currentIndex()][2]+" - "+self.items[self.y_combo.currentIndex()][2])
				
	def add_units(self,box):
		for i in range(0,len(self.items)):
			box.addItem(self.items[i][0]+" ("+self.items[i][1]+")")

	def callback_edited(self):
		self.populate_boxes()
		self.changed.emit()

	def __init__(self):
		QWidget.__init__(self)
		self.items=[]
		#input description+units	//output label // Output unit// equation to si //mull si to display
		self.items.append([_("Wavelength"),"nm",_("Wavelength"),"nm","1e-9",1e9])
		self.items.append([_("Wavelength"),"um",_("Wavelength"),"nm","1e-6",1e9])
		self.items.append([_("Wavelength"),"cm",_("Wavelength"),"nm","1e-3",1e9])
		self.items.append([_("Wavelength"),"m",_("Wavelength"),"nm","1.0",1e9])
		self.items.append([_("J"),"mA/cm2",_("J"),"A/m2","10000.0/1000.0",1.0])
		self.items.append([_("J"),"m/m2",_("J"),"A/m2","1.0",1.0])
		self.items.append([_("Amps"),"A",_("J"),"A/m2","1.0/("+str(mesh_get_xlen())+"*"+str(mesh_get_zlen())+")",1.0])
		self.items.append([_("-Amps"),"A",_("J"),"A/m2","-1.0/("+str(mesh_get_xlen())+"*"+str(mesh_get_zlen())+")",1.0])
		self.items.append([_("Voltage"),"V",_("Voltage"),"V","1.0",1.0])
		self.items.append([_("-Voltage"),"V",_("Voltage"),"V","-1.0",1.0])
		self.items.append([_("Voltage"),"mV",_("Voltage"),"V","1e-3",1.0])
		self.items.append([_("Attenuation coefficient"),"au",_("Absorption"),"m^{-1}","4*3.14159/x",1.0])
		self.items.append([_("Refractive index"),"au",_("Refractive index"),"au","1.0",1.0])

		i=0
		self.x_label=self.items[i][2]
		self.x_units=self.items[i][3]
		self.x_mul_to_si=self.items[i][4]
		self.x_disp_mul=self.items[i][5]

		self.y_label=self.items[i][2]
		self.y_units=self.items[i][3]
		self.y_mul_to_si=self.items[i][4]
		self.y_disp_mul=self.items[i][5]

		self.main_vbox=QVBoxLayout()
		self.x_combo=QComboBox()
		self.add_units(self.x_combo)
		self.y_combo=QComboBox()
		self.add_units(self.y_combo)
		
		self.units_x_label=QLabel(_("x units:"))
		self.units_data_label=QLabel(_("y units:"))
		

		self.x_combo.currentIndexChanged.connect(self.callback_edited)
		self.y_combo.currentIndexChanged.connect(self.callback_edited)


		self.title_widget=QWidget()
		self.title_hbox=QHBoxLayout()
		self.title_label=QLabel(_("Title:"))
		self.title_entry=QLineEdit()
		self.title_hbox.addWidget(self.title_label)
		self.title_hbox.addWidget(self.title_entry)
		self.title_widget.setLayout(self.title_hbox)
		self.main_vbox.addWidget(self.title_widget)

		self.xlabel_widget=QWidget()
		self.xlabel_hbox=QHBoxLayout()
		self.xlabel_label=QLabel(_("x-label:"))
		self.xlabel_entry=QLineEdit()
		self.xlabel_hbox.addWidget(self.xlabel_label)
		self.xlabel_hbox.addWidget(self.xlabel_entry)
		self.xlabel_hbox.addWidget(self.units_x_label)
		self.xlabel_hbox.addWidget(self.x_combo)
		self.xlabel_widget.setLayout(self.xlabel_hbox)
		self.main_vbox.addWidget(self.xlabel_widget)

		self.ylabel_widget=QWidget()
		self.ylabel_hbox=QHBoxLayout()
		self.ylabel_label=QLabel(_("y-label:"))
		self.ylabel_entry=QLineEdit()
		self.ylabel_hbox.addWidget(self.ylabel_label)
		self.ylabel_hbox.addWidget(self.ylabel_entry)
		self.ylabel_hbox.addWidget(self.units_data_label)
		self.ylabel_hbox.addWidget(self.y_combo)
		self.ylabel_widget.setLayout(self.ylabel_hbox)
		self.main_vbox.addWidget(self.ylabel_widget)

		self.xlabel_entry.textEdited.connect(self.callback_edited)
		self.ylabel_entry.textEdited.connect(self.callback_edited)
		self.title_entry.textEdited.connect(self.callback_edited)

		self.setLayout(self.main_vbox)


	def set_xlabel(self,text):
		self.xlabel_entry.blockSignals(True)
		self.xlabel_entry.setText(text)
		self.xlabel_entry.blockSignals(False)

	def set_ylabel(self,text):
		self.ylabel_entry.blockSignals(True)
		self.ylabel_entry.setText(text)
		self.ylabel_entry.blockSignals(False)

	def set_title(self,text):
		self.title_entry.blockSignals(True)
		self.title_entry.setText(text)
		self.title_entry.blockSignals(False)

	def enable_units(self,val):
		self.x_combo.setEnabled(val)
		self.y_combo.setEnabled(val)

	def get_title(self):
		return self.title_entry.text()

	def get_xlabel(self):
		return self.xlabel_entry.text()

	def get_ylabel(self):
		return self.ylabel_entry.text()
		
class import_data(QDialog):

	changed = pyqtSignal()
	
	def callback_tab_changed(self):
		self.changed.emit()

	def __init__(self,out_file):
		QDialog.__init__(self)
		self.out_file=out_file
		self.path=os.path.dirname(self.out_file)
		resize_window_to_be_sane(self,0.6,0.7)
		self.data=dat_file()
		self.info_token=dat_file()

		#self.setFixedSize(900, 600)
		self.setWindowIcon(QIcon_load("import"))

		self.setWindowTitle(_("Import data")+" (https://www.gpvdm.com)") 

		self.main_vbox = QVBoxLayout()
	

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))

		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		
		self.open_data= QAction(QIcon_load("document-open"), _("Open file"), self)
		self.open_data.triggered.connect(self.callback_open)
		toolbar.addAction(self.open_data)

		self.import_data= QAction(QIcon_load("document-save-as"), _("Import data to model"), self)
		self.import_data.triggered.connect(self.callback_import)
		self.import_data.setEnabled(False)
		toolbar.addAction(self.import_data)
		
		toolbar.addWidget(spacer)


		self.tb_help = QAction(QIcon_load("help"), _("Help"), self)
		self.tb_help.setStatusTip(_("Help"))
		self.tb_help.triggered.connect(self.callback_help)
		toolbar.addAction(self.tb_help)

		self.main_vbox.addWidget(toolbar)


		self.input_output_hbox=QHBoxLayout()

		self.raw_data_widget=QWidget()
		self.raw_data_hbox=QVBoxLayout()
		self.raw_data_widget.setLayout(self.raw_data_hbox)
		self.raw_data = QTextEdit(self)
		self.raw_data.setReadOnly(True)
		self.raw_data.setLineWrapMode(QTextEdit.NoWrap)
		font = self.raw_data.font()
		font.setFamily("Courier")
		font.setPointSize(10)
		self.raw_data_label=QLabel(_("The file to import:"))
		self.raw_data_hbox.addWidget(self.raw_data_label)
		self.raw_data_hbox.addWidget(self.raw_data)
		self.raw_data_path=QLabel()
		self.raw_data_hbox.addWidget(self.raw_data_path)

		self.out_data_widget=QWidget()
		self.out_data_hbox=QVBoxLayout()
		self.out_data_widget.setLayout(self.out_data_hbox)
		self.out_data = QTextEdit(self)
		self.out_data.setReadOnly(True)
		self.out_data.setLineWrapMode(QTextEdit.NoWrap)
		font = self.out_data.font()
		font.setFamily("Courier")
		font.setPointSize(10)
		self.out_data_label=QLabel(_("The imported file, the numbers should now be in SI units"))
		self.out_data_hbox.addWidget(self.out_data_label)

		self.out_data_hbox.addWidget(self.out_data)

		self.out_data_path=QLabel()
		self.out_data_hbox.addWidget(self.out_data_path)
		
		self.input_output_hbox.addWidget(self.raw_data_widget)
		self.input_output_hbox.addWidget(self.out_data_widget)
		self.input_output_widget=QWidget()
		self.input_output_widget.setLayout(self.input_output_hbox)

		self.unit_sel=graph_data_display()
		self.main_vbox.addWidget(self.unit_sel)
		self.main_vbox.addWidget(self.input_output_widget)		

		self.unit_sel.changed.connect(self.update)
		self.setLayout(self.main_vbox)

		self.out_data_path.setText(self.out_file)
		
		self.win_list=windows()
		self.win_list.load()
		self.win_list.set_window(self,"import_window")

		self.ret=self.load_file()
			
	def callback_help(self,widget):
		webbrowser.open('https://www.gpvdm.com/man/index.html')

	def closeEvent(self, event):
		self.win_list.update(self,"config_window")
		#self.hide()

	def gen_output(self):
		text="#gpvdm\n"
		text=text+"\n".join(gen_header_from_token(self.info_token))
		text=text+"\n"

		for i in range(0,self.data.y_len):
			x=0
			x_command="self.data.y_scale[i]*"+self.unit_sel.x_mul_to_si
			x=eval(x_command)
			y_command="self.data.data[0][0][i]*"+self.unit_sel.y_mul_to_si
			y=eval(y_command)
			x_text=str('{:.8e}'.format(float(x)))
			y_text=str('{:.8e}'.format(float(y)))
			text=text+x_text+" "+y_text+"\n"

		text=text+"#end\n"
		self.out_data.setText(text)

	def update(self):
		self.info_token.title=self.unit_sel.get_title()

		self.info_token.x_label=self.unit_sel.get_xlabel()
		self.info_token.data_label=self.unit_sel.get_ylabel()

		self.info_token.x_units=self.unit_sel.x_units
		self.info_token.data_units=self.unit_sel.y_units
		
		self.info_token.x_mul=self.unit_sel.x_disp_mul
		self.info_token.y_mul=self.unit_sel.y_disp_mul

		self.info_token.x_len=self.data.x_len
		self.info_token.y_len=self.data.y_len
		self.info_token.z_len=self.data.z_len
				
		self.gen_output()

		
	def callback_import(self):
		a = open(self.out_file, "w")
		a.write(self.out_data.toPlainText())
		a.close()
		self.close()

	def load_file(self):
		file_name=open_as_filter(self,"dat (*.dat);;csv (*.csv);;txt (*.txt)",path=self.path)

		if file_name!=None:
			f = open(file_name, "r")
			lines = f.readlines()
			f.close()
			text=""
			for l in range(0, len(lines)):
				text=text+lines[l].rstrip()+"\n"
			self.raw_data_path.setText(file_name)
			self.raw_data.setText(text)

			got_info=plot_load_info(self.info_token,file_name)
			self.import_data.setEnabled(True)

			if dat_file_read(self.data,file_name)==True:
				#if got_info==False:
				self.unit_sel.populate_boxes()

				self.gen_output()
				return True
				#print("importing file",file_name)
				#shutil.copy(file_name, os.path.join(os.getcwd(),"fit_data"+str(self.index)+".inp"))
				#self.update()
		else:
			return False

	def callback_open(self):
		self.load_file()

	def run(self):
		self.exec_()
