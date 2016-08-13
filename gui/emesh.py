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



import os
from cal_path import get_exe_command
from numpy import *
from matplotlib.figure import Figure
from cal_path import get_image_file_path
import webbrowser
from electrical_mesh_editor import electrical_mesh_editor
from inp_util import inp_search_token_value
from epitaxy import epitaxy_get_dos_files
#from mesh_dump_ctl import mesh_dump_ctl

#inp
from inp import inp_load_file
from inp import inp_sum_items

#qt
from PyQt5.QtWidgets import QMainWindow, QTextEdit, QAction, QApplication
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QSizePolicy,QHBoxLayout,QPushButton,QDialog,QFileDialog,QToolBar,QMessageBox,QVBoxLayout
from PyQt5.QtCore import pyqtSignal

#windows
from window_list import windows
from mesh import mesh_load_all

#matplotlib
import matplotlib
matplotlib.use('Qt5Agg')
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar

class tab_electrical_mesh(QWidget):
	lines=[]
	edit_list=[]

	line_number=[]

	file_name=""
	name=""
	visible=1

	changed = pyqtSignal()

	def save_image(self,file_name):
		self.fig.savefig(file_name)

	def refresh(self):
		self.emesh_editor_y.refresh()
		#self.update_graph()


	def callback_close(self, widget, data=None):
		self.hide()
		return True


	def callback_help(self, widget, data=None):
		webbrowser.open('http://www.gpvdm.com/man/index.html')

	def callback_dim_1d(self):
		self.emesh_editor_y.enable_dim()
		self.emesh_editor_x.disable_dim()
		self.emesh_editor_z.disable_dim()
		self.update_dim()
		mesh_load_all()
		
	def callback_dim_2d(self):
		self.emesh_editor_y.enable_dim()
		self.emesh_editor_x.enable_dim()
		self.emesh_editor_z.disable_dim()
		self.update_dim()
		mesh_load_all()

	def callback_dim_3d(self):
		self.emesh_editor_y.enable_dim()
		self.emesh_editor_x.enable_dim()
		self.emesh_editor_z.enable_dim()
		self.update_dim()
		mesh_load_all()


	def update_dim(self):
		if self.emesh_editor_x.mesh_points==1 and self.emesh_editor_z.mesh_points==1:
			self.one_d.setEnabled(False)
			self.two_d.setEnabled(True)
			self.three_d.setEnabled(True)
			self.emesh_editor_y.setEnabled(True)
			self.emesh_editor_x.setEnabled(False)
			self.emesh_editor_z.setEnabled(False)


		if self.emesh_editor_x.mesh_points>1 and self.emesh_editor_z.mesh_points==1:
			self.one_d.setEnabled(True)
			self.two_d.setEnabled(False)
			self.three_d.setEnabled(True)
			self.emesh_editor_y.setEnabled(True)
			self.emesh_editor_x.setEnabled(True)
			self.emesh_editor_z.setEnabled(False)


		if self.emesh_editor_x.mesh_points>1 and self.emesh_editor_z.mesh_points>1:
			self.one_d.setEnabled(True)
			self.two_d.setEnabled(True)
			self.three_d.setEnabled(False)
			self.emesh_editor_y.setEnabled(True)
			self.emesh_editor_x.setEnabled(True)
			self.emesh_editor_z.setEnabled(True)

		self.changed.emit()


	def __init__(self):
		QWidget.__init__(self)
		self.setFixedSize(900, 600)
		self.setWindowIcon(QIcon(os.path.join(get_image_file_path(),"mesh.png")))

		self.setWindowTitle(_("Electrical Mesh Editor - (www.gpvdm.com)")) 
		

		self.main_vbox = QVBoxLayout()

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))

		self.one_d = QAction(QIcon(os.path.join(get_image_file_path(),"1d.png")), "1D simulation", self)
		self.one_d.triggered.connect(self.callback_dim_1d)
		toolbar.addAction(self.one_d)

		self.two_d = QAction(QIcon(os.path.join(get_image_file_path(),"2d.png")), "2D simulation", self)
		self.two_d.triggered.connect(self.callback_dim_2d)
		toolbar.addAction(self.two_d)

		self.three_d = QAction(QIcon(os.path.join(get_image_file_path(),"3d.png")), "3D simulation", self)
		self.three_d.triggered.connect(self.callback_dim_3d)
		toolbar.addAction(self.three_d)

		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		toolbar.addWidget(spacer)

		self.undo = QAction(QIcon(os.path.join(get_image_file_path(),"help.png")), 'Hide', self)
		self.undo.setStatusTip(_("Close"))
		self.undo.triggered.connect(self.callback_help)
		toolbar.addAction(self.undo)

		self.main_vbox.addWidget(toolbar)
		
		self.win_list=windows()
		self.win_list.load()
		self.win_list.set_window(self,"emesh_window")

		widget=QWidget()
		mesh_hbox=QHBoxLayout()
		widget.setLayout(mesh_hbox)
	
		self.emesh_editor_x=electrical_mesh_editor("x")
		self.emesh_editor_y=electrical_mesh_editor("y")
		self.emesh_editor_z=electrical_mesh_editor("z")

		mesh_hbox.addWidget(self.emesh_editor_x)
		mesh_hbox.addWidget(self.emesh_editor_y)
		mesh_hbox.addWidget(self.emesh_editor_z)

		self.main_vbox.addWidget(widget)

		self.update_dim()

		self.setLayout(self.main_vbox)

		return

		self.mesh_dump_ctl=mesh_dump_ctl()
		self.mesh_dump_ctl.init()
		self.mesh_dump_ctl.show()
		window_main_vbox.pack_start(self.mesh_dump_ctl, True, True, 0)

		#self.mesh_dump_ctl.connect("update", self.callback_update)

		self.statusbar = gtk.Statusbar()
		self.statusbar.show()
		window_main_vbox.pack_start(self.statusbar, False, False, 0)

		self.add(window_main_vbox)
		self.connect("delete-event", self.callback_close)
		self.set_position(gtk.WIN_POS_CENTER)



