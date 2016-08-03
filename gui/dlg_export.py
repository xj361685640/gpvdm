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



import os
#from global_objects import global_object_get
from plot_io import get_plot_file_info
from plot_state import plot_state
from util import latex_to_pygtk_subscript
from help import my_help_class
from cal_path import get_image_file_path
from export_as import export_as
from export_archive import export_archive


COL_PATH = 0
COL_PIXBUF = 1
COL_IS_DIRECTORY = 2

import i18n
_ = i18n.language.gettext

def dlg_export():
	dialog = gtk.FileChooserDialog(_("Export the simulation as"), None, gtk.FILE_CHOOSER_ACTION_SAVE,
                           (gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL, gtk.STOCK_SAVE, gtk.RESPONSE_OK))

	dialog.set_default_response(gtk.RESPONSE_OK)

	filter = gtk.FileFilter()
	filter.set_name(_("gpvdm archive input+output files"))
	filter.add_pattern("*.gpvdm")
	dialog.add_filter(filter)

	filter = gtk.FileFilter()
	filter.set_name(_("gpvdm archive input files"))
	filter.add_pattern("*.gpvdm")
	dialog.add_filter(filter)

	filter = gtk.FileFilter()
	filter.set_name(_("pdf file"))
	filter.add_pattern("*.pdf")
	dialog.add_filter(filter)

	filter = gtk.FileFilter()
	filter.set_name(_("jpg image"))
	filter.add_pattern("*.jpg")
	dialog.add_filter(filter)

	filter = gtk.FileFilter()
	filter.set_name(_("tex file"))
	filter.add_pattern("*.tex")
	dialog.add_filter(filter)

	filter = gtk.FileFilter()
	filter.set_name(_("optical materials database"))
	filter.add_pattern("*.zip")
	dialog.add_filter(filter)


	response = dialog.run()
	if response == gtk.RESPONSE_OK:
		file_name=dialog.get_filename()
		filter=dialog.get_filter()
		dialog.destroy()
		print "rod",filter.get_name()

		if filter.get_name()==_("gpvdm archive input+output files"):
			export_archive(file_name,True)
		elif filter.get_name()==_("gpvdm archive input files"):
			export_archive(file_name,False)
		elif filter.get_name()==_("optical materials database"):
			export_materials(file_name)
		elif filter.get_name()==_("pdf file") or _("jpg image") or _("tex file"):
			if os.path.splitext(file_name)[1]=="":
				export_as(file_name)
			else:
				export_as(file_name+filter.get_name())

	
	elif response == gtk.RESPONSE_CANCEL:
		print _("Closed, no files selected")
		dialog.destroy()
