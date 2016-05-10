#!/usr/bin/env python2.7
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



import pygtk
pygtk.require('2.0')
import gtk
#import sys
import os
#import shutil
#import commands
from cal_path import get_image_file_path
from search import find_fit_log
from search import find_fit_speed_log
from window_list import windows
from inp import inp_load_file
from inp_util import inp_search_token_value

class hpc_class(gtk.Window):

	file_name=""
	name=""
	visible=1
	enabled=os.path.exists("./hpc.inp")
	cpus=[]

	def callback_node(self, widget, data=None):
		lines=[]
		a = open("../hpc/allowed_nodes", "w")

		if inp_load_file(lines,"./server.inp")==True:
			cpus_per_job=int(inp_search_token_value(lines, "#server_cpus"))
			print "CPUs per job=",cpus_per_job

		a.write(str(cpus_per_job)+"\n")

		for i in range(0, len(self.button)):
			print "cpus=",self.name[i]
			if self.button[i].get_active()==True:
				print "active=",self.name[i]
				a.write(self.name[i]+"\n")
				a.write(str(self.cpus[i])+"\n")
		a.close()

		now_dir=os.getcwd()

		os.chdir("../hpc")
		os.system("./make_node_list.py")

		os.chdir(now_dir)

	def callback_cluster_get_data(self, widget, data=None):
		self.myserver.cluster_get_data()

	def callback_cluster_copy_src(self, widget, data=None):
		self.myserver.copy_src_to_cluster()

	def on_changed(self, widget):
		packet="gpvdmsetmaxloads"
		for i in range(0,len(self.slider)):
			ip = self.ip[i]
			max_cpus = self.slider[i].get_value()
			packet=packet+"\n"+ip+"\n"+str(max_cpus)
		print packet
		self.myserver.send_command(packet)

	def callback_cluster_get_info(self, widget, data=None):
		self.myserver.cluster_get_info()
		self.name=[]
		self.ip=[]
		self.cpus=[]
		self.load=[]
		self.max_cpus=[]
		self.last_seen=[]

		for i in range(0, len(self.myserver.nodes)):
			self.name.append(self.myserver.nodes[i][0])
			self.ip.append(self.myserver.nodes[i][1])
			self.cpus.append(self.myserver.nodes[i][2])
			self.load.append(self.myserver.nodes[i][4])
			self.max_cpus.append(self.myserver.nodes[i][5])
			self.last_seen.append(self.myserver.nodes[i][6])

		if len(self.myserver.nodes)>len(self.button):
			needed=len(self.myserver.nodes)-len(self.button)
			for i in range(0,needed):

				self.button.append(gtk.HBox(False, 0))
				self.button[i].set_size_request(-1, 70)
				self.button[i].show()

				self.bar.append(gtk.ProgressBar())
				self.bar[i].set_size_request(-1, 70)
				self.bar[i].set_orientation(gtk.PROGRESS_LEFT_TO_RIGHT)
				self.bar[i].show()

				self.label.append(gtk.Label("text"))
				self.label[i].show()

				self.slider.append(gtk.HScale())
				self.slider[i].set_range(0, int(self.cpus[i]))
				#self.slider[i].set_increments(1, 100)
				self.slider[i].set_digits(0)
				self.slider[i].set_value(int(self.max_cpus[i]))
				self.slider[i].set_size_request(200, -1)
				self.slider[i].connect("value-changed", self.on_changed)
				self.slider[i].show()

				self.button[i].pack_start(self.label[i], False, False, 3)
				self.button[i].pack_start(self.bar[i], False, False, 3)
				self.button[i].pack_start(self.slider[i], False, False, 3)
				#self.button[i].add(hbox)

				self.prog_hbox.pack_start(self.button[i], False, False, 3)

		if len(self.button)>len(self.myserver.nodes):
			for i in range(len(self.myserver.nodes), len(self.button)):
				self.button[i].hide()

		for i in range(0, len(self.myserver.nodes)):
			self.button[i].show()
			self.label[i].set_text(self.name[i])
			if self.ip[i]!="none":
				self.bar[i].set_text(self.ip[i]+" "+self.load[i]+":"+self.cpus[i]+":seen="+self.last_seen[i])
				if float(self.cpus[i])!=0.0:
					prog=float(self.load[i])/float(self.cpus[i])
				else:
					prog=0.0

				if prog>1.0:
					prog=1.0
				self.bar[i].set_fraction(prog)
			else:
				self.bar[i].set_text("node down?????")

	def callback_cluster_make(self, widget, data=None):
		self.myserver.cluster_make()

	def callback_cluster_clean(self, widget, data=None):
		self.myserver.cluster_clean()

	def callback_cluster_off(self, widget, data=None):
		self.myserver.cluster_quit()
		self.cluster_gui_update()

	def callback_cluster_sleep(self,widget,data):
		self.myserver.sleep()

	def callback_cluster_poweroff(self,widget,data):
		self.myserver.poweroff()

	def callback_cluster_print_jobs(self,widget):
		self.myserver.print_jobs()

	def callback_wol(self, widget, data):
		self.myserver.wake_nodes()

	def callback_cluster_connect(self, widget, data=None):
		if self.myserver.connect()==False:
			md = gtk.MessageDialog(None, gtk.DIALOG_DESTROY_WITH_PARENT, gtk.MESSAGE_INFO, gtk.BUTTONS_CLOSE, "Can not connect to cluster.")
			md.run()
			md.destroy()

		self.cluster_gui_update()

	def cluster_gui_update(self):
		if self.myserver.cluster==True:
			self.cluster_button.set_stock_id(gtk.STOCK_DISCONNECT)
			self.cluster_clean.set_sensitive(True)
			self.cluster_make.set_sensitive(True)
			self.cluster_copy_src.set_sensitive(True)
			self.cluster_get_info.set_sensitive(True)
			self.cluster_get_data.set_sensitive(True)
			self.cluster_off.set_sensitive(True)
		else:
			self.cluster_button.set_stock_id(gtk.STOCK_CONNECT)
			self.cluster_clean.set_sensitive(False)
			self.cluster_make.set_sensitive(False)
			self.cluster_copy_src.set_sensitive(False)
			self.cluster_get_info.set_sensitive(False)
			self.cluster_get_data.set_sensitive(False)
			self.cluster_off.set_sensitive(False)


	def callback_close_window(self, widget, event, data=None):
		self.win_list.update(self,"hpc_window")
		#gtk.main_quit()
		return False



	def init(self, server):
		self.myserver=server
		self.win_list=windows()
		main_box=gtk.VBox()

		self.tooltips = gtk.Tooltips()

		toolbar = gtk.Toolbar()
		toolbar.set_style(gtk.TOOLBAR_ICONS)
		toolbar.set_size_request(-1, 70)
		main_box.pack_start(toolbar, False, False, 0)

		self.cluster_button = gtk.ToolButton(gtk.STOCK_CONNECT)
		self.tooltips.set_tip(self.cluster_button, _("Connect to cluster"))
		self.cluster_button.connect("clicked", self.callback_cluster_connect)
		toolbar.insert(self.cluster_button, -1)

		image = gtk.Image()
   		image.set_from_file(os.path.join(get_image_file_path(),"server_get_data.png"))
		self.cluster_get_data = gtk.ToolButton(image)
		self.cluster_get_data.connect("clicked", self.callback_cluster_get_data)
		self.tooltips.set_tip(self.cluster_get_data, _("Cluster get data"))
		toolbar.insert(self.cluster_get_data, -1)
		self.cluster_get_data.set_sensitive(False)
		self.cluster_get_data.show()

		image = gtk.Image()
   		image.set_from_file(os.path.join(get_image_file_path(),"server_get_info.png"))
		self.cluster_get_info = gtk.ToolButton(image)
		self.cluster_get_info.connect("clicked", self.callback_cluster_get_info)
		self.tooltips.set_tip(self.cluster_get_info, _("Cluster get data"))
		toolbar.insert(self.cluster_get_info, -1)
		self.cluster_get_info.set_sensitive(False)
		self.cluster_get_info.show()

		image = gtk.Image()
   		image.set_from_file(os.path.join(get_image_file_path(),"server_copy_src.png"))
		self.cluster_copy_src = gtk.ToolButton(image)
		self.cluster_copy_src.connect("clicked", self.callback_cluster_copy_src)
		self.tooltips.set_tip(self.cluster_copy_src, _("Copy src to cluster"))
		toolbar.insert(self.cluster_copy_src, -1)
		self.cluster_copy_src.set_sensitive(False)
		self.cluster_copy_src.show()

		image = gtk.Image()
   		image.set_from_file(os.path.join(get_image_file_path(),"server_make.png"))
		self.cluster_make = gtk.ToolButton(image)
		self.cluster_make.connect("clicked", self.callback_cluster_make)
		self.tooltips.set_tip(self.cluster_make, _("Copy src to cluster"))
		self.cluster_make.set_sensitive(False)
		toolbar.insert(self.cluster_make, -1)
		self.cluster_make.show()

		image = gtk.Image()
   		image.set_from_file(os.path.join(get_image_file_path(),"server_clean.png"))
		self.cluster_clean = gtk.ToolButton(image)
		self.cluster_clean.connect("clicked", self.callback_cluster_clean)
		self.tooltips.set_tip(self.cluster_clean, _("Copy src to cluster"))
		self.cluster_clean.set_sensitive(False)
		toolbar.insert(self.cluster_clean, -1)
		self.cluster_clean.show()

		image = gtk.Image()
   		image.set_from_file(os.path.join(get_image_file_path(),"off.png"))
		self.cluster_off = gtk.ToolButton(image)
		self.cluster_off.connect("clicked", self.callback_cluster_off)
		self.tooltips.set_tip(self.cluster_off, _("Copy src to cluster"))
		self.cluster_off.set_sensitive(False)
		toolbar.insert(self.cluster_off, -1)
		self.cluster_off.show()

		vbox_r=gtk.VBox(False, 2)
		vbox_r.show()
		main_box.pack_start(vbox_r, False, False, 0)

		self.prog_hbox=gtk.VBox(False, 2)
		self.prog_hbox.show()
		main_box.pack_start(self.prog_hbox, True, True, 0)
		main_box.show_all()
		self.add(main_box)
		#check load

		self.win_list.set_window(self,"hpc_window")
		self.set_icon_from_file(os.path.join(get_image_file_path(),"server.png"))
		self.set_size_request(700,-1)
		self.set_title("General-purpose Photovoltaic Device Model (www.gpvdm.com)")
		self.connect("delete-event", self.callback_close_window)
		self.bar=[]
		self.button=[]
		self.slider=[]
		self.label=[]

