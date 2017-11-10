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
import shutil

from util import gpvdm_delete_file
from inp import inp_get_token_value
from scan_tree import tree_load_flat_list
from util import copy_scan_dir

from scan_tree import tree_load_program
from scan_tree import tree_gen

from server_io import server_find_simulations_to_run
from util_zip import read_lines_from_archive

import i18n
_ = i18n.language.gettext

def scan_ml_build_vector(sim_dir):
	vectors=[]
	out=open(os.path.join(sim_dir,"vectors.dat"),'wb')

	for root, dirs, files in os.walk(sim_dir):
		for name in files:
			if name=="measure.dat":
				full_name=os.path.join(root, name)
				lines=read_lines_from_archive(os.path.join(root,"sim.gpvdm"),name)
				if lines[0].count("nan")==0 and lines[0].count("inf")==0:
					out.write(str.encode(root[len(sim_dir):]+" "+str(lines[0])+"\n"))
	out.close()
