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

from progress import progress_class
from gui_util import process_events

def get_vectors(path,dir_name,file_name,dolog=False,div=1.0,fabs=False):
	base=os.path.join(path,dir_name)
	lines=read_lines_from_archive(os.path.join(base,"sim.gpvdm"),file_name)
	if lines[0].count("nan")==0 and lines[0].count("inf")==0:
		ret=lines[0].split()

		n=[]
		for i in range(0,len(ret)):
			#print(ret[i])
			r=float(ret[i])/div

			if fabs==True:
				r=abs(r)

			if dolog==True:
				#print(r)
				if r!=0.0:
					r=log10(r)
				else:
					r=0.0

			n.append(r)
		#print(n)

		s=""
		for ii in range(0,len(n)):
			s=s+'{:e}'.format(float(n[ii]))+" "

		return s

def get_vectors_binary(path,dir_name):
	base=os.path.join(path,dir_name)
	lines=read_lines_from_archive(os.path.join(base,"sim.gpvdm"),"measure_output.dat")

	ret=lines[0].split()

	n=[]

	names=["#mueffe","#mueffh","#Etrape","#Etraph","#Ntrape","#Ntraph","#srhsigman_e","#srhsigmap_e","#srhsigman_h","#srhsigmap_h","#Rshunt","#Rcontact","#jv_pmax_tau","#jv_pmax_mue", "#jv_pmax_muh"]
	s=""
	for i in range(0,len(ret)):
		r=int(float(ret[i]))
		s=s+names[i]+"_bin\n"
		if r==1:
			s=s+"1 0\n"
		else:
			s=s+"0 1\n"

	return s

def scan_ml_build_token_vector(file_name,token,vector):
	a=float(inp_get_token_value(file_name, token))
	v=[]
	for i in range(0,len(vector)):
		v.append(0.0)
		
	if a<=vector[0]:
		v[0]=1.0
	elif a>=vector[len(vector)-1]:
		v[len(v)-1]=1.0
	else:
		for i in range(1,len(vector)-1):
			if a<vector[i]:
				v[i]=1.0
				break

	s=token+"\n"
	for i in range(0,len(v)):
		if v[i]==0.0:
			s=s+"0 "
		else:
			s=s+"1 "
	
	s=s[:-1]+"\n"

	
	vectors=[]

	return s

def scan_ml_build_vector(sim_dir):
	progress_window=progress_class()
	progress_window.show()
	progress_window.start()
	out=open(os.path.join(sim_dir,"vectors.dat"),'wb')

	dirs=os.listdir(sim_dir)
	items=len(dirs)
	for i in range(0,len(dirs)):
		full_name=os.path.join(sim_dir, dirs[i])
		print(full_name)
		if os.path.isdir(full_name)==True:
			v="#ml_id\n"
			v=v+dirs[i]+"\n"
			v=v+"#ml_input_jv_dark\n"
			v=v+get_vectors(full_name,"0.0","measure_jv.dat",dolog=True)+"\n"
			v=v+"#ml_input_jv_light\n"
			v=v+get_vectors(full_name,"1.0","measure_jv.dat",div=1e2)+"\n"
			v=v+"#ml_input_tpc_neg\n"
			v=v+get_vectors(full_name,"TPC","measure_tpc.dat",fabs=True,dolog=True)+"\n"
			v=v+"#ml_input_tpc\n"
			v=v+get_vectors(full_name,"TPC_0","measure_tpc.dat",fabs=True,dolog=True)+"\n"
			v=v+"#ml_input_tpv\n"
			v=v+get_vectors(full_name,"TPV","measure_tpv.dat",fabs=True)+"\n"
			v=v+"#ml_input_celiv\n"
			v=v+get_vectors(full_name,"CELIV","measure_celiv.dat",fabs=True)+"\n"

			v=v+get_vectors_binary(full_name,"1.0")#get_vectors(full_name,"1.0","measure_output.dat")+"\n"

			a=scan_ml_build_token_vector(os.path.join(full_name,"dos0.inp"),"#Etrape",[40e-3,80e-3,120e-3,140e-3])
			v=v+a

			a=scan_ml_build_token_vector(os.path.join(full_name,"dos0.inp"),"#Etraph",[40e-3,80e-3,120e-3,140e-3])
			v=v+a

			a=scan_ml_build_token_vector(os.path.join(full_name,"parasitic.inp"),"#Rshunt",[1e3,1e4,1e5,1e6,1e7])
			v=v+a

			a=scan_ml_build_token_vector(os.path.join(full_name,"dos0.inp"),"#mueffe",[1e-9, 1e-8, 1e-7,1e-6,1e-5,1e-4,1e-3])
			v=v+a

			out.write(str.encode(v))

			progress_window.set_fraction(float(i)/float(items))
			progress_window.set_text(full_name)

			process_events()

	progress_window.stop()
#			lines=read_lines_from_archive(os.path.join(root,"sim.gpvdm"),name)
#			if lines[0].count("nan")==0 and lines[0].count("inf")==0:
#				
	out.close()
