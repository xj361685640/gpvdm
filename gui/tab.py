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



import os

#from scan_item import scan_item_add
from token_lib import tokens
#from undo import undo_list_class
from tab_base import tab_base
from util import str2bool
from scan_item import scan_remove_file
from inp import inp_load_file
from inp import inp_update_token_value
from inp import inp_get_token_value
#from help import my_help_class
from util import latex_to_pygtk_subscript
from i18n import yes_no
from cal_path import get_image_file_path
#from gtkswitch import gtkswitch
#from leftright import leftright

from PyQt5.QtWidgets import QGridLayout,QPushButton,QLabel,QLineEdit

import i18n
_ = i18n.language.gettext

class tab_class(QGridLayout,tab_base):

	lines=[]
	edit_list=[]



	def callback_edit(self, file_name,token,data):
		print file_name, token, data
		inp_update_token_value(file_name, token, data,1)

		#my_help_class.help_set_help(["32_save.png","<big><b>Saved to disk</b></big>\n"])

	def help(self):
		my_help_class.get_help(self.file_name)


	def rename(self,tab_name):
		self.tab_name=tab_name

		mytext=tab_name
		if len(mytext)<10:
			for i in range(len(mytext),10):
				mytext=mytext+" "
		self.label.set_text(mytext)


	def init(self,filename,tab_name):

		scan_remove_file(filename)

		self.edit_list=[]
		inp_load_file(self.lines,filename)

		n=0
		pos=0
		my_token_lib=tokens()
		height=27

		for i in range(0, len(self.lines)/2):

			show=False
			units="Units"
			token=self.lines[pos]
			result=my_token_lib.find(token)
			if result!=False:
				units=result.units
				text_info=result.info
				show=True
			pos=pos+1
			#self.set_size_request(600,-1)
			if show == True :
				description=QLabel()
				description.setText(text_info)
				#label.set_markup(latex_to_pygtk_subscript(text_info))


				#if result.opt[0]=="text":
				edit_box=QLineEdit()
				edit_box.setText(self.lines[pos])
					#edit_box.set_text(self.lines[pos]);
				edit_box.textChanged.connect(lambda: self.callback_edit(filename,token,edit_box.text()))
					#edit_box.show()

				unit=QLabel()
				unit.setText(units)


				self.addWidget(description,i,0)
				self.addWidget(edit_box,i,1)
				self.addWidget(unit,i,2)
		return
		if show == True :
			if show == True :


				if result.opt[0]=="switch":
					edit_box=gtkswitch()
					edit_box.init()
					edit_box.set_value(str2bool(self.lines[pos]))
					edit_box.connect("changed", self.callback_edit, token)
					edit_box.show_all()
				elif result.opt[0]=="leftright":
					edit_box=leftright()
					edit_box.init()
					edit_box.set_value(str2bool(self.lines[pos]))
					edit_box.connect("changed", self.callback_edit, token)
					edit_box.show_all()
				else:
					edit_box=gtk.combo_box_new_text()
					index=0
					true_false=False
					if len(result.opt)==2:
						if result.opt[0]==_("true") and result.opt[1]==_("false"):
							true_false=True

					for i in range(0,len(result.opt)):
						edit_box.append_text(result.opt[i])
						if true_false==False:
							if yes_no(self.lines[pos])==yes_no(result.opt[i]):
								index=i
						else:
							if str2bool(self.lines[pos])==True:
								index=0
							else:
								index=1

					edit_box.set_active(index);

					edit_box.connect("changed", self.callback_edit, token)
					edit_box.show()

				line=1
				scan_item_add(filename,token,text_info,line)

				n=n+1

			pos=pos+1
