# -*- coding: utf-8 -*-
 
import os, sys
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
