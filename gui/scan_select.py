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
from window_list import windows
from scan_item import scan_items_get_list
from scan_item import scan_items_get_file
from scan_item import scan_items_get_token

import i18n
_ = i18n.language.gettext

#qt
from PyQt5.QtCore import QSize, Qt 
from PyQt5.QtWidgets import QWidget,QVBoxLayout,QToolBar,QSizePolicy,QAction,QTabWidget,QTableWidget,QAbstractItemView, QTreeWidget, QPushButton, QHBoxLayout, QTreeWidgetItem
from PyQt5.QtGui import QPainter,QIcon

from cal_path import get_image_file_path
from gui_util import tab_set_value

class select_param(QWidget):
	def init(self,treeview):
		self.dest_treeview=treeview
		
	def __init__(self):
		QWidget.__init__(self)
		self.win_list=windows()
		self.setFixedSize(300,700)
		self.main_vbox=QVBoxLayout()

		self.setWindowIcon(QIcon(os.path.join(get_image_file_path(),"scan.png")))

		self.setWindowTitle(_("Select simulation parameter (www.gpvdm.com)")) 


		self.tab = QTreeWidget()
		#self.tab.setHeaderItem("Scan items")


		self.main_vbox.addWidget(self.tab)

		self.hwidget=QWidget()

		okButton = QPushButton("OK")
		cancelButton = QPushButton("Cancel")

		hbox = QHBoxLayout()
		hbox.addStretch(1)
		hbox.addWidget(okButton)
		hbox.addWidget(cancelButton)

		self.hwidget.setLayout(hbox)

		self.main_vbox.addWidget(self.hwidget)

		self.setLayout(self.main_vbox)

		okButton.clicked.connect(self.tree_apply_click) 
		cancelButton.clicked.connect(self.close)

		#self.tab.itemSelectionChanged.connect(self.tree_apply_click)
		
		self.update()

		return

	def make_entry(self,root,text):
		depth=0
		pointer=root
		for depth in range(0,len(text)):
			found=False
			for i in range(0,pointer.childCount()):
				if pointer.child(i).text(0)==text[depth]:
					found=True
					pointer=pointer.child(i)
					break
			if found==False:
				pointer=QTreeWidgetItem(pointer, [text[depth]])
				


	def update(self):
		self.tab.clear()
		root = QTreeWidgetItem(self.tab, ["root"])
		
		param_list=scan_items_get_list()
		i=0
		for item in range(0, len(param_list)):
			div_str=param_list[item].name.replace("\\", "/")
			div_str=div_str.split("/")
			piter=None
			self.make_entry(root,div_str)

	 

	def on_destroy(self):
		self.win_list.update(self,"scan_select")
		self.hide()
		return True

	def cal_path(self):
		getSelected = self.tab.selectedItems()
		if getSelected:
			item = getSelected[0]
#			getChildNode = baseNode.text(0)


			path = []
			while item is not None:
				path.append(str(item.text(0)))
				item = item.parent()

		ret="/".join(reversed(path))
		ret=ret[5:]
		return ret
	
	def tree_apply_click(self):
		getSelected = self.tab.selectedItems()
		if getSelected:
			path=self.cal_path()
			tab_set_value(self.dest_treeview,0,0,scan_items_get_file(path))
			tab_set_value(self.dest_treeview,0,1,scan_items_get_token(path))
			tab_set_value(self.dest_treeview,0,2,path)

			self.close()
		return



