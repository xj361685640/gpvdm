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
#from tab_homo import tab_bands

#util
from win_lin import running_on_linux

#paths
from cal_path import get_bin_path
from cal_path import get_image_file_path
from code_ctrl import enable_webbrowser
from cal_path import get_exe_command


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
		
	def get_current_page(self):
		i=self.currentIndex()
		return self.tabText(i)

	def goto_page(self,page):
		for i in range(0,self.count()):
				if self.tabText(i)==page:
					self.setCurrentIndex(i)
					break

	def callback_close_button(self,index):
		print("close_handler called, index = %s" % index)
		self.removeTab(index)

	def callback_switch_page(self, notebook, page, page_num):
		if self.last_page!=page_num:
			self.last_page=page_num
			self.get_children()[page_num].help()
			#print "Rod", page_num
		#self.toggle_tab_visible(data)

	def clean_menu(self):
		for menu in self.menu_items:
			self.item_factory.delete_item(menu)
		self.menu_items=[]

	def callback_view_toggle(self, widget, data):
		self.toggle_tab_visible(data.get_label())

	def set_item_factory(self,item_factory):
		self.item_factory=item_factory

	def toggle_tab_visible(self,name):
		if self.finished_loading==True:
			for child in self.get_children():
				if child.label_name==name:
					if child.visible==False:
						if self.item_factory!=None:
							widget=self.item_factory.get_widget(_("/View/")+name)
							widget.set_active(True)
						child.show()
						child.visible=True
					else:
						if self.item_factory!=None:
							widget=self.item_factory.get_widget(_("/View/")+name)
							widget.set_active(False)
						child.hide()
						child.visible=False

					#print "gui_config.inp", "#"+child.file_name, str(int(child.visible)),2
					inp_update_token_value("gui_config.inp", "#"+child.file_name, str(int(child.visible)),1)

	def add_to_menu(self,name,visible):
		#print _("/View/")+name
		a = (( _("/View/")+name,  None, self.callback_view_toggle, 0, "<ToggleItem>" ),   )
		self.item_factory.create_items( a, )
		path=_("/View/")+name
		myitem=self.item_factory.get_item(path)
		self.menu_items.append(path)
		myitem.set_active(visible)

	def add_info_page(self):
		browser=information()
		self.addTab(browser,_("Information"))

	def load(self):
		self.clean_menu()
		self.last_page=0

		self.setTabsClosable(True)
		self.setMovable(True)
		self.tabCloseRequested.connect(self.callback_close_button)
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
			#tab.show()
			#tab.init(file_name,name)

			self.addTab(widget,_("Device structure"))
			#self.main_tab.show()
			#self.append_page(self.main_tab, gtk.Label(_("Device structure")))
	
			lines=[]
			pos=0
			if inp_load_file(lines,"gui_config.inp")==True:
				pos=0
				tab_number=0
				tabs=(len(lines)-3)/2
				print("tabs=",tabs)
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

					#elif file_name=="lumo0.inp":
					#	tab=tab_bands()
					#	tab.update()
					#	if tab.enabled==True:
					#		add_to_widget=True
					#		tab.visible=visible
					#		tab.wow()
					#		tab.label_name=name
					#		tab.file_name=file_name

					#el
					if file_name=="epitaxy.inp":
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


					#	add_to_widget=True
					#	tab=
					#	tab.visible=visible
					#	tab.init(file_name,name)
					#	tab.label_name=name
					#	tab.file_name=file_name

					if add_to_widget==True:
						mytext=name
						if len(mytext)<10:
							for i in range(len(mytext),10):
								mytext=mytext+" "
						self.addTab(widget,mytext)


					#	if (visible==True):
					#		tab.show()


					#	self.add_to_menu(name,visible)
			#else:
			#	print _("No gui_config.inp file found\n")

			#for child in self.get_children():
			#		print type(child)

			#if running_on_linux()==True:
			self.terminal=tab_terminal()
			self.terminal.init()
			self.addTab(self.terminal,"Terminal")
			self.terminal.run(os.getcwd(),get_exe_command()+" --version")

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



