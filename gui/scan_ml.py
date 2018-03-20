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
from server import server_break
from util_zip import zip_lsdir
from util_zip import extract_dir_from_archive

import zipfile

tindex=[]

def tindex_add(token,value):
	global tindex
	for i in range(0,len(tindex)):
		if tindex[i][0]==token:
			return
	tindex.append([token,value])		
	return

def tindex_dump(fname):
	global tindex

	f = open(fname, 'w')
	for i in range(0,len(tindex)):
		f.write(tindex[i][0]+" "+tindex[i][1]+'\n')
	f.close()

def get_vectors(path,dir_name,file_name,dolog=False,div=1.0,fabs=False,do_norm=False):
	base=os.path.join(path,dir_name)


	lines=read_lines_from_archive(os.path.join(base,"sim.gpvdm"),file_name)

	if lines==False:
		print("\n\nbase>>",base,"\n\n")
		return False
	
	if lines[0].count("nan")==0 and lines[0].count("inf")==0:
		ret=lines[0].split()

		if do_norm==True:
			mi=1e6
			for i in range(0,len(ret)):
				if float(ret[i])<mi:
					mi=float(ret[i])
			div=mi

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

def is_it_good(token,x):
	if token=="#mueffe" and x>1e-4:
		return True

	if token=="#mueffh" and x>1e-4:
		return True

	if token=="#Etrape" and x<50e-3:
		return True

	if token=="#Etraph" and x<50e-3:
		return True

	if token=="#Ntrape" and x<1e24:
		return True

	if token=="#Ntraph" and x<1e24:
		return True

	if token=="#srhsigman_e" and x<1e-20:
		return True

	if token=="#srhsigmap_e" and x<1e-20:
		return True

	if token=="#srhsigman_h" and x<1e-20:
		return True

	if token=="#srhsigmap_h" and x<1e-20:
		return True

	if token=="#Rshunt" and x>1e5:
		return True

	if token=="#Rcontact" and x<15:
		return True

	return False
	
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

def scan_ml_both_good(file_name,token0,token1):
	v0=float(inp_get_token_value(file_name, token0))
	v1=float(inp_get_token_value(file_name, token1))
	ret=token0+"_"+token1[1:]+"_good\n"
	vector="0 1\n"
	if is_it_good(token0,v0)==True and is_it_good(token1,v1)==True:
		vector="1 0\n"
	
	ret=ret+vector
	return ret

def scan_ml_one_good(file_name,token0,token1):
	v0=float(inp_get_token_value(file_name, token0))
	v1=float(inp_get_token_value(file_name, token1))
	ret=token0+"_"+token1[1:]+"_one_good\n"
	vector="0 1\n"
	if is_it_good(token0,v0)==True and is_it_good(token1,v1)==False:
		vector="1 0\n"
		
	if  is_it_good(token0,v0)==False and is_it_good(token1,v1)==True:
		vector="1 0\n"
	
	ret=ret+vector
	return ret

def scan_ml_both_bad(file_name,token0,token1):
	v0=float(inp_get_token_value(file_name, token0))
	v1=float(inp_get_token_value(file_name, token1))
	ret=token0+"_"+token1[1:]+"_bad\n"
	vector="0 1\n"
	if is_it_good(token0,v0)==False and is_it_good(token1,v1)==False:
		vector="1 0\n"
	
	ret=ret+vector
	return ret

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

def scan_ml_build_token_vectors(file_name,token0,token1,vector,min_max=""):

	if token0==token1 and min_max=="min":
		return ""

	if token0==token1 and min_max=="max":
		return ""

	if min_max=="":
		val=float(inp_get_token_value(file_name, token0))
	elif min_max=="max":
		v0=float(inp_get_token_value(file_name, token0))
		v1=float(inp_get_token_value(file_name, token1))
		val=v0
		if v1>v0:
			val=v1
	elif min_max=="min":
		v0=float(inp_get_token_value(file_name, token0))
		v1=float(inp_get_token_value(file_name, token1))
		val=v0
		if v1<v0:
			val=v1
	elif min_max=="avg":
		v0=float(inp_get_token_value(file_name, token0))
		v1=float(inp_get_token_value(file_name, token1))
		val=(v0+v1)/2.0

	v=[]
	s=""
	for i in range(0,len(vector)):
		full_token=token0+"_"+min_max+"_"+str(i)

		tindex_add(full_token,">"+str(vector[i]))
		s=s+full_token+"\n"
		if val>vector[i]:
			s=s+"0 1\n"
		else:
			s=s+"1 0\n"

	return s


def scan_ml_build_token_abs(file_name,token0,token1,error,min_max=""):

	if token0==token1 and min_max=="min":
		return ""

	if token0==token1 and min_max=="max":
		return ""

	if min_max=="":
		val=float(inp_get_token_value(file_name, token0))
	elif min_max=="max":
		v0=float(inp_get_token_value(file_name, token0))
		v1=float(inp_get_token_value(file_name, token1))
		val=v0
		if v1>v0:
			val=v1
	elif min_max=="min":
		v0=float(inp_get_token_value(file_name, token0))
		v1=float(inp_get_token_value(file_name, token1))
		val=v0
		if v1<v0:
			val=v1
	elif min_max=="avg":
		v0=float(inp_get_token_value(file_name, token0))
		v1=float(inp_get_token_value(file_name, token1))
		val=(v0+v1)/2.0

	v=[]
	s=""

	s=token0+"_"+min_max+"_abs\n"
	s=s+str(val)+"\n"

	full_token=token0+"_"+min_max+"_abs"

	tindex_add(full_token,str(error))

	return s

