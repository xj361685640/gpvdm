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


import os
from win_lin import running_on_linux
from threading import Thread
from ver import ver_core
from ver import ver_mat
import platform
from gpvdm_http import get_data_from_web
from cal_path import get_share_path
import hashlib
from sim_warnings import sim_warnings
from code_ctrl import enable_webupdates
import i18n
_ = i18n.language.gettext
from ver import ver

#qt
from PyQt5.QtCore import QSize, Qt
from PyQt5.QtGui import QIcon
from PyQt5.QtWidgets import QWidget
from PyQt5.QtGui import QPixmap
from PyQt5.QtCore import pyqtSignal
from PyQt5.QtWidgets import QWidget

from gui_util import yes_no_dlg

from display import is_open_gl_working

from PyQt5.QtCore import QTimer

from uid_gen import uid_get

#Under windows, this class will connect to gpvdm.com and look for updates, a user prompt will be displayed if any are found.  It can also download updates if the user asks it to.  It's not called under linux because linux has it's own package management system.

checked_web=False

def sp(value):
	return value.split(os.sep)

def update_fetch():
	text=[]
	text.append("Checking web for updates...")

#	disk_files=[]
	web_src=[]
	disk_dest=[]

	update_path="http://www.gpvdm.com/update_windows/"+ver()+"/"
	lines=get_data_from_web(update_path+"list.dat")
	print("Got file list")
	lines=lines.split('\n')
	files=[]
	md5=[]
	web_md5=[]
	for i in range(0,len(lines)):
		if lines[i].count("  ")!=0:
			m,f=lines[i].split("  ")
			f=f[2:].split("/")
			md5.append(m)
			files.append(f)

	for i in range(0,len(files)):

		root=files[i][0]
		if root=="images" or root=="solvers" or root=="gpvdm_core.exe" or root=="device_lib" or root=="sim.gpvdm" or root=="lang" or root=="materials" or root=="light":
			md5_web=md5[i]
			md5_disk="none"
			disk_path=os.path.join(get_share_path(),"/".join(files[i]))
			web_path=update_path+"/".join(files[i])
			if os.path.isfile(disk_path):
				md5_disk=hashlib.md5(open(disk_path,'rb').read()).hexdigest()

			if md5_web!=md5_disk:
				web_src.append(web_path)
				disk_dest.append(disk_path)
				web_md5.append(md5_web)

	for i in range(0,len(web_src)):
		text.append(web_src[i]+" "+disk_dest[i])
		a=get_data_from_web(web_src[i])
		l=len(a)
		if l>100:
			l=100;
		if a[:l].count("403 Forbidden")!=0:
			text.append("Access to file "+web_src[i]+" forbidden")
		else:
			web_hash=hashlib.md5(a).hexdigest()
			list_hash=web_md5[i]
			if web_hash==list_hash:
				text.append("updating file "+disk_dest[i])
				if running_on_linux()==False:
					f=open(disk_dest[i], mode='wb')
					lines = f.write(a)
					f.close()
			else:
				text.append("Checksum error "+disk_dest[i])
	return text

def update_now():
	response=yes_no_dlg(self,_("This feature is sill under development.  It searches the gpvdm web page for updates then uses them to update gpvdm.  Use it at your own risk - I take no responsibility for any data loss!  Do you wish to continue?"))

	if response==True:
		ret='\n'.join(update_fetch())
		dialog=sim_warnings()
		dialog.init(ret)
		response=dialog.run()
		dialog.destroy()

	md.destroy()



class update_thread(QWidget):
	got_data = pyqtSignal(str)

	def __init__(self):
		QWidget.__init__(self)
		self.text=""

	def get_from_web(self,url):
			page="http://www.gpvdm.com/download_windows/update.php?ver_core="+ver_core()+"&uid="+uid_get()+"&os="+platform.platform()+"&opengl="+is_open_gl_working()
			message=get_data_from_web(page)

			message=message.split("\n")
			print(message)
			self.text=""
			if message[0].startswith("update"):
				token,ver=message[0].split("#")
				self.text="Version "+ver+" of opvdm is now available."
			self.got_data.emit(self.text)
			
	def foo(self,n):
		self.get_from_web('http://www.gpvdm.com')

	def start_thread(self):
		p = Thread(target=self.foo, args=(10,))
		#multiprocessing.Process(target=self.foo, name="Foo", args=(10,))
		p.daemon = True
		p.start()
		
	def start(self):
		global checked_web
		if checked_web==False:			#make this a one shot thing
			checked_web=True
			self.timer=QTimer()
			self.timer.setSingleShot(True)
			self.timer.timeout.connect(self.start_thread)
			self.timer.start(5000)
		


