#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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
import io
import sys
from numpy import *

#matplotlib
import matplotlib
matplotlib.use('Qt5Agg')
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure

import zipfile
from util import lines_to_xyz
from dat_file import dat_file_read
from dat_file_class import dat_file


from matplotlib.figure import Figure
from plot_io import get_plot_file_info

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QSystemTrayIcon,QMenu,QApplication
from PyQt5.QtGui import QIcon,QPixmap,QImage

#epitaxy
from epitaxy import epitaxy_get_layers
from epitaxy import epitaxy_get_mat_file
from epitaxy import epitaxy_get_electrical_layer
from epitaxy import epitaxy_get_width
from epitaxy import epitaxy_get_name

#inp
from inp import inp_load_file
from inp_util import inp_search_token_value
from inp import inp_get_token_value
#calpath
from icon_lib import QIcon_load

from open_save_dlg import save_as_filter
from cal_path import get_sim_path

class band_graph(QWidget):
	def init(self):

		self.main_vbox = QVBoxLayout()

		toolbar=QToolBar()
		toolbar.setIconSize(QSize(32, 32))

		self.tb_save = QAction(QIcon_load("save"), _("Save image"), self)
		self.tb_save.setStatusTip(_("Close"))
		self.tb_save.triggered.connect(self.callback_save_image)
		toolbar.addAction(self.tb_save)

		self.main_vbox.addWidget(toolbar)

		self.my_figure=Figure(figsize=(5,4), dpi=100)
		self.canvas = FigureCanvas(self.my_figure)
		self.canvas.mpl_connect('key_press_event', self.press)
		self.canvas.setFocusPolicy( Qt.ClickFocus )
		self.canvas.setFocus()
		self.canvas.figure.patch.set_facecolor('white')
		#self.canvas.set_size_request(600, 400)
		self.canvas.show()

		self.main_vbox.addWidget(self.canvas)

		#self.canvas.connect('key_press_event', self.on_key_press_event)


		self.setLayout(self.main_vbox)


#	def keyPressEvent(self, event):
#		pritn("oh")
#
#		keyname = ''
#		key = event.key()
#		modifiers = int(event.modifiers())
#		if (Qt.CTRL & modifiers)==modifiers and key==67:
#			self.do_clip()
#			self.canvas.draw()

	def press(self,event):
		#print('press', event.key)
		sys.stdout.flush()
		if event.key == "ctrl+c":
			self.do_clip()


	def do_clip(self):
		buf = io.BytesIO()
		self.my_figure.savefig(buf)
		QApplication.clipboard().setImage(QImage.fromData(buf.getvalue()))
		buf.close()

	def callback_save_image(self):
		response=save_as_filter(self,"png (*.png);;jpg (*.jpg)")
		if response != None:
			print(response)
			self.my_figure.savefig(response)

	def set_data_file(self,file):
		self.optical_mode_file=os.path.join(get_sim_path(),"light_dump",file)

	def draw_graph(self):
		self.layer_end=[]
		self.layer_name=[]

		n=0
		self.my_figure.clf()
		ax1 = self.my_figure.add_subplot(111)
		ax2 = ax1.twinx()
		x_pos=0.0
		layer=0
		color =['r','g','b','y','o','r','g','b','y','o']
		start=0.0

		for i in range(0,epitaxy_get_layers()):
			if epitaxy_get_electrical_layer(i).startswith("dos")==False:
				start=start-epitaxy_get_width(i)
			else:
				break

		start=start*1e9

		x_pos=start
		for i in range(0,epitaxy_get_layers()):

#			label=epitaxy_get_mat_file(i)
			layer_ticknes=epitaxy_get_width(i)
			layer_material=epitaxy_get_mat_file(i)

			delta=float(layer_ticknes)*1e9
			#print(epitaxy_get_electrical_layer(i))
			lines=[]
			material_type=inp_get_token_value(os.path.join(get_sim_path(),'materials',layer_material,'mat.inp'), "#material_type")
			if epitaxy_get_electrical_layer(i).startswith("dos")==False:
				if inp_load_file(lines,os.path.join(get_sim_path(),'materials',layer_material,'dos.inp'))==True:
					lumo=-float(inp_search_token_value(lines, "#Xi"))
					Eg=float(inp_search_token_value(lines, "#Eg"))
			else:

				if inp_load_file(lines,os.path.join(get_sim_path(),epitaxy_get_electrical_layer(i)+".inp"))==True:
					lumo=-float(inp_search_token_value(lines, "#Xi"))
					Eg=float(inp_search_token_value(lines, "#Eg"))

			x = [x_pos,x_pos+delta,x_pos+delta,x_pos]

			lumo_delta=lumo-0.1
			homo=lumo-Eg
			homo_delta=homo-0.1

			if material_type=="oxide" or material_type=="metal":
				lumo_delta=-7.0
				homo=0.0
			lumo_shape = [lumo,lumo,lumo_delta,lumo_delta]
			x_pos=x_pos+delta
			self.layer_end.append(x_pos)
			self.layer_name.append(layer_material)
			ax2.fill(x,lumo_shape, color[layer],alpha=0.4)
			ax2.text(x_pos-delta/1.5, lumo-0.4, epitaxy_get_name(i))

			if homo!=0.0:
				homo_shape = [homo,homo,homo_delta,homo_delta]
				ax2.fill(x,homo_shape, color[layer],alpha=0.4)

			layer=layer+1

			n=n+1

		state=dat_file()
		if dat_file_read(state,self.optical_mode_file)==True:
			print(self.optical_mode_file,os.path.isfile(self.optical_mode_file))
			#summary="<big><b>"+self.store[path[0]][0]+"</b></big>\n"+"\ntitle: "+state.title+"\nx axis: "+state.x_label+" ("+latex_to_pygtk_subscript(state.x_units)+")\ny axis: "++" ("+latex_to_pygtk_subscript(state.y_units)+")\n\n<big><b>Double click to open</b></big>"

			#print("ROD!!!!",state.data_label,self.optical_mode_file)
			ax1.set_ylabel(state.data_label+" ("+state.data_units+")")
			ax1.set_xlabel(_("Position")+" (nm)")
			ax2.set_ylabel(_("Energy")+" (eV)")
			ax2.set_xlim([start, x_pos])
			#ax2.axis(max=)#autoscale(enable=True, axis='x', tight=None)

			for i in range(0,len(state.y_scale)):
				state.y_scale[i]=state.y_scale[i]*1e9
			
			ax1.plot(state.y_scale,state.data[0][0], 'black', linewidth=3 ,alpha=0.5)



		self.my_figure.tight_layout()


