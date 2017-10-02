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

from scan_item import scan_item_add
from token_lib import tokens
from undo import undo_list_class
from tab_base import tab_base
from util import str2bool
from scan_item import scan_remove_file
from inp import inp_load_file
from inp import inp_update_token_value
from inp import inp_get_token_value
from inp import inp_get_next_token_array
from util import latex_to_html
from i18n import yes_no
from gtkswitch import gtkswitch
from leftright import leftright
from help import help_window
from gpvdm_select import gpvdm_select

from PyQt5.QtCore import pyqtSignal

from PyQt5.QtWidgets import QTextEdit,QWidget, QScrollArea,QVBoxLayout,QProgressBar,QLabel,QDesktopWidget,QToolBar,QHBoxLayout,QAction, QSizePolicy, QTableWidget, QTableWidgetItem,QComboBox,QDialog,QAbstractItemView,QGridLayout,QLineEdit
from PyQt5.QtCore import QSize, Qt
from PyQt5.QtGui import QPixmap, QIcon

from QComboBoxLang import QComboBoxLang
from QColorPicker import QColorPicker

from icon_lib import QIcon_load

from PyQt5.QtCore import pyqtSignal

import i18n
_ = i18n.language.gettext

import functools
from inp import inp_update_token_array


class QChangeLog(QTextEdit):
	def __init__(self):
		QTextEdit.__init__(self)

class QLabel_click(QLabel):
	clicked = pyqtSignal()

	def __init(self, parent):
		QLabel.__init__(self, parent)

	def mouseDoubleClickEvent(self, ev):
		self.clicked.emit()

