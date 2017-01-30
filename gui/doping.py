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
from numpy import *
import webbrowser
from cal_path import get_image_file_path
from window_list import windows

#inp
from inp import inp_update
from inp import inp_load_file
from inp_util import inp_search_token_value

import i18n
_ = i18n.language.gettext

#epitaxy
from epitaxy import epitaxy_get_layers
from epitaxy import epitaxy_get_dos_file
from epitaxy import epitaxy_get_width

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QTableWidget,QTableWidgetItem,QAbstractItemView
from PyQt5.QtGui import QPainter,QIcon

#matplotlib
import matplotlib
matplotlib.use('Qt5Agg')
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

from open_save_dlg import save_as_gpvdm

class doping_window(QWidget):
	lines=[]


	def save_data(self):
		print("save")
		for i in range(0,self.tab.rowCount()):
			inp_update(self.tab.item(i, 0).text()+".inp", "#doping_start", self.tab.item(i, 2).text())
			inp_update(self.tab.item(i, 0).text()+".inp", "#doping_stop", self.tab.item(i, 3).text())




	def update(self):
		self.build_mesh()
		self.draw_graph()
		self.fig.canvas.draw()


	def draw_graph(self):

#		n=0

		self.fig.clf()
		self.fig.subplots_adjust(bottom=0.2)
		self.fig.subplots_adjust(left=0.1)
		self.ax1 = self.fig.add_subplot(111)
		self.ax1.ticklabel_format(useOffset=False)
		#ax2 = ax1.twinx()
#		x_pos=0.0
#		layer=0
#		color =['r','g','b','y','o','r','g','b','y','o']

		self.ax1.set_ylabel(_("Doping (m^{-3})"))
		x_plot=[]
		for i in range(0,len(self.x_pos)):
			x_plot.append(self.x_pos[i]*1e9)


		frequency, = self.ax1.plot(x_plot,self.doping, 'ro-', linewidth=3 ,alpha=1.0)
		self.ax1.set_xlabel(_("Position (nm)"))



	def save_image(self,file_name):
		self.fig.savefig(file_name)

	def callback_save(self, widget, data=None):
		file_name=save_as_gpvdm(self)
		if file_name!=False:

			if os.path.splitext(file_name)[1]:
				self.save_image(file_name)
			else:
				filter=dialog.get_filter()
				self.save_image(file_name+".png")


	def callback_help(self):
		webbrowser.open("https://www.gpvdm.com/man/index.html")

	def load(self):
		self.tab.blockSignals(True)
		self.tab.clear()
		self.tab.setHorizontalHeaderLabels([_("File Name"), _("Width"), _("Start"), _("Stop")])
		layers=epitaxy_get_layers()

		for i in range(0,layers):
			dos_file=epitaxy_get_dos_file(i)
			width=epitaxy_get_width(i)
			if dos_file!="none":
				lines=[]
				print("loading",dos_file)
				if inp_load_file(lines,dos_file+".inp")==True:
					doping_start=float(inp_search_token_value(lines, "#doping_start"))
					doping_stop=float(inp_search_token_value(lines, "#doping_stop"))

					print("add",dos_file)

					count=self.tab.rowCount()
					self.tab.insertRow(count)

					item1 = QTableWidgetItem(str(dos_file))
					self.tab.setItem(count,0,item1)

					item2 = QTableWidgetItem(str(width))
					self.tab.setItem(count,1,item2)

					item3 = QTableWidgetItem(str(doping_start))
					self.tab.setItem(count,2,item3)

					item3 = QTableWidgetItem(str(doping_stop))
					self.tab.setItem(count,3,item3)

		self.tab.blockSignals(False)

		return

	def build_mesh(self):
#		lines=[]
		self.doping=[]
		self.x_pos=[]
		pos=0.0
		for i in range(0,self.tab.rowCount()):
			print(i)
			doping_start=float(self.tab.item(i, 2).text())
			doping_stop=float(self.tab.item(i, 3).text())
			width=float(self.tab.item(i, 1).text())
			self.doping.append(doping_start)
			self.x_pos.append(pos)
			pos=pos+width
			self.doping.append(doping_stop)
			self.x_pos.append(pos)

		return True

	def callback_close(self, widget, data=None):
		self.win_list.update(self,"doping")
		self.hide()
		return True

	def tab_changed(self, x,y):
		self.build_mesh()
		self.draw_graph()
		self.fig.canvas.draw()
		self.save_data()


	def __init__(self):
		QWidget.__init__(self)
		self.win_list=windows()
		self.setFixedSize(900, 600)
		self.setWindowIcon(QIcon(os.path.join(get_image_file_path(),"doping.png")))
		self.setWindowTitle(_("Doping profile editor")+" (https://www.gpvdm.com)") 

		self.win_list.set_window(self,"doping")
		self.main_vbox=QVBoxLayout()

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))

		self.save = QAction(QIcon(os.path.join(get_image_file_path(),"save.png")), _("Save"), self)
		self.save.triggered.connect(self.callback_save)
		toolbar.addAction(self.save)

		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		toolbar.addWidget(spacer)


		self.help = QAction(QIcon(os.path.join(get_image_file_path(),"help.png")), _("Help"), self)
		self.help.triggered.connect(self.callback_help)
		toolbar.addAction(self.help)

		self.main_vbox.addWidget(toolbar)

		self.fig = Figure(figsize=(5,4), dpi=100)
		self.ax1=None
		self.show_key=True
		canvas = FigureCanvas(self.fig)
		#canvas.set_background('white')
		#canvas.set_facecolor('white')
		canvas.figure.patch.set_facecolor('white')
		canvas.show()

		self.main_vbox.addWidget(canvas)

		self.tab = QTableWidget()
		self.tab.resizeColumnsToContents()

		self.tab.verticalHeader().setVisible(False)

		self.tab.clear()
		self.tab.setColumnCount(4)
		self.tab.setSelectionBehavior(QAbstractItemView.SelectRows)

		self.load()
		self.build_mesh()

		self.tab.cellChanged.connect(self.tab_changed)

		self.main_vbox.addWidget(self.tab)


		self.draw_graph()

		self.setLayout(self.main_vbox)
		return


