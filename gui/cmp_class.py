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
from inp import inp_update_token_value
from inp import inp_write_lines_to_file
from inp import inp_load_file
from inp_util import inp_search_token_value
from plot_widget import plot_widget
from window_list import windows
from dat_file_class import dat_file
from plot_io import plot_load_info
from cal_path import get_exe_command
from icon_lib import QIcon_load
import webbrowser

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QHBoxLayout,QLabel,QComboBox
from PyQt5.QtGui import QPainter,QIcon

from snapshot_slider import snapshot_slider

from open_save_dlg import save_as_filter

from PyQt5.QtWidgets import QApplication

from help import help_window

class cmp_class(QWidget):
	mix_y=None
	max_y=None
	max_z=1e24

	def check_2d_file(self,name):
		mapfiles=["pt_map","nt_map"]
		filename=os.path.basename(name)
		for i in range(len(filename)-1,0,-1):
			if filename[i]=="_":
				break
		data=filename[:i]
		count=mapfiles.count(data)
		if count==1:
			return True
		else:
			return False

	def callback_close(self, widget, data=None):
		self.win_list.update(self,"cmp_class")
		return False

	def do_clip(self):

		snap = self.canvas.get_snapshot()
		pixbuf = gtk.gdk.pixbuf_get_from_drawable(None, snap, snap.get_colormap(),0,0,0,0,snap.get_size()[0], snap.get_size()[1])
		clip = gtk.Clipboard()
		clip.set_image(pixbuf)


	def on_key_press_event(self,widget, event):
		#keyname = gtk.gdk.keyval_name(event.keyval)
		if gtk.gdk.keyval_name(event.keyval) == "c":
			if event.state == gtk.gdk.CONTROL_MASK:
				self.do_clip()

	def update(self):
		file_name=self.slider.get_file_name()
		self.plot.set_labels(["data"])
		config_file=os.path.splitext(file_name)[0]+".oplot"
		self.plot.load_data([file_name],config_file)
		self.plot.do_plot()

		return
		files=self.entry2.get_text().split()
		value=int(value)
		print("hello")

		if value>self.dumps:
			return


		path0=self.entry0.get_active_text()
		path1=self.entry1.get_active_text()
		self.file_names=[]
		labels=[]
		zero_frame=[]

		#title=self.lines[value].split()
		#mul,unit=time_with_units(float(title[1]))
		#self.plot.plot_title="Voltage="+title[0]+" time="+str(float(title[1])*mul)+" "+unit

		for i in range(0,len(files)):
			self.file_names.append(os.path.join(path0,str(int(value)),files[i]+".dat"))
			zero_frame.append(os.path.join(path0,"0",files[i]+".dat"))
			labels.append(files[i])

			self.file_names.append(os.path.join(path1,str(int(value)),files[i]+".dat"))
			zero_frame.append(os.path.join(path1,"0",files[i]+".dat"))
			labels.append("")

		plot_id=[]
		if self.multi_plot==False:
			for i in range(0,len(self.file_names)):
				plot_id.append(0)
		else:
			for i in range(0,len(self.file_names)):
				plot_id.append(i)

		exp_files=self.entry3.get_text().split()
		for i in range(0,len(exp_files)):
			self.file_names.append(exp_files[i])
			zero_frame.append("")
			labels.append("")
			plot_id.append(i)
		self.plot.zero_frame_list=zero_frame

		print("hi",self.file_names)
		print(plot_id)




	def callback_scale(self, adj):
		self.update(self.adj1.value)

		if plot_load_info(self.plot_token,self.file_names[0])==True:
			self.plot.do_plot()


	def callback_edit(self,data):
		lines=[]
		lines.append("#entry0")
		lines.append(self.entry0.get_active_text())
		lines.append("#entry1")
		lines.append(self.entry1.get_active_text())
		lines.append("#entry2")
		lines.append(self.entry2.get_text())
		lines.append("#entry3")
		lines.append(self.entry3.get_text())
		inp_write_lines_to_file("gui_cmp_config.inp",lines)
		self.plot.gen_colors(2)
		self.count_dumps()

	def config_load(self):
		lines=[]
		if inp_load_file(lines,"gui_cmp_config.inp")==True:

			if self.snapshot_list.count(inp_search_token_value(lines, "#entry0"))!=0:
				self.entry0.set_active(self.snapshot_list.index(inp_search_token_value(lines, "#entry0")))
			else:
				self.entry0.set_active(0)

			if self.snapshot_list.count(inp_search_token_value(lines, "#entry1"))!=0:
				self.entry1.set_active(self.snapshot_list.index(inp_search_token_value(lines, "#entry1")))
			else:
				self.entry1.set_active(0)

			self.entry2.set_text(inp_search_token_value(lines, "#entry2"))
			self.entry3.set_text(inp_search_token_value(lines, "#entry3"))

		else:
			self.entry0.set_active(0)
			self.entry1.set_active(0)
			self.entry2.set_text("n p")
			self.entry3.set_text("")

	def save_image(self,file_name):
		file_ext="jpg"
		types=self.plot.fig.canvas.get_supported_filetypes()
		if "jpg" in types:
			file_ext="jpg"
		elif "png" in  types:
			file_ext="png"

		dir_name, ext = os.path.splitext(file_name)

		if (ext=="."+file_ext):
			self.plot.fig.savefig(file_name)
		elif ext==".avi":

			if os.path.isdir(dir_name)==False:
				os.mkdir(dir_name)

			jpgs=""
			for i in range(0,self.slider.slider_max):
				self.slider.slider0.setValue(i)
				QApplication.processEvents()
				self.update()
				self.plot.do_plot()
				image_name=os.path.join(dir_name,"image_"+str(i)+"."+file_ext)
				print(image_name)
				self.plot.fig.savefig(image_name)
				jpgs=jpgs+" mf://"+image_name

			os.system("mencoder "+jpgs+" -mf type="+file_ext+":fps=1.0 -o "+file_name+" -ovc lavc -lavcopts vcodec=mpeg1video:vbitrate=800")
			#msmpeg4v2
		else:
			print("Unknown file extension")

	def callback_save(self, widget, data=None):
		file_name=save_as_filter(self,"avi (*.avi)")
		if file_name!=None:
			self.save_image(file_name)

	def callback_help(self, widget, data=None):
		webbrowser.open('http://www.gpvdm.com/man/gpvdm.html')


	def callback_toggle_subtract(self, widget, data):
		self.plot.zero_frame_enable=data.get_active()
		self.update(self.adj1.value)
		print("CONVERTh!!!!!!!!!!!",type(self.plot.plot_token.key_units))
		self.plot.do_plot()

	def callback_multi_plot(self, data, widget):
		if widget.get_active()==True:
			self.multi_plot=True
		else:
			self.multi_plot=False
		self.update(self.adj1.value)
		self.plot.do_plot()

	def find_snapshots(self):

		matches = []
		for root, dirnames, filenames in os.walk(os.getcwd()):
			for filename in filenames:
				my_file=os.path.join(root,filename)
				if my_file.endswith("snapshots.inp")==True:
					matches.append( os.path.dirname(my_file))

		return matches

	def callback_snapshots_combobox(self):
		self.slider.set_path(self.snapshots_combobox.currentText())
		
	def __init__(self):
		QWidget.__init__(self)
		self.setWindowTitle(_("Examine simulation results in time domain")) 

		self.snapshots_hbox = QHBoxLayout()
		self.snapshots_label= QLabel("Snapshots")
		self.snapshots_hbox.addWidget(self.snapshots_label)
		self.snapshots_combobox=QComboBox()
		self.snapshots_hbox.addWidget(self.snapshots_combobox)
		self.snapshots_widget=QWidget()
		self.snapshot_dirs=self.find_snapshots()
		for i in range(0,len(self.snapshot_dirs)):
			self.snapshots_combobox.addItem(self.snapshot_dirs[i])

		self.snapshots_combobox.currentIndexChanged.connect(self.callback_snapshots_combobox)

		self.snapshots_widget.setLayout(self.snapshots_hbox)
		
		self.main_vbox = QVBoxLayout()

		self.slider=snapshot_slider()
		self.slider.set_path(os.path.join(os.getcwd(),"snapshots"))
		self.slider.changed.connect(self.update)
		self.plot=plot_widget()
		self.plot.init()
		#Toolbar
		toolbar=QToolBar()
		toolbar.setIconSize(QSize(42, 42))

		self.tb_video = QAction(QIcon_load("video"), _("Save video"), self)
		self.tb_video.triggered.connect(self.callback_save)
		toolbar.addAction(self.tb_video)

		#self.tb_scale = QAction(QIcon_load("scale"), _("Scale"), self)
		#self.tb_scale.triggered.connect(self.callback_scale)
		#toolbar.addAction(self.tb_rotate)


		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		toolbar.addWidget(spacer)


		self.help = QAction(QIcon_load("help"), _("Help"), self)
		self.help.setStatusTip(_("Close"))
		self.help.triggered.connect(self.callback_help)
		toolbar.addAction(self.help)
