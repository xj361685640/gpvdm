#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2016 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
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


from cal_path import get_image_file_path
import os

#qt
from PyQt5.QtWidgets import QMainWindow, QTextEdit, QAction, QApplication,QTableWidgetItem,QComboBox, QMessageBox, QDialog, QDialogButtonBox, QFileDialog
from PyQt5.uic import loadUi
from PyQt5.QtGui import QPixmap


#windows
from cal_path import get_ui_path

from gpvdm_select import gpvdm_select

class dlg_get_text():
	def __init__(self,text,default,image):
		#QDialog.__init__(self)
		self.ui = loadUi(os.path.join(get_ui_path(),"question.ui"))
		self.ui.label.setText(text)
		self.ui.text.setText(default)
		pixmap = QPixmap(os.path.join(get_image_file_path(),image))
		self.ui.image.setPixmap(pixmap)
		ret=self.ui.exec_()
		if ret==True:
			self.ret=self.ui.text.text()
		else:
			self.ret=None

def save_as_gpvdm(parent):
	dialog = QFileDialog(parent)
	dialog.setWindowTitle(_("Save a the simulation as"))
	dialog.setNameFilter(_("Directory"))
	dialog.setAcceptMode(QFileDialog.AcceptSave)
	dialog.setOption(QFileDialog.ShowDirsOnly, True) 
	if dialog.exec_() == QDialog.Accepted:
		filename = dialog.selectedFiles()[0]
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
	
def save_as_jpg(parent):
	return save_as_filter(parent,"jpg (*.jpg)")


def save_as_image(parent):
	return save_as_filter(parent,"png (*.png);;jpg (*.jpg);;gnuplot (*.)")

def save_as_image_inc_gnuplot(parent):
	return save_as_filter(parent,"png (*.png);;jpg (*.jpg);;gnuplot (*.)")


def process_events():
	QApplication.processEvents()

def tab_get_value(tab,y,x):
	if type(tab.cellWidget(y, x))==QComboBox:
		return tab.cellWidget(y, x).currentText()
	elif type(tab.cellWidget(y,x))==gpvdm_select:
		return tab.cellWidget(y, x).text()
	else:
		return tab.item(y, x).text()

def tab_set_value(tab,y,x,value):
	if type(tab.cellWidget(y, x))==QComboBox:
		tab.cellWidget(y, x).blockSignals(True)
		tab.cellWidget(y, x).setCurrentIndex(tab.cellWidget(y, x).findText(value))
		tab.cellWidget(y, x).blockSignals(False)
	elif type(tab.cellWidget(y,x))==gpvdm_select:
		tab.cellWidget(y, x).blockSignals(True)
		tab.cellWidget(y, x).setText(value)
		tab.cellWidget(y, x).blockSignals(False)
	else:
		item = QTableWidgetItem(str(value))
		tab.setItem(y,x,item)

def tab_get_selected(tab):
	a=tab.selectionModel().selectedRows()

	if len(a)>0:
		y=a[0].row()
	else:
		return False

	ret=[]
	
	for i in range(0,tab.columnCount()):
		ret.append(str(tab_get_value(tab,y,i)))

	return ret

def tab_move_down(tab):
	if tab.rowCount()==0:
		return

	tab.blockSignals(True)
	a=tab.selectionModel().selectedRows()

	if len(a)>0:
		a=a[0].row()

		b=a+1
		if b>=tab.rowCount():
			b=0

		av=[]
		for i in range(0,tab.columnCount()):
			av.append(str(tab_get_value(tab,a,i)))

		bv=[]
		for i in range(0,tab.columnCount()):
			bv.append(str(tab_get_value(tab,b,i)))

		for i in range(0,tab.columnCount()):
			tab_set_value(tab,b,i,str(av[i]))
			tab_set_value(tab,a,i,str(bv[i]))

		tab.selectRow(b)
		tab.blockSignals(False)
	else:
		return

def tab_insert_row(tab):
	tab.blockSignals(True)
	index = tab.selectionModel().selectedRows()

	if len(index)>0:
		pos=index[0].row()+1
	else:
		pos = tab.rowCount()

	tab.insertRow(pos)
	tab.blockSignals(False)
	return pos

	
def tab_add(tab,data):
	tab.blockSignals(True)
	index = tab.selectionModel().selectedRows()

	if len(index)>0:
		pos=index[0].row()+1
	else:
		pos = tab.rowCount()

	tab.insertRow(pos)

	for i in range(0,len(data)):
		tab.setItem(pos,i,QTableWidgetItem(data[i]))

	tab.blockSignals(False)


def tab_remove(tab):
	tab.blockSignals(True)
	index = tab.selectionModel().selectedRows()

	if len(index)>0:
		pos=index[0].row()
		tab.removeRow(pos)
	tab.blockSignals(False)

def error_dlg(parent,text):
	msgBox = QMessageBox(parent)
	msgBox.setIcon(QMessageBox.Critical)
	msgBox.setText("gpvdm error:")
	msgBox.setInformativeText(text)
	msgBox.setStandardButtons(QMessageBox.Ok )
	msgBox.setDefaultButton(QMessageBox.Ok)
	reply = msgBox.exec_()

def yes_no_dlg(parent,text):
	msgBox = QMessageBox(parent)
	msgBox.setIcon(QMessageBox.Question)
	msgBox.setText("Question")
	msgBox.setInformativeText(text)
	msgBox.setStandardButtons(QMessageBox.Yes| QMessageBox.No )
	msgBox.setDefaultButton(QMessageBox.No)
	reply = msgBox.exec_()
	if reply == QMessageBox.Yes:
		return True
	else:
		return False

def yes_no_cancel_dlg(parent,text):
	msgBox = QMessageBox(parent)
	msgBox.setIcon(QMessageBox.Question)
	msgBox.setText("Question")
	msgBox.setInformativeText(text)
	msgBox.setStandardButtons(QMessageBox.Yes| QMessageBox.No| QMessageBox.Cancel  )
	msgBox.setDefaultButton(QMessageBox.No)
	reply = msgBox.exec_()
	if reply == QMessageBox.Yes:
		return "yes"
	elif reply == QMessageBox.No:
		return "no"
	else:
		return "cancel"
