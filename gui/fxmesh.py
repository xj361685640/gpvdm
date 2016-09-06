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

mesh_articles = []

class tab_fxmesh(QWidget):
	lines=[]
	edit_list=[]

	line_number=[]
	save_file_name=""

	file_name=""
	name=""
	visible=1

	def save_data(self):
		file_name="fxmesh"+str(self.index)+".inp"
		scan_remove_file(file_name)

		out_text=[]
		out_text.append("#fx_start")
		out_text.append(str(float(self.fx_start)))
		out_text.append("#fx_segments")
		out_text.append(str(self.tab.rowCount()))

		for i in range(0,self.tab.rowCount()):
			out_text.append("#fx_segment"+str(i)+"_len")
			scan_item_add(file_name,out_text[len(out_text)-1],_("Part ")+str(i)+_(" period"),1)
			out_text.append(str(tab_get_value(self.tab,i, 0)))

			out_text.append("#fx_segment"+str(i)+"_dfx")
			scan_item_add(file_name,out_text[len(out_text)-1],_("Part ")+str(i)+_(" dfx"),1)
			out_text.append(str(tab_get_value(self.tab,i, 1)))

			out_text.append("#fx_segment"+str(i)+"_mul")
			scan_item_add(file_name,out_text[len(out_text)-1],_("Part ")+str(i)+_(" mul"),1)
			out_text.append(str(tab_get_value(self.tab,i, 2)))


		out_text.append("#ver")
		out_text.append("1.0")
		out_text.append("#end")

		inp_write_lines_to_file(os.path.join(os.getcwd(),file_name),out_text)
		self.update_scan_tokens()

	def update_scan_tokens(self):
		file_name="fxmesh"+str(self.index)+".inp"
		scan_remove_file(file_name)

		for i in range(0,len(self.list)):
			scan_item_add(file_name,"#fx_segment"+str(i)+"_len",_("Part ")+str(i)+_(" period"),1)
			scan_item_add(file_name,"#fx_segment"+str(i)+"_dfx",_("Part ")+str(i)+_(" dfx"),1)
			scan_item_add(file_name,"#fx_segment"+str(i)+"_mul",_("Part ")+str(i)+_(" mul"),1)


	def callback_add_section(self, widget, treeview):

		tab_add(self.tab,["0.0","0.0","0.0"])

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

	def callback_start_fx(self):
		new_fx=dlg_get_text( _("Enter the start frequency of the simulation"), str(self.fx_start))

		if new_fx!=None:
			self.fx_start=float(new_fx)
			self.build_mesh()
			self.draw_graph()
			self.fig.canvas.draw()
			self.save_data()

	def update(self):
		self.build_mesh()
		self.draw_graph()
		self.fig.canvas.draw()

	def draw_graph(self):

#		n=0
		if (len(self.fx)>0):
			mul,unit=fx_with_units(float(self.fx[len(self.fx)-1]-self.fx[0]))
		else:
			mul=1.0
			unit="Hz"

		fx=[]
		for i in range(0,len(self.fx)):
			fx.append(self.fx[i]*mul)

		self.fig.clf()
		self.fig.subplots_adjust(bottom=0.2)
		self.fig.subplots_adjust(left=0.1)
		self.ax1 = self.fig.add_subplot(111)
		self.ax1.ticklabel_format(useOffset=False)

		self.ax1.set_ylabel(_("Magnitude (Volts)"))

		frequency, = self.ax1.plot(fx,self.mag, 'ro-', linewidth=3 ,alpha=1.0)
		self.ax1.set_xlabel(_("Frequency (")+unit+')')

	def save_image(self,file_name):
		self.fig.savefig(file_name)

	def callback_save(self):
		file_name = save_as_jpg(self)
		if file_name !=None:
			self.save_image(file_name)

	def callback_help(self):
		webbrowser.open('http://www.gpvdm.com/man/index.html')

	def create_model(self):
		store = gtk.ListStore(str, str, str)

		for line in self.list:
			store.append([str(line[SEG_LENGTH]), str(line[SEG_DFX]), str(line[SEG_MUL])])

		return store


	def load_data(self):
		self.tab.clear()
		self.tab.setColumnCount(3)
		self.tab.setSelectionBehavior(QAbstractItemView.SelectRows)
		self.tab.setHorizontalHeaderLabels([_("Frequency"), _("dfx"), _("Multiply")])

		lines=[]
		self.start_fx=0.0
		self.list=[]

		file_name="fxmesh"+str(self.index)+".inp"

		ret=inp_load_file(lines,file_name)
		if ret==True:
			if inp_search_token_value(lines, "#ver")=="1.0":
				pos=0
				token,value,pos=inp_read_next_item(lines,pos)
				self.fx_start=float(value)

				token,value,pos=inp_read_next_item(lines,pos)
				segments=int(value)

				for i in range(0, segments):
					token,length,pos=inp_read_next_item(lines,pos)
					token,dfx,pos=inp_read_next_item(lines,pos)
					token,mul,pos=inp_read_next_item(lines,pos)
					self.list.append((length,dfx,mul))

					tab_add(self.tab,[str(length),str(dfx),str(mul)])

				return True
			else:
				print("file "+file_name+"wrong version")
				exit("")
				return False
		else:
			print("file "+file_name+" not found")
			return False

		return False

	def build_mesh(self):
		self.mag=[]
		self.fx=[]
		pos=self.fx_start

		seg=0
		for i in range(0,self.tab.rowCount()):
			end_fx=pos+float(tab_get_value(self.tab,i, 0))
			dfx=float(tab_get_value(self.tab,i, 1))
			mul=float(tab_get_value(self.tab,i, 2))

			if dfx!=0.0 and mul!=0.0:
				while(pos<end_fx):
					self.fx.append(pos)
					self.mag.append(1.0)
					pos=pos+dfx

					dfx=dfx*mul

			seg=seg+1


		#self.statusbar.push(0, str(len(self.fx))+_(" mesh points"))

	def on_cell_edited(self, x,y):
		print("Cell edited",x,y)
		self.build_mesh()
		self.draw_graph()
		self.fig.canvas.draw()
		self.save_data()

	def __init__(self,index):
		QWidget.__init__(self)

		self.index=index
		self.ax1=None
		self.show_key=True
		self.edit_list=[]
		self.line_number=[]
		self.list=[]

		self.fig = Figure(figsize=(5,4), dpi=100)
		self.canvas = FigureCanvas(self.fig)
		self.canvas.figure.patch.set_facecolor('white')

		gui_pos=0

		print("index=",index)

		self.main_vbox = QVBoxLayout()

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(48, 48))

		self.tb_save = QAction(QIcon(os.path.join(get_image_file_path(),"32_save.png")), _("Save image"), self)
		self.tb_save.triggered.connect(self.callback_save)
		toolbar.addAction(self.tb_save)

		self.tb_startfx = QAction(QIcon(os.path.join(get_image_file_path(),"start.png")), _("Simulation start frequency"), self)
		self.tb_startfx.triggered.connect(self.callback_start_fx)
		toolbar.addAction(self.tb_startfx)


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

		self.load_data()
		self.build_mesh()
		self.draw_graph()

		self.update_scan_tokens()

		self.tab.cellChanged.connect(self.on_cell_edited)

		self.main_vbox.addWidget(self.tab)

		self.setLayout(self.main_vbox)

		return




