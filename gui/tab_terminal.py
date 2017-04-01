#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2016 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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

from tab_base import tab_base

from PyQt5.QtWidgets import QTabWidget,QTextEdit,QWidget,QHBoxLayout
from PyQt5.QtCore import QProcess
from PyQt5.QtGui import QPalette,QColor,QFont

from QHTabBar import QHTabBar

import multiprocessing
import functools
from cpu_usage import cpu_usage
from win_lin import running_on_linux

class tab_terminal(QWidget,tab_base):

	def __init__(self):
		QWidget.__init__(self)
		self.tab=QTabWidget()
		self.vbox=QHBoxLayout()
		self.vbox.addWidget(self.tab)
		self.usage=cpu_usage()
		self.vbox.addWidget(self.usage)
		self.setLayout(self.vbox)

	def dataReady(self,i):
		cursor = self.terminals[i].textCursor()
		cursor.movePosition(cursor.End,cursor.MoveAnchor)
		self.terminals[i].setTextCursor(cursor)
		r=self.process[i].readAll()
		#print(">",r,"<")
		data=str(r,'utf-8',errors='ignore')
		#data=data[:-1]
		cursor.insertHtml(data)
		self.terminals[i].ensureCursorVisible()

	def run(self,path,command):
		for i in range(0,self.cpus):
	
			if self.process[i].state()==QProcess.NotRunning:
				cursor = self.terminals[i].textCursor()
				cursor.insertHtml(_("Running: ")+command+"<br>")
				self.process[i].setWorkingDirectory(path)

				if running_on_linux()==False:
					if command.count(".exe")>0:
						command="\""+command
						command=command.replace(".exe",".exe\"",1)

				print("exe command=",command)
				self.process[i].start(command)
				return

		print(_("I could not find a free cpu to run the command on"))

	def init(self):
		self.cpus=multiprocessing.cpu_count()
		
		self.tab.setTabsClosable(True)
		self.tab.setMovable(True)
		self.tab.setTabBar(QHTabBar())
		self.tab.setTabPosition(QTabWidget.West)
		
		self.font = QFont()
		self.font.setFamily('Monospace')
		self.font.setStyleHint(QFont.Monospace)
		self.font.setFixedPitch(True)
		self.font.setPointSize(int(12))

		self.terminals=[]
		self.process=[]
		for i in range(0,self.cpus):
			term=QTextEdit()
			term.setFont(self.font)
			
			pal = QPalette()
			bgc = QColor(0, 0, 0)
			pal.setColor(QPalette.Base, bgc)
			textc = QColor(230, 230, 230)
			pal.setColor(QPalette.Text, textc)
			term.setPalette(pal)

			proc=QProcess(self)
			proc.readyRead.connect(functools.partial(self.dataReady,i))
			self.process.append(proc)
			self.terminals.append(term)
			self.tab.addTab(term,_("CPU")+" "+str(i))




	def help(self):
		my_help_class.help_set_help(["utilities-terminal.png","<big><b>The terminal window</b></big>\nThe model will run in this window.  You can also use it to enter bash commands."])