class tab_class(QWidget,tab_base):

	lines=[]
	edit_list=[]
	changed = pyqtSignal()
		
	def __init__(self):
		self.icon_file=""
		QWidget.__init__(self)
		self.editable=True


	def callback_edit(self, file_name,token,widget):
		if type(widget)==QLineEdit:
			a=undo_list_class()
			a.add([file_name, token, inp_get_token_value(self.file_name, token),widget])
			inp_update_token_value(file_name, token, widget.text())
		elif type(widget)==gtkswitch:
			inp_update_token_value(file_name, token, widget.get_value())
		elif type(widget)==leftright:
			inp_update_token_value(file_name, token, widget.get_value())
		elif type(widget)==gpvdm_select:
			inp_update_token_value(file_name, token, widget.text())
		elif type(widget)==QComboBox:
			inp_update_token_value(file_name, token, widget.itemText(widget.currentIndex()))
		elif type(widget)==QComboBoxLang:
			inp_update_token_value(file_name, token, widget.currentText_english())
		elif type(widget)==QColorPicker:
			inp_update_token_array(file_name, token, [str(widget.r),str(widget.g),str(widget.b)])
		elif type(widget)==QChangeLog:
			a=undo_list_class()
			a.add([file_name, token, inp_get_token_value(self.file_name, token),widget])
			inp_update_token_array(file_name, token, widget.toPlainText().split("\n"))
			
		help_window().help_set_help(["document-save-as","<big><b>Saved to disk</b></big>\n"])
		
		self.changed.emit()

	def help(self):
		help_window().get_help(self.file_name)

	def set_edit(self,editable):
		self.editable=editable

	def callback_ref(self,file_name,token,widget):
		from ref import ref_window
		self.ref_window=ref_window(os.path.splitext(file_name)[0]+"_"+token[1:])
		self.ref_window.show()

	def init(self,filename,tab_name):
		self.scroll=QScrollArea()
		self.main_box_widget=QWidget()
		self.vbox=QVBoxLayout()
		self.hbox=QHBoxLayout()
		self.hbox.setAlignment(Qt.AlignTop)
		self.file_name=filename
		self.tab_name=tab_name

		self.tab=QGridLayout()
		widget=QWidget()
		widget.setLayout(self.tab)
		self.vbox.addWidget(widget)

		scan_remove_file(filename)

		self.edit_list=[]
		self.lines=inp_load_file(filename)
		if self.lines==False:
			print("file not found",filename)
		n=0
		pos=0
		my_token_lib=tokens()
		widget_number=0

		while (1):
			ret,pos=inp_get_next_token_array(self.lines,pos)

			token=ret[0]
			if token=="#ver":
				break

			if token=="#end":
				break

			if token.startswith("#"):
				show=False
				units="Units"

				value=ret[1]

				result=my_token_lib.find(token)
				if result!=False:
					units=result.units
					text_info=result.info
					show=True
				
				#self.set_size_request(600,-1)
				if show == True :
					description=QLabel_click()
					description.setText(latex_to_html(text_info))
					if os.path.isfile(os.path.splitext(filename)[0]+"_"+token[1:]+".ref"):
						description.setStyleSheet('color: green')

					description.clicked.connect(functools.partial(self.callback_ref,filename,token,description))


					if result.widget=="gtkswitch":
						edit_box=gtkswitch()
						edit_box.setFixedSize(300, 25)
						edit_box.set_value(str2bool(value))
						edit_box.changed.connect(functools.partial(self.callback_edit,filename,token,edit_box))
					elif result.widget=="leftright":
						edit_box=leftright()
						edit_box.setFixedSize(300, 25)
						edit_box.set_value(str2bool(value))
						edit_box.changed.connect(functools.partial(self.callback_edit,filename,token,edit_box))
					elif result.widget=="gpvdm_select":
						edit_box=gpvdm_select(file_box=True)
						edit_box.setFixedSize(300, 25)
						edit_box.setText(value)
						edit_box.edit.textChanged.connect(functools.partial(self.callback_edit,filename,token,edit_box))
					elif result.widget=="QLineEdit":
						edit_box=QLineEdit()
						edit_box.setFixedSize(300, 25)
						if self.editable==False:
							edit_box.setReadOnly(True)
						edit_box.setText(value)
						#edit_box.set_text(self.lines[pos]);
						edit_box.textChanged.connect(functools.partial(self.callback_edit,filename,token,edit_box))
						#edit_box.show()
					elif result.widget=="QColorPicker":
						r=float(ret[1])
						g=float(ret[2])
						b=float(ret[3])
						edit_box=QColorPicker(r,g,b)
						edit_box.setFixedSize(300, 25)
						edit_box.changed.connect(functools.partial(self.callback_edit,filename,token,edit_box))
					elif result.widget=="QComboBoxLang":
						edit_box=QComboBoxLang()
						edit_box.setFixedSize(300, 25)
						for i in range(0,len(result.defaults)):
							edit_box.addItemLang(result.defaults[i][0],result.defaults[i][1])

						edit_box.setValue_using_english(value)
								
						edit_box.currentIndexChanged.connect(functools.partial(self.callback_edit,filename,token,edit_box))
					elif result.widget=="QChangeLog":
						edit_box=QChangeLog()
						edit_box.setMinimumHeight(100)
						if self.editable==False:
							edit_box.setReadOnly(True)
						edit_box.setText(value)
						edit_box.textChanged.connect(functools.partial(self.callback_edit,filename,token,edit_box))
					else:
						edit_box=QComboBox()
						edit_box.setFixedSize(300, 25)
						for i in range(0,len(result.defaults)):
							edit_box.addItem(result.defaults[i])
							
						all_items  = [edit_box.itemText(i) for i in range(edit_box.count())]
						for i in range(0,len(all_items)):
							if all_items[i] == value:
								edit_box.setCurrentIndex(i)
								break
								
						edit_box.currentIndexChanged.connect(functools.partial(self.callback_edit,filename,token,edit_box))

					
					unit=QLabel()
					unit.setText(latex_to_html(units))


					self.tab.addWidget(description,widget_number,0)
					self.tab.addWidget(edit_box,widget_number,1)
					self.tab.addWidget(unit,widget_number,2)
					
					scan_item_add(filename,token,text_info,1)
					
					widget_number=widget_number+1

		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		self.vbox.addWidget(spacer)
		self.main_box_widget.setLayout(self.vbox)
		
		self.scroll.setWidget(self.main_box_widget)

		self.icon_widget=QWidget()
		self.icon_widget_vbox=QVBoxLayout()
		self.icon_widget.setLayout(self.icon_widget_vbox)
		
		if self.icon_file!="":
			self.image=QLabel()
			icon=QIcon_load(self.icon_file)
			self.image.setPixmap(icon.pixmap(icon.actualSize(QSize(32, 32))))
			self.icon_widget_vbox.addWidget(self.image)

			spacer2 = QWidget()
			spacer2.setSizePolicy(QSizePolicy.Fixed, QSizePolicy.Expanding)
			self.icon_widget_vbox.addWidget(spacer2)
		
			self.hbox.addWidget(self.icon_widget)

		self.hbox.addWidget(self.scroll)
		
		self.setLayout(self.hbox)
