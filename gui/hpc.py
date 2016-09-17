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
from cal_path import get_image_file_path
from search import find_fit_log
from search import find_fit_speed_log
from window_list import windows
from inp import inp_load_file
from inp_util import inp_search_token_value
from status_icon import status_icon_stop
from jobs import jobs_view

#qt
from PyQt5.QtWidgets import QMainWindow, QTextEdit, QAction, QApplication
from PyQt5.QtGui import QIcon
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QSizePolicy,QHBoxLayout,QPushButton,QDialog,QFileDialog,QToolBar,QVBoxLayout,QTabWidget
from about import about_dlg
from gui_util import error_dlg

class hpc_class(QToolBar):

	name=""
	cpus=[]

	def callback_cluster_view_button(self, widget, data=None):

		if self.hpc_window.get_property("visible")==True:
			self.hpc_window.hide()
		else:
			self.hpc_window.show()

	def callback_cluster_get_data(self, widget, data=None):
		self.myserver.cluster_get_data()

	def callback_cluster_copy_src(self, widget, data=None):
		self.myserver.copy_src_to_cluster()

	def on_changed(self, widget):
		ip=[]
		loads=[]

		for i in range(0,len(self.slider)):
			ip.append(self.ip[i])
			loads.append(self.slider[i].get_value())
		self.myserver.set_cluster_loads(ip,loads)

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

	def callback_cluster_make(self):
		self.myserver.cluster_make()

	def callback_cluster_stop(self):
		self.myserver.killall()

	def callback_cluster_clean(self):
		self.myserver.cluster_clean()

	def callback_cluster_off(self):
		self.myserver.cluster_quit()
		self.cluster_gui_update()

	def callback_cluster_sync(self):
		self.myserver.copy_src_to_cluster_fast()


	def callback_cluster_jobs(self):
		self.myserver.cluster_list_jobs()

	def callback_cluster_sleep(self):
		self.myserver.sleep()

	def callback_cluster_poweroff(self):
		self.myserver.poweroff()

	def callback_cluster_print_jobs(self):
		self.myserver.print_jobs()

	def callback_wol(self, widget, data):
		self.myserver.wake_nodes()

	def callback_cluster_connect(self):
		if self.myserver.connect()==False:
			error_dlg(self,_("Can not connect to cluster."))

		self.cluster_gui_update()
		if self.myserver.cluster==True:
			status_icon_stop(True)
		else:
			status_icon_stop(False)

	def cluster_gui_update(self):
		if self.myserver.cluster==True:
			self.cluster_button.setIcon(QIcon(os.path.join(get_image_file_path(),"connected.png")))
			self.cluster_clean.setEnabled(True)
			self.cluster_make.setEnabled(True)
			self.cluster_copy_src.setEnabled(True)
			self.cluster_get_info.setEnabled(True)
			self.cluster_get_data.setEnabled(True)
			self.cluster_off.setEnabled(True)
			self.cluster_sync.setEnabled(True)
			self.cluster_jobs.setEnabled(True)
			self.cluster_stop.setEnabled(True)
			self.cluster_view_button.setEnabled(True)

		else:
			self.cluster_button.setIcon(QIcon(os.path.join(get_image_file_path(),"not_connected.png")))
			self.cluster_clean.setEnabled(False)
			self.cluster_make.setEnabled(False)
			self.cluster_copy_src.setEnabled(False)
			self.cluster_get_info.setEnabled(False)
			self.cluster_get_data.setEnabled(False)
			self.cluster_off.setEnabled(False)
			self.cluster_sync.setEnabled(False)
			self.cluster_jobs.setEnabled(False)
			self.cluster_stop.setEnabled(False)
			self.cluster_view_button.setEnabled(False)

	def callback_close_window(self, widget, event, data=None):
		self.win_list.update(self.hpc_window,"hpc_window")
		#gtk.main_quit()
		return False


	def init_job_window(self):
		self.status_window=QWidget()
		self.status_window.setFixedSize(900, 600)
		self.status_window.setWindowIcon(QIcon(os.path.join(get_image_file_path(),"connected.png")))
		self.status_window.setWindowTitle(_("Steady state simulation (www.gpvdm.com)")) 
		
		self.main_vbox = QVBoxLayout()
		
		self.notebook = QTabWidget()

		self.notebook.setTabsClosable(True)
		self.notebook.setMovable(True)

		self.main_vbox.addWidget(self.notebook)


		#self.notebook = gtk.Notebook()
		#self.notebook.set_tab_pos(gtk.POS_TOP)
		#self.notebook.show()
		#label = gtk.Label("Nodes")
		#self.notebook.append_page(self.prog_hbox, label)


		self.jview=jobs_view()
		self.jview.init(self.myserver.jobs_list)
		tab.init(files[i],description[i])
		self.notebook.addTab(self.jview,"Jobs list")

		self.setLayout(self.main_vbox)

		self.win_list.set_window(self.status_window,"hpc_window")
		self.status_window.show()

	def __init__(self, server):
		QToolBar.__init__(self)
		self.hpc_window = QWidget()
		#self.hpc_window.show()

		self.myserver=server
		self.win_list=windows()


		self.setIconSize(QSize(42, 42))

		self.cluster_button = QAction(QIcon(os.path.join(get_image_file_path(),"not_connected.png")), _("Connect to cluster"), self)
		self.cluster_button.triggered.connect(self.callback_cluster_connect)
		self.addAction(self.cluster_button)

		self.cluster_get_data = QAction(QIcon(os.path.join(get_image_file_path(),"server_get_data.png")), _("Cluster get data"), self)
		self.cluster_get_data.triggered.connect(self.callback_cluster_get_data)
		self.addAction(self.cluster_get_data)
		self.cluster_get_data.setEnabled(False)

		self.cluster_get_info = QAction(QIcon(os.path.join(get_image_file_path(),"server_get_info.png")), _("Cluster get info"), self)
		self.cluster_get_info.triggered.connect(self.callback_cluster_get_info)
		self.addAction(self.cluster_get_info)
		self.cluster_get_info.setEnabled(False)


		self.cluster_copy_src = QAction(QIcon(os.path.join(get_image_file_path(),"server_copy_src.png")), _("Copy src to cluster"), self)
		self.cluster_copy_src.triggered.connect(self.callback_cluster_copy_src)
		self.addAction(self.cluster_copy_src)
		self.cluster_copy_src.setEnabled(False)

		self.cluster_make = QAction(QIcon(os.path.join(get_image_file_path(),"server_make.png")), _("Make on cluster"), self)
		self.cluster_make.triggered.connect(self.callback_cluster_make)
		self.addAction(self.cluster_make)
		self.cluster_make.setEnabled(False)

		self.cluster_clean = QAction(QIcon(os.path.join(get_image_file_path(),"server_clean.png")), _("Clean cluster"), self)
		self.cluster_clean.triggered.connect(self.callback_cluster_clean)
		self.addAction(self.cluster_clean)
		self.cluster_clean.setEnabled(False)

		self.cluster_off = QAction(QIcon(os.path.join(get_image_file_path(),"off.png")), _("Kill all cluster code"), self)
		self.cluster_off.triggered.connect(self.callback_cluster_off)
		self.addAction(self.cluster_off)
		self.cluster_off.setEnabled(False)


		self.cluster_sync = QAction(QIcon(os.path.join(get_image_file_path(),"sync.png")),  _("Sync"), self)
		self.cluster_sync.triggered.connect(self.callback_cluster_sync)
		self.addAction(self.cluster_sync)
		self.cluster_sync.setEnabled(False)

		self.cluster_stop = QAction(QIcon(os.path.join(get_image_file_path(),"pause.png")),  _("Stop"), self)
		self.cluster_stop.triggered.connect(self.callback_cluster_stop)
		self.addAction(self.cluster_stop)
		self.cluster_stop.setEnabled(False)


		self.cluster_jobs = QAction(QIcon(os.path.join(get_image_file_path(),"server_jobs.png")),  _("Get jobs"), self)
		self.cluster_jobs.triggered.connect(self.callback_cluster_jobs)
		self.addAction(self.cluster_jobs)
		self.cluster_jobs.setEnabled(False)

		self.cluster_view_button = QAction(QIcon(os.path.join(get_image_file_path(),"server.png")),  _("Configure cluster"), self)
		self.cluster_view_button.triggered.connect(self.callback_cluster_view_button)
		self.addAction(self.cluster_view_button)
		self.cluster_view_button.setEnabled(False)

		self.init_job_window()
		
		return



		self.prog_hbox=gtk.VBox(False, 2)
		self.prog_hbox.show()

		self.bar=[]
		self.button=[]
		self.slider=[]
		self.label=[]

