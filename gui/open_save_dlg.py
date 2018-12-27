#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2017 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
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

## @package open_save_dlg
#  Open and save dlgs 
#


import os

try:
	from PyQt5.QtWidgets import QMainWindow, QTextEdit, QAction,QApplication,QTableWidgetItem,QComboBox, QMessageBox, QDialog, QDialogButtonBox, QFileDialog
	from PyQt5.QtWidgets import QGraphicsScene,QListWidgetItem,QListView,QLineEdit,QWidget,QHBoxLayout,QPushButton
	from PyQt5.QtWidgets import QFileDialog
	from PyQt5.uic import loadUi
	from PyQt5.QtGui import QPixmap
	from PyQt5.QtCore import QSize, Qt, QTimer
	from PyQt5.QtCore import QPersistentModelIndex
	from QComboBoxLang import QComboBoxLang
	from PyQt5.QtGui import QIcon
except:
	pass

from cal_path import to_native_path

def save_as_gpvdm(parent):
	dialog = QFileDialog(parent)
	dialog.setWindowTitle(_("Save a the simulation as"))
	dialog.setNameFilter(_("Directory"))
	dialog.setAcceptMode(QFileDialog.AcceptSave)
	dialog.setOption(QFileDialog.ShowDirsOnly, True) 
	if dialog.exec_() == QDialog.Accepted:
		filename = dialog.selectedFiles()[0]
		filename = to_native_path(filename)
		return filename
	else:
		return None

def save_as_filter(parent,my_filter):
	selected_filter = ""
	dialog = QFileDialog(parent)
	dialog.setWindowTitle(_("Save as"))
	dialog.setNameFilter(my_filter)
	dialog.setAcceptMode(QFileDialog.AcceptSave)
	if dialog.exec_() == QDialog.Accepted:
		filename = dialog.selectedFiles()[0]
		s=dialog.selectedNameFilter()
		if s.count("(*")==1:
			s=s.split("(*")[1]
			s=s[:-1]

			if filename.endswith(s)==False:
				filename=filename+s
			else:
				filename=filename

		return filename
	else:
		return None

def open_as_filter(parent,my_filter,path=""):
	selected_filter = ""
	if path=="":
		open_path=os.getcwd()
	else:
		open_path=path

	dialog = QFileDialog(parent,_("Open file"))
	dialog.setDirectory(open_path)
	print(">>>>>",open_path)
	dialog.setNameFilter(my_filter)
	dialog.setAcceptMode(QFileDialog.AcceptOpen)
	if dialog.exec_() == QDialog.Accepted:
		filename = dialog.selectedFiles()[0]
		s=dialog.selectedNameFilter()
		if s.count("(*")==1:
			s=s.split("(*")[1]
			s=s[:-1]

			if filename.endswith(s)==False:
				filename=filename+s
			else:
				filename=filename

		return filename
	else:
		return None
	
def save_as_jpg(parent):
	return save_as_filter(parent,"jpg (*.jpg)")


def save_as_image(parent):
	return save_as_filter(parent,"png (*.png);;jpg (*.jpg);;gnuplot (*.)")
