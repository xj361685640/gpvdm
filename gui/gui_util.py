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


from cal_path import get_image_file_path
import os

#qt
from PyQt5.QtWidgets import QMainWindow, QTextEdit, QAction, QApplication

def dlg_get_text( message, default='',image_name=""):

	d = gtk.MessageDialog(None,
	          gtk.DIALOG_MODAL ,
	          gtk.MESSAGE_QUESTION,
	          gtk.BUTTONS_OK_CANCEL,
	          message)

	if image_name!="":
		image = gtk.Image()
	   	image.set_from_file(os.path.join(get_image_file_path(),image_name))
		image.show()
		print image_name
		d.set_image(image)

	entry = gtk.Entry()
	entry.set_text(default)
	entry.show()

	d.vbox.pack_end(entry)
	entry.connect('activate', lambda _: d.response(gtk.RESPONSE_OK))
	d.set_default_response(gtk.RESPONSE_OK)

	r = d.run()
	text = entry.get_text().decode('utf8')
	d.destroy()
	if r == gtk.RESPONSE_OK:
		return text
	else:
		return None

def dlg_get_multi_text( title_text,info, default=''):
	ret=[]
	d = gtk.Dialog( title=title_text,flags=gtk.DIALOG_DESTROY_WITH_PARENT,buttons=("OK",True,"Cancel",False))
	d.set_default_response(gtk.RESPONSE_OK)
	entry=[]
	for i in range(0,len(info)):
		hbox=gtk.HBox()
		entry.append(gtk.Entry())
		entry[len(entry)-1].set_text(info[i][1])
		l = gtk.Label( info[i][0])
		hbox.pack_start(l)
		hbox.pack_start(entry[len(entry)-1])
		hbox.show_all()
		d.vbox.pack_start( hbox)


	r = d.run()

	if r == True:
		for i in range(0,len(info)):
			ret.append(entry[i].get_text().decode('utf8'))
	else:
		for i in range(0,len(info)):
			ret.append(info[i][1])

	d.destroy()
	return ret

def process_events():
	QApplication.processEvents()

