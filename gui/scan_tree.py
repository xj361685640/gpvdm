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
import shutil
import glob
import random
from random import randint

from inp import inp_update_token_value
from scan_item import scan_items_index_item
from inp import inp_get_token_value
from inp import inp_get_token_array
from util import str2bool
from util import get_cache_path
from cal_path import get_materials_path
from clone import clone_materials
from scan_item import scan_items_get_file
from scan_item import scan_items_get_token

#windows
from gui_util import tab_add

copy_materials=False

def tree_load_flat_list(sim_dir):
	config=[]
	file_name=os.path.join(sim_dir,'flat_list.inp')

	f = open(file_name)
	lines = f.readlines()
	f.close()

	for i in range(0, len(lines)):
		lines[i]=lines[i].rstrip()

	number=int(lines[0])

	for i in range(1,number+1):
		lines[i]=os.path.join(sim_dir,lines[i])
		if os.path.isdir(lines[i]):
			config.append(lines[i])

	return config

def tree_save_flat_list(sim_dir,flat_list):
	config=[]
	file_name=os.path.join(sim_dir,'flat_list.inp')

	a = open(file_name, "w")
	a.write(str(len(flat_list))+"\n")
	for i in range(0,len(flat_list)):
		rel_dir=flat_list[i][len(sim_dir):]

		if rel_dir[0]=="/":
			rel_dir=rel_dir[1:]

		if rel_dir[0]=="\\":
			rel_dir=rel_dir[1:]

		a.write(rel_dir+"\n")

	a.close()

	return config

def tree_load_program(program_list,sim_dir):

	file_name=os.path.join(sim_dir,'gpvdm_gui_config.inp')

	if os.path.isfile(file_name)==True:
		f=open(file_name)
		config = f.readlines()
		f.close()

		for ii in range(0, len(config)):
			config[ii]=config[ii].rstrip()

		pos=0
		mylen=int(config[0])
		pos=pos+1

		for i in range(0, mylen):
			program_list.append([config[pos],config[pos+1],config[pos+3], config[pos+4]])
			pos=pos+6

def tree_load_config(sim_dir):
	global copy_materials

	copy_materials=inp_get_token_value(os.path.join(sim_dir,"scan_config.inp"),"#copy_materials")
	if copy_materials==None:
		copy_materials="False"
	copy_materials=str2bool(copy_materials)


def tree_gen(flat_simulation_list,program_list,base_dir,sim_dir):
	sim_dir=os.path.abspath(sim_dir)	# we are about to traverse the directory structure better to have the abs path
	print("here",program_list)
	tree_items=[[],[],[]]	#file,token,values,opp
	for i in range(0,len(program_list)):
		print(i,program_list[i][0],program_list[i][1],program_list[i][2],program_list[i][3])
		if program_list[i][3]=="scan":
			tree_items[0].append(program_list[i][0])
			tree_items[1].append(program_list[i][1])
			values=program_list[i][2]
			#This expands a [ start stop step ] command.
			if len(values)>0:
				if values[0]=='[' and values[len(values)-1]==']':
					values=values[1:len(values)-1]
					data=values.split()
					if len(data)==3:
						a=float(data[0])
						b=float(data[1])
						c=float(data[2])
						values=""
						pos=a
						while pos<b:
							values=values+str(pos)+" "
							pos=pos+c
						values=values[0:len(values)-1]

			tree_items[2].append(values)
			#tree_items[3].append(program_list[i][3])

	print("tree items=",tree_items)
	ret=tree(flat_simulation_list,program_list,tree_items,base_dir,0,sim_dir,"","")

	return ret

def tree_apply_mirror(program_list):
	print(program_list)
	for i in range(0, len(program_list)):
		if program_list[i][2]=="mirror":
			f=scan_items_get_file(program_list[i][3])
			t=scan_items_get_token(program_list[i][3])
			src_value=inp_get_token_value(f, t)
			inp_update_token_value(program_list[i][0], program_list[i][1], src_value)

	return True

def tree_apply_constant(program_list):
	for i in range(0, len(program_list)):
		if program_list[i][3]=="constant":
			inp_update_token_value(program_list[i][0], program_list[i][1], program_list[i][2])

	return True

def tree_apply_python_script(program_list):
	for i in range(0, len(program_list)):
		if program_list[i][3]=="python_code":

			ret=""
			exec(program_list[i][2])
			inp_update_token_value(program_list[i][0], program_list[i][1], ret)
			print(os.getcwd())
			print("Replace",program_list[i][0], program_list[i][1], ret)

	return True

def copy_simulation(base_dir,cur_dir):
	global copy_materials

	f_list=glob.iglob(os.path.join(base_dir, "*.inp"))
	for inpfile in f_list:
		shutil.copy(inpfile, cur_dir)

	shutil.copy(os.path.join(base_dir, "sim.gpvdm"), cur_dir)

	print(">>>>>>>>>>>>>>>>>>>>>>>materials>>>>>>>>",copy_materials)
	if copy_materials==True:
		clone_materials(cur_dir)

def tree(flat_simulation_list,program_list,tree_items,base_dir,level,path,var_to_change,value_to_change):
	print(level,tree_items)
	values=tree_items[2][level]
	values=values.split()

	if tree_items[0][level]=="notknown":
		return False

	if tree_items[1][level]=="notknown":
		return False

	pass_var_to_change=var_to_change+" "+str(level)
	print(pass_var_to_change)
	for ii in values:
		cur_dir=os.path.join(path,ii)

		if not os.path.exists(cur_dir):
			os.makedirs(cur_dir)

		pass_value_to_change=value_to_change+" "+ii

		if ((level+1)<len(tree_items[0])):
				ret=tree(flat_simulation_list,program_list,tree_items,base_dir,level+1,cur_dir,pass_var_to_change,pass_value_to_change)
				if ret==False:
					return False
		else:
			flat_simulation_list.append(cur_dir)
			new_values=pass_value_to_change.split()
			pos=pass_var_to_change.split()

			config_file=os.path.join(cur_dir,"sim.gpvdm")
			if os.path.isfile(config_file)==False:	#Don't build a simulation over something that exists already
				copy_simulation(base_dir,cur_dir)

				os.chdir(cur_dir)

				for i in range(0, len(pos)):
					inp_update_token_value(tree_items[0][int(pos[i])], tree_items[1][int(pos[i])], new_values[i])
					print("updating",tree_items[0][int(pos[i])], tree_items[1][int(pos[i])], new_values[i])

				if tree_apply_constant(program_list)==False:
					return False

				if tree_apply_python_script(program_list)==False:
					return False

				tree_apply_mirror(program_list)

				inp_update_token_value("dump.inp", "#plot", "0")

		if level==0:
			f = open(os.path.join(cur_dir,'scan.inp'),'w')
			f.write("data")
			f.close()
	return True
