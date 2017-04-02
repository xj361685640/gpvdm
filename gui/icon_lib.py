#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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
import os
from PyQt5.QtGui import QIcon
from cal_path import get_icon_path
from win_lin import running_on_linux
from inp import inp_get_token_value
from util import str2bool

use_theme=None
def QIcon_load(name,size=-1):
	global use_theme
	if use_theme==None:
		use_theme=inp_get_token_value(os.path.join(os.getcwd(),"config.inp") , "#gui_use_icon_theme")
		if use_theme==None:
			use_theme=False
		else:
			use_theme=str2bool(use_theme)

	if running_on_linux()==True and use_theme==True:
		image=QIcon()
		if image.hasThemeIcon(name)==True:
			return image.fromTheme(name)

	return QIcon(get_icon_path(name,size=size))
