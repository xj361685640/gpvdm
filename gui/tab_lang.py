#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
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

## @package tab_lang
#  A tab to select the language used.
#

import os

from scan_item import scan_item_add
from token_lib import tokens
from undo import undo_list_class
from tab_base import tab_base
from util import str2bool
from scan_item import scan_remove_file
from inp import inp_update_token_value
from inp import inp_get_token_value
from util import latex_to_html
from i18n import yes_no
from cal_path import get_share_path
from gtkswitch import gtkswitch
from leftright import leftright
from help import help_window

from PyQt5.QtWidgets import QWidget, QScrollArea,QVBoxLayout,QProgressBar,QLabel,QDesktopWidget,QToolBar,QHBoxLayout,QAction, QSizePolicy, QTableWidget, QTableWidgetItem,QComboBox,QDialog,QAbstractItemView,QGridLayout,QLineEdit

import i18n
from i18n import get_languages
_ = i18n.language.gettext

import functools
from error_dlg import error_dlg

class language_tab_class(QWidget,tab_base):

	def __init__(self):
		QScrollArea.__init__(self)
		self.vbox=QVBoxLayout()

		self.tab=QHBoxLayout()
		widget=QWidget()
		widget.setLayout(self.tab)
		self.file_path=os.path.join(get_share_path(),"lang.inp")

		title_label=QLabel()
		title_label.setWordWrap(True)
		title_label.setOpenExternalLinks(True)
		title_label.setText(latex_to_html("<font size=5><b>Select the language you would like use gpvdm to use.</b><br><br> If a translation to your language does not exist or could be improved, then please consider joining the <a href=\"https://gpvdm.com/translation.html\">gpvdm translation project</a>.  I would like gpvdm translated into as many langauges as possible to improve access to high quality solar cell simulation tools for all.</font>"))

		self.vbox.addWidget(title_label)
		self.vbox.addWidget(widget)


		description=QLabel()
		description.setText(latex_to_html("Default language:"))

		self.lang_box=QComboBox()

		langs=get_languages()
		self.lang_box.addItem("auto")
		for i in range(0,len(langs)):
			self.lang_box.addItem(langs[i])

		token=inp_get_token_value(self.file_path, "#lang",archive="base.gpvdm")
		all_items  = [self.lang_box.itemText(i) for i in range(self.lang_box.count())]
		for i in range(0,len(all_items)):
			if all_items[i] == token:
				self.lang_box.setCurrentIndex(i)
				
		self.lang_box.currentIndexChanged.connect(self.callback_edit)

		self.lang_box.setFixedSize(300, 25)


		self.tab.addWidget(description)
		self.tab.addWidget(self.lang_box)
		


		spacer = QWidget()
		spacer.setSizePolicy(QSizePolicy.Expanding, QSizePolicy.Expanding)
		self.vbox.addWidget(spacer)
		self.setLayout(self.vbox)

	def callback_edit(self):
		print("updating lang file",self.file_path)
		inp_update_token_value(self.file_path, "#lang", self.lang_box.itemText(self.lang_box.currentIndex()),archive="base.gpvdm")
		error_dlg(self,"Please restart gpvdm for the changes to take effect.")

	def help(self):
		help_window().get_help(self.file_name)


