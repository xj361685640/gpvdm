# -*- coding: utf-8 -*-
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

import sys
from PyQt5.QtWidgets import QApplication, QWidget

import sys
from PyQt5.QtWidgets import QWidget
from PyQt5.QtGui import QIcon,QPalette
from PyQt5.QtGui import QPainter,QFont,QColor,QPen,QPainterPath,QBrush
from PyQt5.QtCore import Qt, QTimer

from psutil import cpu_percent
from psutil import cpu_times
from psutil import disk_io_counters
import random

class cpu_usage(QWidget):
    
	def __init__(self):
		super().__init__()
		self.load=[]
		self.wait=[]
		self.wait_last=0.0
		self.setMinimumSize(40, 40)
		self.color=[]
		for i in range(0,1000):
			self.load.append(0)
			self.wait.append(0)
			self.color.append([0,0,0])

		self.delta=0    
		self.start()
		
	def start(self):
		self.timer=QTimer()
		self.timer.timeout.connect(self.update)
		self.timer.start(100);

	def stop(self):
		self.timer.stop()
		
	def update(self):
		a=0
		tot=0
		self.load.append(cpu_percent())
		for i in range(len(self.load)-1,len(self.load)):
			a=a+self.load[i]
			tot=tot+1.0
		a=a/tot
		self.load[len(self.load)-1]=a
		self.load.pop(0)

		try:		#user reported bug, This is a problem with the underlying function.
			w_temp=disk_io_counters()[3]/1000
		except:
			w_temp=0

		w_delta=w_temp-self.wait_last
		self.wait_last=w_temp

		self.wait.append(int(w_delta))
		#print(w_delta)
		self.wait.pop(0)
		
		self.color.append([255,0,0])
		self.color.pop(0)


		self.repaint()
		
	def paintEvent(self, e):
		qp = QPainter()
		qp.begin(self)
		self.drawWidget(qp)
		qp.end()
		
	def drawWidget(self, qp):
		h=self.height()
		w=self.width()
		qp = QPainter()
		qp.begin(self)

		qp.setBrush(QColor(0,0,0))
		qp.setPen(QColor(0,0,0))
		qp.drawRect(0, 0, w, h)
		
		
#		dx=self.width()/len(self.load)

#		for i in range(0,len(self.load)):
#			qp.setBrush(QColor(self.color[i][0],self.color[i][1],self.color[i][2]))
#			qp.setPen(QColor(self.color[i][0],self.color[i][1],self.color[i][2]))
		
#			dy=self.load[i]*h/100.0
#			qp.drawRect(dx*i, h, dx, -dy)

		dy=h/len(self.load)

		for i in range(0,len(self.load)):
			qp.setBrush(QColor(0,0,255))
			qp.setPen(QColor(0,0,255))

			dx=self.wait[i]*w/100.0
			qp.drawRect(w, h-dy*i, -dx, dy)
##########
			
			
			qp.setBrush(QColor(self.color[i][0],self.color[i][1],self.color[i][2]))
			qp.setPen(QColor(self.color[i][0],self.color[i][1],self.color[i][2]))
		
			dx=self.load[i]*w/100.0
			qp.drawRect(w, h-dy*i, -dx, dy)



			
		#for i in range(0,len(self.wait)):
		#	dy=self.wait[i]*h/1000.0
		#	qp.drawRect(dx*i, h, dx, -dy)

        

