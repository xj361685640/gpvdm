#!/usr/bin/env python2.7
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


#import os, sys
import locale
import gettext
from cal_path import get_lang_path


current_locale, encoding = locale.getdefaultlocale()
#print get_lang_path()
locale_path = get_lang_path()
#print locale_path, current_locale
if current_locale==None:
	print "No local language set assuming en_US"
	current_locale="en_US"

language = gettext.translation ('gpvdm', locale_path, [current_locale] , fallback=True)
language.install()

def yes_no(a):
	if a.lower() in ("ja","yes", "true", "t", "1"):
		return True
	elif a.lower() in ("nein","no", "false", "f", "0"):
		return False
	else:
		return a
