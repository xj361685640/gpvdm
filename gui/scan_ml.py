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

from server_io import server_find_simulations_to_run
from util_zip import read_lines_from_archive
from math import log10

import i18n
_ = i18n.language.gettext
def get_vectors(path,dir_name,start=0,stop=-1,dolog=False,div=1.0):
	base=os.path.join(path,dir_name)
	lines=read_lines_from_archive(os.path.join(base,"sim.gpvdm"),"measure.dat")
	if lines[0].count("nan")==0 and lines[0].count("inf")==0:
		if stop==-1:
			ret=lines[0].split()[start:]
		else:
			ret=lines[0].split()[start:stop]

		n=[]
		for i in range(0,len(ret)):
			r=float(ret[i])/div
			if dolog==True:
				r=log10(r)
			n.append(r)
		print(n)
		return n


def scan_ml_build_vector(sim_dir):
	vectors=[]
	out=open(os.path.join(sim_dir,"vectors.dat"),'wb')

	dirs=os.listdir(sim_dir)
	for i in range(0,len(dirs)):
		full_name=os.path.join(sim_dir, dirs[i])
		if os.path.isdir(full_name)==True:
			v=[]
			v.extend(get_vectors(full_name,"0.0",stop=9,dolog=True))
			v.extend(get_vectors(full_name,"1.0",stop=9,div=1e2))
			v.extend(get_vectors(full_name,"1.0",start=9))
			s=""
			for ii in range(0,len(v)):
				s=s+'{:e}'.format(float(v[ii]))+" "
			out.write(str.encode(dirs[i]+" "+s+"\n"))

#			lines=read_lines_from_archive(os.path.join(root,"sim.gpvdm"),name)
#			if lines[0].count("nan")==0 and lines[0].count("inf")==0:
#				
	out.close()
