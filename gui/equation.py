#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
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
import sys
from scan_item import scan_item_add
from gui_util import dlg_get_text
from inp import inp_write_lines_to_file
import webbrowser
from util import fx_with_units
from cal_path import get_image_file_path
from scan_item import scan_remove_file

import i18n
_ = i18n.language.gettext

#inp
from inp_util import inp_search_token_value
from inp import inp_load_file
from inp import inp_read_next_item

#matplotlib
import matplotlib
matplotlib.use('Qt5Agg')
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QTableWidget,QAbstractItemView
from PyQt5.QtGui import QPainter,QIcon

#windows
from gui_util import tab_add
from gui_util import tab_move_down
from gui_util import tab_remove
from gui_util import tab_get_value
from gui_util import save_as_jpg

from dat_file import dat_file
from dat_file import dat_file_read

from ref import ref

mesh_articles = []

class equation(QWidget):


	def save_data(self):

		out_text=[]
		out_text.append("#points")
		out_text.append(str(self.points))
		out_text.append("#equations")
		out_text.append(str(self.tab.rowCount()))

		for i in range(0,self.tab.rowCount()):
			out_text.append("#start"+str(i))
			out_text.append(str(tab_get_value(self.tab,i, 0)))

			out_text.append("#stop"+str(i))
			out_text.append(str(tab_get_value(self.tab,i, 1)))

			out_text.append("#equation"+str(i))
			out_text.append(str(tab_get_value(self.tab,i, 2)))

		out_text.append("#ver")
		out_text.append("1.0")
		out_text.append("#end")
		
		dump=""
		for item in out_text:
			dump=dump+item+"\n"

		dump=dump.rstrip("\n")

		f=open(os.path.join(self.path,self.file_name), mode='wb')
		lines = f.write(str.encode(dump))
		f.close()


	def callback_add_section(self):

		tab_add(self.tab,["100e-9","1000e-9",self.default_value])

		self.build_mesh()
		self.draw_graph()
		self.fig.canvas.draw()
		self.save_data()

	def callback_remove_item(self):
		tab_remove(self.tab)

		self.build_mesh()

		self.draw_graph()
		self.fig.canvas.draw()
		self.save_data()

	def callback_move_down(self):

		tab_move_down(self.tab)

		self.build_mesh()
		self.draw_graph()
		self.fig.canvas.draw()
		self.save_data()


	def update(self):
		self.build_mesh()
		self.draw_graph()
		self.fig.canvas.draw()

	def draw_graph(self):
		self.fig.clf()
		self.fig.subplots_adjust(bottom=0.2)
		self.fig.subplots_adjust(left=0.1)
		self.ax1 = self.fig.add_subplot(111)
		self.ax1.ticklabel_format(useOffset=False)

		self.ax1.set_ylabel(self.ylabel)

		x_nm= [x * 1e9 for x in self.x]
		frequency, = self.ax1.plot(x_nm,self.y, 'ro-', linewidth=3 ,alpha=1.0)

		if os.path.isfile(os.path.join(self.path,self.exp_file))==True:
			data=dat_file()
			dat_file_read(data,os.path.join(self.path,self.exp_file))
			x_nm= [x * 1e9 for x in data.y_scale]
			frequency, = self.ax1.plot(x_nm,data.data[0][0], 'bo-', linewidth=3 ,alpha=1.0)

		self.ax1.set_xlabel(_("Wavelength (nm)"))

	def save_image(self,file_name):
		self.fig.savefig(file_name)

	def callback_save(self):
		file_name = save_as_jpg(self)
		if file_name !=None:
			self.save_image(file_name)

	def callback_help(self):
		webbrowser.open('http://www.gpvdm.com/man/index.html')

	def load_data(self):
		self.tab.clear()
		self.tab.setColumnCount(3)
		self.tab.setSelectionBehavior(QAbstractItemView.SelectRows)
		self.tab.setHorizontalHeaderLabels([_("start (m)"), _("stop (m)"), _("Python Equation")])
		self.tab.setColumnWidth(2, 500)
		lines=[]
		pos=0
		if inp_load_file(lines,os.path.join(self.path,self.file_name))==True:
			token,self.points,pos=inp_read_next_item(lines,pos)		
			token,equations,pos=inp_read_next_item(lines,pos)
			equations=int(equations)
			self.points=int(self.points)
			for i in range(0, equations):
				token,start,pos=inp_read_next_item(lines,pos)
				token,stop,pos=inp_read_next_item(lines,pos)
				token,equation,pos=inp_read_next_item(lines,pos)
				tab_add(self.tab,[str(start),str(stop),str(equation)])

	def build_mesh(self):
		self.x=[]
		self.y=[]

		data_min=100.0
		if self.tab.rowCount()!=0:
			for i in range(0,self.tab.rowCount()):
				val=float(tab_get_value(self.tab,i, 0))
				if val<data_min:
					data_min=val

			#find max
			data_max=0.0
			for i in range(0,self.tab.rowCount()):
				val=float(tab_get_value(self.tab,i, 1))
				if val>data_max:
					data_max=val

			w=data_min
			dx=(data_max-data_min)/(float(self.points))

			for i in range(0,self.points):
				val=0.0
				for ii in range(0,self.tab.rowCount()):
					range_min=float(tab_get_value(self.tab,ii, 0))
					range_max=float(tab_get_value(self.tab,ii, 1))
					command=tab_get_value(self.tab,ii, 2)
					try:
						equ=eval(command)
					except:
						print("error evaluating command ", sys.exc_info())
						equ=-1
					
					if w>=range_min and w <=range_max:
						val=val+equ
				if val<0.0:
					val=1.0
					
				self.x.append(w)
				self.y.append(val)
				w=w+dx
			
			a = open(os.path.join(self.path,self.out_file), "w")
			for i in range(0,len(self.y)):
				a.write(str(self.x[i])+" "+str(self.y[i])+"\n")
			a.close()

	def on_cell_edited(self, x,y):
		self.build_mesh()
		self.draw_graph()
		self.fig.canvas.draw()
		self.save_data()

	def set_default_value(self,value):
		self.default_value=value
	
	def set_ylabel(self,value):
		self.ylabel=value

	def callback_ref(self):
		a=ref(os.path.join(self.path,self.file_name),"#root")
		a.run()
		
	def init(self):
		self.fig = Figure(figsize=(5,4), dpi=100)
		self.canvas = FigureCanvas(self.fig)
		self.canvas.figure.patch.set_facecolor('white')

		self.main_vbox = QVBoxLayout()

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))

		self.tb_save = QAction(QIcon(os.path.join(get_image_file_path(),"32_save.png")), _("Save image"), self)
		self.tb_save.triggered.connect(self.callback_save)
		toolbar.addAction(self.tb_save)

		self.tb_ref= QAction(QIcon(os.path.join(get_image_file_path(),"32_ref.png")), _("Insert reference information"), self)
		self.tb_ref.triggered.connect(self.callback_ref)
		toolbar.addAction(self.tb_ref)
		
		

		self.main_vbox.addWidget(toolbar)


		self.main_vbox.addWidget(self.canvas)

		#toolbar 2

		toolbar2=QToolBar()
		toolbar2.setIconSize(QSize(48, 48))

		self.tb_add = QAction(QIcon(os.path.join(get_image_file_path(),"add.png")), _("Add section"), self)
		self.tb_add.triggered.connect(self.callback_add_section)
		toolbar2.addAction(self.tb_add)

		self.tb_remove = QAction(QIcon(os.path.join(get_image_file_path(),"minus.png")), _("Delete section"), self)
		self.tb_remove.triggered.connect(self.callback_remove_item)
		toolbar2.addAction(self.tb_remove)

		self.tb_move = QAction(QIcon(os.path.join(get_image_file_path(),"down.png")), _("Move down"), self)
		self.tb_move.triggered.connect(self.callback_move_down)
		toolbar2.addAction(self.tb_move)

		self.main_vbox.addWidget(toolbar2)

		self.tab = QTableWidget()
		self.tab.resizeColumnsToContents()

		self.tab.verticalHeader().setVisible(False)

		self.main_vbox.addWidget(self.tab)

		self.setLayout(self.main_vbox)

		self.load_data()

		self.build_mesh()

		self.draw_graph()

		self.tab.cellChanged.connect(self.on_cell_edited)
		
	def __init__(self,path,file_name,out_file,exp_file):
		QWidget.__init__(self)
		self.points=200
		self.default_value="3.0"
		self.path=path
		self.file_name=file_name
		self.out_file=out_file
		self.exp_file=exp_file





