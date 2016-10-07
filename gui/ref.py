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
from PyQt5.QtWidgets import QMainWindow, QTextEdit, QAction, QApplication,QTableWidgetItem,QComboBox, QMessageBox, QDialog, QDialogButtonBox, QFileDialog
from PyQt5.uic import loadUi
from PyQt5.QtGui import QPixmap, QIcon


#windows
from cal_path import get_ui_path

from gpvdm_select import gpvdm_select

from inp import inp_isfile
from inp import inp_get_token_array
from inp import inp_update
from inp import inp_check_ver
from inp import inp_new_file
from inp import inp_write_lines_to_file
from inp import inp_add_token
from inp import inp_load_file
from inp import inp_save_lines


class ref():
	def __init__(self,file_name,token):
		self.file_name=os.path.splitext(file_name)[0]+".ref"
		self.token=token
		self.ui = loadUi(os.path.join(get_ui_path(),"ref.ui"))
		self.ui.label.setText(_("Reference information"))
		pixmap = QPixmap(os.path.join(get_image_file_path(),"ref.png"))
		self.ui.image.setPixmap(pixmap)
		self.ui.setWindowIcon(QIcon(os.path.join(get_image_file_path(),"ref.jpg")))		

		self.load()

	def load(self):
		ret=inp_get_token_array(self.file_name, self.token)
		if ret!=False:									#We have found the file and got the token
			self.ui.text.setText("\n".join(ret))
		else:	
			self.ui.text.setText(_("New file"))

			if inp_check_ver(self.file_name, "1.0")==True:	#The file exists but there is no token.
				lines=[]
				inp_load_file(lines,self.file_name)
				lines=inp_add_token(lines,self.token,self.ui.text.toPlainText())
				print("written to 1",self.file_name)
				inp_save_lines(self.file_name,lines)
			else:											#The file does not exist or there is an error
				lines=inp_new_file()
				lines=inp_add_token(lines,self.token,self.ui.text.toPlainText())
				print("written to 2",self.file_name,lines)
				inp_save_lines(self.file_name,lines)

	def run(self):
		ret=self.ui.exec_()
		if ret==True:
			print("update ",self.file_name)
			inp_update(self.file_name, self.token, self.ui.text.toPlainText())
