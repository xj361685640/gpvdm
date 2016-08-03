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



#import sys
#import os
#import shutil
#from scan_item import scan_item
from tab_base import tab_base
#from help import my_help_class

from PyQt5.QtWidgets import QTabWidget,QTextEdit
from PyQt5.QtCore import QProcess
from PyQt5.QtGui import QPalette,QColor,QFont

class tab_terminal(QTextEdit,tab_base):

	def dataReady(self):
		cursor = self.textCursor()
		cursor.movePosition(cursor.End)
		cursor.insertText(str(self.process.readAll()))
		self.ensureCursorVisible()

	def run(self,command,args):
		self.process.start(command,args)

	def init(self):
		self.font = QFont()
		self.font.setFamily('Monospace')
		self.font.setStyleHint(QFont.Monospace)
		self.font.setFixedPitch(True)
		self.font.setPointSize(int(12))

		self.setFont(self.font)

		pal = QPalette()
		bgc = QColor(0, 0, 0)
		pal.setColor(QPalette.Base, bgc)
		textc = QColor(230, 230, 230)
		pal.setColor(QPalette.Text, textc)
		self.setPalette(pal)
		self.process = QProcess(self)
		self.process.readyRead.connect(self.dataReady)

	def help(self):
		my_help_class.help_set_help(["command.png","<big><b>The terminal window</b></big>\nThe model will run in this window.  You can also use it to enter bash commands."])

