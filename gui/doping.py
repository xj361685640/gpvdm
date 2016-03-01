#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012 Roderick C. I. MacKenzie
#
#	roderick.mackenzie@nottingham.ac.uk
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


import pygtk
pygtk.require('2.0')
import gtk
import sys
import os
import shutil
from numpy import *
from matplotlib.figure import Figure
from numpy import arange, sin, pi
from matplotlib.backends.backend_gtkagg import FigureCanvasGTKAgg as FigureCanvas
from matplotlib.backends.backend_gtkagg import NavigationToolbar2GTKAgg as NavigationToolbar
import gobject
from inp import inp_load_file
from inp import inp_get_token_value
import matplotlib.mlab as mlab
import webbrowser
from inp_util import inp_search_token_value
from cal_path import get_image_file_path
from window_list import windows

from epitaxy import epitaxy_get_dos_files
from tab import tab_class
from epitaxy import epitaxy_get_layers
from epitaxy import epitaxy_get_electrical_layer
from epitaxy import epitaxy_get_pl_file
from epitaxy import epitaxy_get_mat_file
from global_objects import global_object_register
from help import my_help_class
from epitaxy import epitaxy_get_name
from epitaxy import epitaxy_get_dos_file
from epitaxy import epitaxy_get_width
from inp import inp_update

import i18n
_ = i18n.language.gettext

(
SEG_FILE,
SEG_WIDTH,
SEG_START,
SEG_STOP
) = range(4)

mesh_articles = []

