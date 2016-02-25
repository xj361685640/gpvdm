#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012 Roderick C. I. MacKenzie
#
#	roderick.mackenzie@nottingham.ac.uk
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

import pygtk
pygtk.require('2.0')
import gtk
import sys
import os
import shutil
import commands
import subprocess
from win_lin import running_on_linux
from cal_path import get_exe_command
import urllib2
import socket 
from threading import Thread
import time
from socket import setdefaulttimeout
from socket import socket
from socket import error
from socket import AF_INET
from socket import SOCK_STREAM
from socket import SOL_SOCKET
from socket import SO_REUSEADDR
from socket import getdefaulttimeout
import urlparse
import re
import os
from ver import ver_core
from ver import ver_mat
from ver import ver_gui
import gobject
import platform
import getpass
from tab_base import tab_base
from help import my_help_class

socket.setdefaulttimeout = 1.0
os.environ['no_proxy'] = '127.0.0.1,localhost'
linkRegex = re.compile('<a\s*href=[\'|"](.*?)[\'"].*?>')
CRLF = "\r\n\r\n"

#This is the welcome tab.  It sends a request to www.gpvdm.com/update.php and then displays the text it fetches in the tab.  The idea of this is to tell people when updates are available.



class update_thread(gtk.VBox):
	def __init__(self):
		self.__gobject_init__()
		self.text=""

	def get_from_web(self,url):
		setdefaulttimeout(4.0)
		url = urlparse.urlparse(url)
		HOST = url.netloc
		PORT = 80
		try:
			s = socket(AF_INET, SOCK_STREAM)
		except error as msg:
			s = None

		s.settimeout(4.0)


		s.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)

		try:
			s.connect((HOST, PORT))
		except error as msg:
			s.close()
			s = None
		if s!=None:
			s.send("GET http://www.gpvdm.com/download_windows/update.php?ver_core="+ver_core()+"&ver_gui="+ver_gui()+"&ver_mat="+ver_mat()+"&os="+platform.platform()+"&"+" HTTP/1.0" +CRLF)
			data = (s.recv(1000000))

			s.shutdown(1)
			s.close()
			message=data.split('charset=UTF-8\r\n\r\n', 1)[-1]
			message=message.split("\n")
			self.text=""
			if message[0].startswith("update"):
				token,ver=message[0].split("#")
				self.text="Version "+ver+" of opvdm is now avaliable."
			gobject.idle_add(gobject.GObject.emit,self,"got-data")
			#self.emit("got-data")

	def foo(self,n):
		self.get_from_web('http://www.gpvdm.com')

	def start(self):
		p = Thread(target=self.foo, args=(10,))
		#multiprocessing.Process(target=self.foo, name="Foo", args=(10,))
		p.daemon = True
		p.start()


gobject.type_register(update_thread)
gobject.signal_new("got-data", update_thread, gobject.SIGNAL_RUN_FIRST,gobject.TYPE_NONE, ())