def scan_ml_build_vector(sim_dir):


	out=open(os.path.join(sim_dir,"vectors.dat"),'wb')
	for archive_name in os.listdir(sim_dir):
		if archive_name.startswith("archive")==True and archive_name.endswith(".zip")==True:
			progress_window=progress_class()
			progress_window.show()
			progress_window.start()

			archive_path=os.path.join(sim_dir,archive_name)

			zf = zipfile.ZipFile(archive_path, 'r')
			dirs=zip_lsdir(archive_path,zf=zf,sub_dir="/")

			items=len(dirs)
			print(items,archive_path,dirs)
			for i in range(0,len(dirs)):
				tmp_dir="/dev/shm/gpvdm"
				if os.path.isdir(tmp_dir)==True:
					shutil.rmtree(tmp_dir)

				os.mkdir(tmp_dir)

				extract_dir_from_archive(tmp_dir,"",dirs[i],zf=zf)
	
				full_name=tmp_dir
				written=False
				#print(dirs[i])
				while(1):
					v="#ml_id\n"
					v=v+dirs[i]+"\n"

					v=v+"#ml_input_jv_dark\n"
					ret=get_vectors(full_name,"0.0","measure_jv.dat",dolog=True)
					if ret==False:
						print("ml_input_jv_dark")
						break
					v=v+ret+"\n"

					v=v+"#ml_input_jv_light\n"
					ret=get_vectors(full_name,"1.0","measure_jv.dat",div=1e2)
					if ret==False:
						print("ml_input_jv_ligh")
						break
					v=v+ret+"\n"
			
					v=v+"#ml_input_tpc_neg\n"
					ret=get_vectors(full_name,"TPC","measure_tpc.dat",fabs=True,dolog=True)
					if ret==False:
						break
					v=v+ret+"\n"

					v=v+"#ml_input_tpc\n"
					ret=get_vectors(full_name,"TPC_0","measure_tpc.dat",fabs=True,dolog=True)
					if ret==False:
						break
					v=v+ret+"\n"

					v=v+"#ml_input_tpc_neg_norm\n"
					ret=get_vectors(full_name,"TPC","measure_tpc.dat",fabs=True,dolog=True,do_norm=True)
					if ret==False:
						break
					v=v+ret+"\n"

					v=v+"#ml_input_tpc_norm\n"
					ret=get_vectors(full_name,"TPC_0","measure_tpc.dat",fabs=True,dolog=True,do_norm=True)
					if ret==False:
						break
					v=v+ret+"\n"

					v=v+"#ml_input_tpc_ideal\n"
					ret=get_vectors(full_name,"TPC_ideal","measure_tpc.dat",fabs=True,dolog=True)
					if ret==False:
						break
					v=v+ret+"\n"

					v=v+"#ml_input_tpc_ideal_norm\n"
					ret=get_vectors(full_name,"TPC_ideal","measure_tpc.dat",fabs=True,dolog=True,do_norm=True)
					if ret==False:
						break
					v=v+ret+"\n"

					v=v+"#ml_input_tpv\n"
					ret=get_vectors(full_name,"TPV","measure_tpv.dat",fabs=True)
					if ret==False:
						break
					v=v+ret+"\n"

					v=v+"#ml_input_celiv\n"
					ret=get_vectors(full_name,"CELIV","measure_celiv.dat",fabs=True)
					if ret==False:
						break
					v=v+ret+"\n"

					v=v+get_vectors_binary(full_name,"1.0")
					for min_max in ["min","max","avg"]:
						a=scan_ml_build_token_vectors(os.path.join(full_name,"dos0.inp"),"#Etrape","#Etraph",[40e-3,50e-3,60e-3,70e-3,80e-3,90e-3,100e-3],min_max=min_max)
						v=v+a

						a=scan_ml_build_token_vectors(os.path.join(full_name,"dos0.inp"),"#mueffe","#mueffh",[1e-9, 1e-8, 1e-7,1e-6,1e-5,1e-4,1e-3],min_max=min_max)
						v=v+a

						a=scan_ml_build_token_vectors(os.path.join(full_name,"dos0.inp"),"#Ntraph","#Ntrape",[1e20,1e21,1e22,1e23,1e24,1e25,1e26,1e27],min_max=min_max)
						v=v+a

					a=scan_ml_build_token_vectors(os.path.join(full_name,"parasitic.inp"),"#Rshunt","#Rshunt",[1e2,1e3,1e4,1e5,1e6,1e7],min_max="avg")
					v=v+a

					#a=scan_ml_build_token_abs(os.path.join(full_name,"parasitic.inp"),"#Rshunt","#Rshunt",min_max="avg")
					#v=v+a

					a=scan_ml_build_token_vectors(os.path.join(full_name,"parasitic.inp"),"#Rcontact","#Rcontact",[5,10,15,20,25,30,35,40],min_max="avg")
					v=v+a

					a=scan_ml_build_token_vectors(os.path.join(full_name,"1.0","sim_info.dat"),"#jv_pmax_tau","#jv_pmax_tau",[1e-1,1e-2,1e-3,1e-4,1e-5,1e-6,1e-7],min_max="avg")
					v=v+a

					a=scan_ml_build_token_vectors(os.path.join(full_name,"1.0","sim_info.dat"),"#jv_pmax_mue","#jv_pmax_mue",[1e-9, 1e-8, 1e-7,1e-6,1e-5,1e-4,1e-3],min_max="avg")
					v=v+a


					out.write(str.encode(v))
					written=True
					break

				if written==False:
					print("Error",dirs[i])
				progress_window.set_fraction(float(i)/float(len(dirs)))
				progress_window.set_text(dirs[i])

				#if server_break()==True:
				#	break
				process_events()
				#return
			progress_window.stop()

