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

#inp
from inp import inp_get_token_value

#path
from cal_path import get_materials_path

from gl import glWidget
from gl_fallback import gl_fallback
#qt
from PyQt5.QtWidgets import QMainWindow, QTextEdit, QAction, QApplication
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QSizePolicy,QVBoxLayout,QPushButton,QDialog,QFileDialog,QToolBar,QMessageBox, QLineEdit,QLabel
from PyQt5.QtCore import QTimer
from PyQt5.QtCore import pyqtSignal

from cal_path import get_image_file_path

from contacts import contacts_window
from emesh import tab_electrical_mesh

from help import help_window
from gl_cmp import gl_cmp

from util import str2bool
from fx_selector import fx_selector

open_gl_working=False

def is_open_gl_working():
	global open_gl_working
	if open_gl_working==True:
		return "yes"
	else:
		return "no"

class display_widget(QWidget):

	colors=[]

	def add_fallback(self):
		global open_gl_working
		open_gl_working=False
		self.tb_rotate.setEnabled(False)
		self.display=gl_fallback()
		self.hbox.addWidget(self.display)
		

	def __init__(self):
		QWidget.__init__(self)
		self.complex_display=False

		self.hbox=QVBoxLayout()
		self.gl_cmp=gl_cmp(os.path.join(os.getcwd(),"snapshots"))
		
		toolbar=QToolBar()
		toolbar.setIconSize(QSize(42, 42))

		self.tb_rotate = QAction(QIcon(os.path.join(get_image_file_path(),"rotate.png")), _("Rotate"), self)
		self.tb_rotate.triggered.connect(self.tb_rotate_click)
		toolbar.addAction(self.tb_rotate)
		self.tb_rotate.setEnabled(True)
		
		self.tb_contact = QAction(QIcon(os.path.join(get_image_file_path(),"contact.png")), _("Contacts"), self)
		self.tb_contact.triggered.connect(self.callback_contacts)
		toolbar.addAction(self.tb_contact)

		self.tb_mesh = QAction(QIcon(os.path.join(get_image_file_path(),"mesh.png")), _("Edit the electrical mesh"), self)
		self.tb_mesh.triggered.connect(self.callback_edit_mesh)
		toolbar.addAction(self.tb_mesh)


		self.tb_config = QAction(QIcon(os.path.join(get_image_file_path(),"cog.png")), _("Configuration"), self)
		self.tb_config.triggered.connect(self.callback_configure)
		toolbar.addAction(self.tb_config)

		self.fx_box=fx_selector()
		self.fx_box.file_name_set_start("light_ray_") 
		self.fx_box.file_name_set_end(".dat")
		self.fx_box.update()
		self.fx_box.cb.currentIndexChanged.connect(self.fx_box_changed)

		#self.fx_box.cb.currentIndexChanged.connect(self.mode_changed)
		toolbar.addWidget(self.fx_box)

		self.hbox.addWidget(toolbar)
		
		enable_3d=inp_get_token_value(os.path.join(os.getcwd(),"config.inp") , "#gui_config_3d_enabled")
		if enable_3d==None:
			enable_3d="True"
		enable_3d=str2bool(enable_3d)
		
		if enable_3d==True:
			self.display=glWidget(self)
			self.display.ray_file=os.path.join(os.getcwd(),"light_dump","light_ray_"+self.fx_box.get_text()+".dat")

			self.hbox.addWidget(self.display)
			self.display.setMinimumSize(800, 600)

			self.timer=QTimer()
			self.timer.setSingleShot(True)
			self.timer.timeout.connect(self.timer_update)
			self.timer.start(2000)
		else:
			self.add_fallback()
			
		self.setLayout(self.hbox)

		self.electrical_mesh=tab_electrical_mesh()
		self.electrical_mesh.changed.connect(self.recalculate)

		self.contacts_window=contacts_window()
		self.contacts_window.changed.connect(self.recalculate)

		self.gl_cmp.slider.changed.connect(self.recalculate)

	def fx_box_changed(self):
		self.display.ray_file=os.path.join(os.getcwd(),"light_dump","light_ray_"+self.fx_box.get_text()+".dat")
		self.display.update()
		#print("rod",self.display.ray_file)
		

	def tb_rotate_click(self):
		self.display.start_rotate()
		
	def timer_update(self):
		global open_gl_working
		
		open_gl_working=not self.display.failed
		
		if open_gl_working==True:
			print("OpenGL is working")
		else:
			print("OpenGL is not working going to fallback")
			self.hbox.removeWidget(self.display)
			self.display.deleteLater()
			self.display = None
			self.add_fallback()

			help_window().help_append(["warning.png",_("<big><b>OpenGL warning</b></big><br>It looks as if you don't have working 3D graphics acceleration on your computer.  gpvdm will therefore fallback to 2D mode. The model will still be fully functional, but not look quite so nice.")])

	def set_selected_layer(self,n):
		global open_gl_working
		if open_gl_working==True:
			self.display.selected_layer=n

	def recalculate(self):
		self.display.graph_path=self.gl_cmp.slider.get_file_name()
		self.display.graph_z_max=self.gl_cmp.slider.z_max
		self.display.graph_z_min=self.gl_cmp.slider.z_min
		self.display.recalculate()
		self.fx_box.update()

	#def update(self):
#		print("recalculate")
	#	self.display.update()
	#	self.fx_box.update()

	def callback_configure(self):
		if self.gl_cmp.isVisible()==True:
			self.gl_cmp.hide()
		else:
			self.gl_cmp.show()


	def callback_contacts(self):
		help_window().help_set_help(["contact.png",_("<big><b>Contacts window</b></big>\nUse this window to change the layout of the contacts on the device")])

		if self.contacts_window.isVisible()==True:
			self.contacts_window.hide()
		else:
			self.contacts_window.show()

	def callback_edit_mesh(self):
		help_window().help_set_help(["mesh.png",_("<big><b>Mesh editor</b></big>\nUse this window to setup the mesh, the window can also be used to change the dimensionality of the simulation.")])

		if self.electrical_mesh.isVisible()==True:
			self.electrical_mesh.hide()
		else:
			self.electrical_mesh.show()
