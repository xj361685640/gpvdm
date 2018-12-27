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

## @package report_error
#  Report an error using a thread.
#

import os
from win_lin import running_on_linux
from threading import Thread
from ver import ver_core
from ver import ver_subver

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

from urllib.parse import quote
from PyQt5.QtCore import QTimer


class report_error(QWidget):
	reported = pyqtSignal(bool)

	def __init__(self):
		QWidget.__init__(self)
		self.error=""

	def tx_error(self,n):
		page="http://www.gpvdm.com/bug.html?ver_core="+ver_core()+"."+ver_subver()+"error="+quote(self.error)
		message=get_data_from_web(page)
		print("from web:",message)
		if message.startswith("ok")==True:
			self.reported.emit(True)
		else:
			self.reported.emit(False)

	def set_error(self,error):
		self.error=error

	def start(self):
		p = Thread(target=self.tx_error, args=(10,))
		p.daemon = True
		p.start()		