class doping_window(gtk.Window):
	lines=[]

	line_number=[]
	save_file_name=""

	file_name=""
	name=""
	visible=1

	def save_data(self):
		print "save"
		#file_name="fxmesh"+str(self.index)+".inp"
		#scan_remove_file(file_name)

		#out_text=[]
		#out_text.append("#fx_start")
		#out_text.append(str(float(self.fx_start)))
		##out_text.append("#fx_segments")
		#out_text.append(str(int(len(self.store))))
		#i=0
		for line in self.store:
			#print line[SEG_FILE],line[SEG_WIDTH],line[SEG_START],line[SEG_STOP]
			inp_update(line[SEG_FILE]+".inp", "#doping_start", str(line[SEG_START]))
			inp_update(line[SEG_FILE]+".inp", "#doping_stop", str(line[SEG_STOP]))




	def update(self):
		self.build_mesh()
		self.draw_graph()
		self.fig.canvas.draw()

	def on_cell_edited_file(self, cell, path, new_text, model):
		#print "Rod",path
		model[path][SEG_FILE] = new_text
		self.build_mesh()
		self.draw_graph()
		self.fig.canvas.draw()
		self.save_data()


	def on_cell_edited_width(self, cell, path, new_text, model):
		model[path][SEG_WIDTH] = new_text
		self.build_mesh()
		self.draw_graph()
		self.fig.canvas.draw()
		self.save_data()


	def on_cell_edited_start(self, cell, path, new_text, model):
		model[path][SEG_START] = new_text
		self.build_mesh()
		self.draw_graph()
		self.fig.canvas.draw()
		self.save_data()

	def on_cell_edited_stop(self, cell, path, new_text, model):
		model[path][SEG_STOP] = new_text
		self.build_mesh()
		self.draw_graph()
		self.fig.canvas.draw()
		self.save_data()

	def gaussian(self,x, mu, sig):
		return exp(-power(x - mu, 2.) / (2 * power(sig, 2.)))

	def draw_graph(self):

		n=0

		self.fig.clf()
		self.fig.subplots_adjust(bottom=0.2)
		self.fig.subplots_adjust(left=0.1)
		self.ax1 = self.fig.add_subplot(111)
		self.ax1.ticklabel_format(useOffset=False)
		#ax2 = ax1.twinx()
		x_pos=0.0
		layer=0
		color =['r','g','b','y','o','r','g','b','y','o']

		self.ax1.set_ylabel(_("Doping (m^{-3})"))
		x_plot=[]
		for i in range(0,len(self.x_pos)):
			x_plot.append(self.x_pos[i]*1e9)


		frequency, = self.ax1.plot(x_plot,self.doping, 'ro-', linewidth=3 ,alpha=1.0)
		self.ax1.set_xlabel(_("Position (nm)"))



	def save_image(self,file_name):
		self.fig.savefig(file_name)

	def callback_save(self, widget, data=None):
		dialog = gtk.FileChooserDialog(_("Save as.."),
                               None,
                               gtk.FILE_CHOOSER_ACTION_SAVE,
                               (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL,
                                gtk.STOCK_SAVE, gtk.RESPONSE_OK))
		dialog.set_default_response(gtk.RESPONSE_OK)

		filter = gtk.FileFilter()
		filter.set_name(".jpg")
		filter.add_pattern("*.jpg")
		dialog.add_filter(filter)

		response = dialog.run()
		if response == gtk.RESPONSE_OK:
			file_name=dialog.get_filename()

			if os.path.splitext(file_name)[1]:
				self.save_image(file_name)
			else:
				filter=dialog.get_filter()
				self.save_image(file_name+filter.get_name())
			
		elif response == gtk.RESPONSE_CANCEL:
		    print _("Closed, no files selected")
		dialog.destroy()

	def callback_help(self, widget, data=None):
		webbrowser.open('http://www.gpvdm.com/man/index.html')

	def create_model(self):
		store = gtk.ListStore(str, str, str,str)

		files=epitaxy_get_dos_files()
		for i in range(0,epitaxy_get_layers()):
			dos_file=epitaxy_get_dos_file(i)
			width=epitaxy_get_width(i)
			if dos_file!="none":
				lines=[]
				print "loading",dos_file
				if inp_load_file(lines,dos_file+".inp")==True:
					doping_start=float(inp_search_token_value(lines, "#doping_start"))
					doping_stop=float(inp_search_token_value(lines, "#doping_stop"))
					doping_width=epitaxy_get_width(i)
					print "add",dos_file
					store.append([str(dos_file), str(width), str(doping_start),str(doping_stop)])
		return store

	def create_columns(self, treeview):


		model=treeview.get_model()
		renderer = gtk.CellRendererText()
		renderer.connect("edited", self.on_cell_edited_file, model)
		renderer.set_property('editable', False)
		column = gtk.TreeViewColumn(_("File Name"), renderer, text=SEG_FILE)
		column.set_sort_column_id(SEG_FILE)
		treeview.append_column(column)

		renderer = gtk.CellRendererText()
		renderer.connect("edited", self.on_cell_edited_width, model)
		column = gtk.TreeViewColumn("Width", renderer, text=SEG_WIDTH)
		renderer.set_property('editable', False)
		column.set_sort_column_id(SEG_WIDTH)
		treeview.append_column(column)

		renderer = gtk.CellRendererText()
		renderer.connect("edited", self.on_cell_edited_start, model)
		renderer.set_property('editable', True)
		column = gtk.TreeViewColumn(_("Start"), renderer, text=SEG_START)
		column.set_sort_column_id(SEG_START)
		treeview.append_column(column)

		renderer = gtk.CellRendererText()
		renderer.connect("edited", self.on_cell_edited_stop, model)
		renderer.set_property('editable', True)
		column = gtk.TreeViewColumn(_("Stop"), renderer, text=SEG_STOP)
		column.set_sort_column_id(SEG_STOP)
		treeview.append_column(column)


	def build_mesh(self):
		lines=[]
		self.doping=[]
		self.x_pos=[]
		pos=0.0
		for line in self.store:
				doping_start=line[SEG_START]
				doping_stop=line[SEG_STOP]
				width=float(line[SEG_WIDTH])
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

	def init(self):
		self.win_list=windows()
		self.set_title(_("Doping profile editor (www.gpvdm.com)"))
		self.set_icon_from_file(os.path.join(get_image_file_path(),"doping.png"))
		self.win_list.set_window(self,"doping")
		self.main_vbox=gtk.VBox()
		self.add(self.main_vbox)

		self.main_vbox.show()
		self.fig = Figure(figsize=(5,4), dpi=100)
		self.ax1=None
		self.show_key=True
		self.hbox=gtk.HBox()
		self.edit_list=[]
		self.line_number=[]
		gui_pos=0

		self.list=[]

		gui_pos=gui_pos+1

		canvas = FigureCanvas(self.fig)  # a gtk.DrawingArea
		#canvas.set_background('white')
		#canvas.set_facecolor('white')
		canvas.figure.patch.set_facecolor('white')
		canvas.set_size_request(500, 150)
		canvas.show()

		tooltips = gtk.Tooltips()

		toolbar = gtk.Toolbar()
		#toolbar.set_orientation(gtk.ORIENTATION_VERTICAL)
		toolbar.set_style(gtk.TOOLBAR_ICONS)
		toolbar.set_size_request(-1, 50)

		self.store = self.create_model()
		treeview = gtk.TreeView(self.store)
		treeview.show()
		tool_bar_pos=0

		save = gtk.ToolButton(gtk.STOCK_SAVE)
		tooltips.set_tip(save, _("Save image"))
		save.connect("clicked", self.callback_save)
		toolbar.insert(save, tool_bar_pos)
		tool_bar_pos=tool_bar_pos+1


		plot_toolbar = NavigationToolbar(self.fig.canvas, self)
		plot_toolbar.show()
		box=gtk.HBox(True, 1)
		box.set_size_request(300,-1)
		box.show()
		box.pack_start(plot_toolbar, True, True, 0)
		tb_comboitem = gtk.ToolItem();
		tb_comboitem.add(box);
		tb_comboitem.show()
		toolbar.insert(tb_comboitem, tool_bar_pos)
		tool_bar_pos=tool_bar_pos+1

		sep = gtk.SeparatorToolItem()
		sep.set_draw(False)
		sep.set_expand(True)
		toolbar.insert(sep, tool_bar_pos)
		sep.show()
		tool_bar_pos=tool_bar_pos+1

		help = gtk.ToolButton(gtk.STOCK_HELP)
		toolbar.insert(help, tool_bar_pos)
		help.connect("clicked", self.callback_help)
		help.show()
		tool_bar_pos=tool_bar_pos+1

		toolbar.show_all()
		self.main_vbox.pack_start(toolbar, True, True, 0)
		tool_bar_pos=tool_bar_pos+1

		canvas.set_size_request(700,400)
		self.main_vbox.pack_start(canvas, True, True, 0)

		treeview.set_rules_hint(True)

		self.create_columns(treeview)

		self.main_vbox.pack_start(treeview, False, False, 0)

		self.statusbar = gtk.Statusbar()
		self.statusbar.show()
		self.main_vbox.pack_start(self.statusbar, False, False, 0)

		self.connect("delete-event", self.callback_close)

		self.build_mesh()
		self.draw_graph()