##############################################
		self.main_vbox.addWidget(toolbar)

		self.main_vbox.addWidget(self.plot)

		self.main_vbox.addWidget(self.snapshots_widget)
		
		self.main_vbox.addWidget(self.slider)

		self.setLayout(self.main_vbox)

		self.win_list=windows()
		self.win_list.load()
		self.win_list.set_window(self,"cmp_class")

		if os.path.isfile(os.path.join(os.getcwd(),"snapshots","0","Ec.dat"))==False:
			help_window().help_append(["warning.png",_("No electrical slice data has been stored in the snapshots directory.  To turn this on set Simulation->Configure->Dump->Dump 1D Slices to on.  This will dump a lot of data and slow down your simulations.")])
		
		#self.light.currentIndexChanged.connect(self.call_back_light_changed)

	def init(self):
		return False
		self.dumps=0
		self.plot_token=dat_file()

		vbox=gtk.VBox()

		self.multi_plot=False

		self.log_scale_y="auto"


		menu_items = (
		    ( "/_Options",         None,         None, 0, "<Branch>" ),
		    ( "/Options/_Subtract 0th frame",     None, self.callback_toggle_subtract, 0, "<ToggleItem>", "gtk-save" ),
		    ( "/_Axis/_Multiplot",     None, self.callback_multi_plot, 0, "<ToggleItem>", "gtk-save" ),
		    ( "/_Axis/_Set y axis to maximum",     None, self.callback_set_min_max, 0, "<ToggleItem>", "gtk-save" ),
		    )

		self.plot.item_factory.create_items(menu_items)

		primary_hbox.add(self.entry0)
		sim_vbox.add(primary_hbox)

		secondary_hbox=gtk.HBox()

		text=gtk.Label("Secondary dir")
		secondary_hbox.add(text)

		self.entry1 = gtk.combo_box_entry_new_text()
		self.entry1.show()

		for i in range(0,len(self.snapshot_list)):
			self.entry1.append_text(self.snapshot_list[i])

		secondary_hbox.add(self.entry1)
		sim_vbox.add(secondary_hbox)

		sim_vbox.show()
		#hbox.set_size_request(-1, 30)
		vbox.pack_start(sim_vbox, False, False, 0)

		hbox2=gtk.HBox()
		text=gtk.Label("Files to plot")
		hbox2.add(text)
		self.entry2 = gtk.Entry()
		self.entry2.set_text("pt_map nt_map")
		self.entry2.show()
		hbox2.add(self.entry2)
		hbox2.set_size_request(-1, 30)
		vbox.pack_start(hbox2, False, False, 0)

		hbox3=gtk.HBox()
		text=gtk.Label("Exprimental data")
		hbox3.add(text)
		self.entry3 = gtk.Entry()
		self.entry3.set_text("")
		self.entry3.show()
		hbox3.add(self.entry3)
		hbox3.set_size_request(-1, 30)
		vbox.pack_start(hbox3, False, False, 0)

		self.update_button = gtk.Button()
		self.update_button.set_label("Update")
		self.update_button.show()
		self.update_button.connect("clicked", self.callback_scale)
		vbox.add(self.update_button)

		self.config_load()
		self.count_dumps()



		if self.dumps==0:
			md = gtk.MessageDialog(None, 0, gtk.MESSAGE_QUESTION,  gtk.BUTTONS_YES_NO, "No slice data has been written to disk.  You need to re-run the simulation with the dump_slices set to 1.  Would you like to do this now?  Note: This generates lots of files and will slow down the simulation.")

			response = md.run()

			if response == gtk.RESPONSE_YES:
				inp_update_token_value("dump.inp", "#dump_1d_slices", "1",1)
				os.system(get_exe_command())

			md.destroy()

			self.count_dumps()

		self.entry0.connect("changed", self.callback_edit)
		self.entry1.connect("changed", self.callback_edit)
		self.entry2.connect("changed", self.callback_edit)
		self.entry3.connect("changed", self.callback_edit)



		vbox.show_all()
		self.add(vbox)

		self.update(0)
		if self.dumps!=0:
			self.plot.do_plot()
			print("CONVERT!!!!!!!!!!!",type(self.plot.plot_token.key_units))
		self.set_border_width(10)
		self.set_title("Compare")
		self.set_icon_from_file(os.path.join(get_image_file_path(),"image.jpg"))

		self.connect('key_press_event', self.on_key_press_event)

		self.show()


