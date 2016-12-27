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
#from progress import progress_class
#from gui_util import process_events

#inp files
from inp import inp_load_file
from inp import inp_get_next_token_array
from inp import inp_isfile
from inp import inp_update_token_value
from inp_description import inp_file_to_description

#tabs
from tab_main import tab_main
from tab import tab_class
from tab_homo import tab_bands

#util
from win_lin import running_on_linux

#paths
from cal_path import get_bin_path
from cal_path import get_image_file_path
from code_ctrl import enable_webbrowser
from cal_path import get_exe_command
from epitaxy import epitaxy_print

#qt
from PyQt5.QtWidgets import QTabWidget,QWidget

#window
from dos_main import dos_main
from pl_main import pl_main
from tab_terminal import tab_terminal

if enable_webbrowser()==True:
	from information_webkit import information
else:
	from information_noweb import information

from help import help_window

import i18n
_ = i18n.language.gettext


class gpvdm_notebook(QTabWidget):
	#progress=progress_class()
	finished_loading=False
	item_factory=None
	menu_items=[]

	def __init__(self):
		QWidget.__init__(self)
		self.terminal=None
		self.update_display_function=None
		self.currentChanged.connect(self.changed_click)

	def update(self):
		for i in range(0,self.count()):
			w=self.widget(i)
			w.update()

	def changed_click(self):
		if self.tabText(self.currentIndex()).strip()==_("Device"):
			help_window().help_set_help(["tab.png",_("<big><b>Device tab</b></big><br>This tab contains information about the device, such as width breadth, carrier density on the contacts, shunt and contact resistance.")])

		if self.tabText(self.currentIndex()).strip()==_("Device structure"):
			help_window().help_set_help(["device.png",_("<big><b>The device structure tab</b></big><br> Use this tab to change the structure of the device, the layer thicknesses and to perform optical simulations.  You can also browse the materials data base and  edit the electrical mesh.")])

		if self.tabText(self.currentIndex()).strip()==_("Bands"):
			help_window().help_set_help(["tab.png",_("<big><b>The bands tab</b></big><br> Use this tab to edit the energetic distribution of the density of states.")])

		if self.tabText(self.currentIndex()).strip()==_("Density of states"):
			help_window().help_set_help(["tab.png",_("<big><b>Density of States</b></big>\nThis tab contains the electrical model parameters, such as mobility, tail slope energy, and band gap.")])

		if self.tabText(self.currentIndex()).strip()==_("Luminescence"):
			help_window().help_set_help(["tab.png",_("<big><b>Luminescence</b></big>\nIf you set 'Turn on luminescence' to true, the simulation will assume recombination is a raditave process and intergrate it to produce Voltage-Light intensity curves (lv.dat).  Each number in the tab tells the model how efficient each recombination mechanism is at producing photons.")])

		if self.tabText(self.currentIndex()).strip()==_("Terminal"):
			help_window().help_set_help(["command.png",_("<big><b>The terminal window</b></big>\nThe output of the model will be displayed in this window, watch this screen for debugging and convergence information.")])

		if self.tabText(self.currentIndex()).strip()==_("Information"):
			help_window().help_set_help(["help.png",_("<big><b>On-line help</b></big>\nYou can view the on-line help and manual here.")])
		
	def get_current_page(self):
		i=self.currentIndex()
		return self.tabText(i)

	def goto_page(self,page):
		self.blockSignals(True)
		for i in range(0,self.count()):
				if self.tabText(i)==page:
					self.setCurrentIndex(i)
					break
		self.blockSignals(False)

	def callback_switch_page(self, notebook, page, page_num):
		if self.last_page!=page_num:
			self.last_page=page_num
			self.get_children()[page_num].help()

	def clean_menu(self):
		for menu in self.menu_items:
			self.item_factory.delete_item(menu)
		self.menu_items=[]

	def callback_view_toggle(self, widget, data):
		self.toggle_tab_visible(data.get_label())

	def set_item_factory(self,item_factory):
		self.item_factory=item_factory

	def add_info_page(self):
		browser=information()
		self.addTab(browser,_("Information"))

	def load(self):
		self.clean_menu()
		self.last_page=0

		#self.setTabsClosable(True)
		self.setMovable(True)
		if (os.path.exists("sim.gpvdm")==True) and (os.path.normcase(os.getcwd())!=os.path.normcase(get_bin_path())):
			self.finished_loading=False
			#self.progress.init()
			#self.progress.show()
			#self.progress.start()
			#self.progress.set_text("Loading..")
			#process_events()

			self.clear()

#			dos_files=inp_get_token_value("device_epitaxy.inp", "#layers")

			widget=tab_main()

			self.addTab(widget,_("Device structure"))

			self.update_display_function=widget.update

			lines=[]
			pos=0

			if inp_load_file(lines,"gui_config.inp")==True:
				pos=0
				tab_number=0
				tabs=(len(lines)-3)/2
				while (1):
					add_to_widget=False
					ret,pos=inp_get_next_token_array(lines,pos)

					if ret[0]=="#ver":
						break

					file_name=ret[0]
					if file_name[0]=="#":
						file_name=file_name[1:]
					name=inp_file_to_description(file_name)
					if name==False:
						print("name not found",name)
						break
					visible=bool(int(ret[1]))

					#self.progress.set_fraction(float(tab_number)/float(tabs))

					tab_number=tab_number+1
					#self.progress.set_text(_("Loading ")+name)
					#process_events()

					if file_name=="lumo0.inp":
						widget=tab_bands()
						#tab.update()
						add_to_widget=True
						widget.visible=visible
						widget.label_name=name
						widget.file_name=file_name
					elif file_name=="epitaxy.inp":
						widget=dos_main()
						widget.update()
						add_to_widget=True
						widget.visible=visible
						widget.label_name=name
						widget.file_name=file_name
					elif file_name=="pl0.inp":
						widget=pl_main()
						widget.update()
						add_to_widget=True
						widget.visible=visible
						widget.label_name=name
						widget.file_name=file_name
					elif inp_isfile(file_name)==True:
						add_to_widget=True
						widget=tab_class()
						widget.init(file_name,name)

					if add_to_widget==True:
						mytext=name
						if len(mytext)<10:
							for i in range(len(mytext),10):
								mytext=mytext+" "
						self.addTab(widget,mytext)


			self.terminal=tab_terminal()
			self.terminal.init()
			self.addTab(self.terminal,_("Terminal"))
			self.terminal.run(os.getcwd(),get_exe_command()+" --version --html")

			self.add_info_page()

			return True




			self.finished_loading=True
			#self.progress.stop()
			#self.progress.set_fraction(0.0)
			self.goto_page("tab_main")

		else:
			self.add_info_page()
			self.goto_page(_("Information"))
			return False



