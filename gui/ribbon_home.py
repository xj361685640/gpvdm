# -*- coding: utf-8 -*-
#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2017 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
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
from cal_path import get_image_file_path

from dump_io import dump_io
from tb_item_sim_mode import tb_item_sim_mode
from tb_item_sun import tb_item_sun

from code_ctrl import enable_betafeatures
from cal_path import get_css_path

#qt
from PyQt5.QtWidgets import QMainWindow, QTextEdit, QAction, QApplication
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import QSize, Qt,QFile,QIODevice
from PyQt5.QtWidgets import QWidget,QSizePolicy,QVBoxLayout,QHBoxLayout,QPushButton,QDialog,QFileDialog,QToolBar,QMessageBox, QLineEdit, QToolButton
from PyQt5.QtWidgets import QTabWidget

from help import help_window
from gpvdm_open import gpvdm_open
from plot_gen import plot_gen
from info import sim_info
from win_lin import desktop_open

class ribbon_home(QToolBar):
	def __init__(self):
		QToolBar.__init__(self)
		self.setToolButtonStyle( Qt.ToolButtonTextUnderIcon)
		self.setIconSize(QSize(42, 42))
		

		self.undo = QAction(QIcon(os.path.join(get_image_file_path(),"undo.png")), _("Undo"), self)
		self.addAction(self.undo)

		self.addSeparator()

		self.run = QAction(QIcon(os.path.join(get_image_file_path(),"play.png")), _("Run\nsimulation"), self)
		self.addAction(self.run)

		self.stop = QAction(QIcon(os.path.join(get_image_file_path(),"pause.png")), _("Stop\nsimulation"), self)
		self.addAction(self.stop)

		self.addSeparator()
		
		self.scan = QAction(QIcon(os.path.join(get_image_file_path(),"scan.png")), _("Parameter\nscan"), self)
		self.addAction(self.scan)


		#self.addSeparator()
		self.fit = QAction(QIcon(os.path.join(get_image_file_path(),"fit.png")), _("Fit\ndata"), self)
		self.addAction(self.fit)
		self.fit.setVisible(False)
		
		self.addSeparator()
		
		self.plot = QAction(QIcon(os.path.join(get_image_file_path(),"plot.png")), _("Plot\nFile"), self)
		self.plot.triggered.connect(self.callback_plot_select)
		self.addAction(self.plot)

		self.time = QAction(QIcon(os.path.join(get_image_file_path(),"plot_time.png")), _("Examine results\nin time domain"), self)
		self.addAction(self.time)

		self.addSeparator()

		self.sun=tb_item_sun()
		self.addWidget(self.sun)

		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		self.addWidget(spacer)

		self.help = QAction(QIcon(os.path.join(get_image_file_path(),"man.png")), _("Help"), self)
		self.addAction(self.help)

	def update(self):
		print("update")
		
	def setEnabled(self,val):
		self.undo.setEnabled(val)
		self.run.setEnabled(val)
		self.stop.setEnabled(val)
		self.scan.setEnabled(val)
		self.fit.setVisible(val)
		self.plot.setEnabled(val)
		self.time.setEnabled(val)
		self.sun.setEnabled(val)
		self.help.setEnabled(val)
		

	def callback_plot_select(self):
		help_window().help_set_help(["dat_file.png",_("<big>Select a file to plot</big><br>Single clicking shows you the content of the file")])

		dialog=gpvdm_open(os.getcwd())
		dialog.show_inp_files=False
		dialog.show_directories=False
		ret=dialog.window.exec_()
		if ret==QDialog.Accepted:
			split=dialog.get_filename().split(".")
			if len(split)>1:
				if split[1]=="xlsx" or split[1]=="xls":
					desktop_open(dialog.get_filename())
	
					print("open with excel")
					return
				
			if os.path.basename(dialog.get_filename())=="sim_info.dat":
				self.sim_info_window=sim_info(dialog.get_filename())
				self.sim_info_window.show()
				return

			plot_gen([dialog.get_filename()],[],"auto")

			#self.plotted_graphs.refresh()
			#self.plot_after_run_file=dialog.get_filename()
