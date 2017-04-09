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


#import sys
import os
from inp import inp_load_file
from token_lib import tokens

#import shutil

check_list=[]

def scan_items_clear():
	global check_list
	check_list=[]

class scan_item:
	name=""
	token=""
	filename=""
	line=""

def scan_item_add(file_name,token,text_info,line):
	global check_list
	check_list.append(scan_item())
	listpos=len(check_list)-1
	text_info=text_info.replace("<sub>","")
	text_info=text_info.replace("</sub>","")
	check_list[listpos].name=os.path.join(os.path.splitext(file_name)[0],text_info)
	check_list[listpos].filename=file_name
	check_list[listpos].token=token
	check_list[listpos].line=line

def scan_items_populate_from_known_tokens():
	my_token_lib=tokens().get_lib()
	for i in range(0,len(my_token_lib)):
		if my_token_lib[i].file_name!="":
			scan_item_add(my_token_lib[i].file_name,my_token_lib[i].token,my_token_lib[i].info,1)
	
def scan_item_save(file_name):
	global check_list
	f = open(file_name,'w')
	f.write(str(len(check_list))+"\n")
	for i in range(0,len(check_list)):
		f.write(check_list[i].name+"\n")
		f.write(check_list[i].filename+"\n")
		f.write(check_list[i].token+"\n")
		f.write(str(check_list[i].line)+"\n")
	f.close()

def scan_remove_file(file_name):
	global check_list
	new_list=[]
	for i in range(0,len(check_list)):
		if 	check_list[i].filename!=file_name:
			new_list.append(check_list[i])

	check_list=new_list


def scan_items_get_file(item):
	global check_list
	for i in range(0,len(check_list)):
		if check_list[i].name==item:
			return check_list[i].filename

	return "notknown"

def scan_items_get_token(item):
	global check_list
	for i in range(0,len(check_list)):
		if check_list[i].name==item:
			return check_list[i].token

	return "notknown"

def scan_items_lookup_item(filename,token):
	global check_list
	for i in range(0,len(check_list)):
		if check_list[i].filename==filename and check_list[i].token==token:
			return check_list[i].name

	return "notknown"

def scan_items_get_list():
	global check_list
	return check_list

def scan_items_index_item(item):
	global check_list
	for i in range(0,len(check_list)):
		if check_list[i].name==item:
			return i

	return -1

def scan_populate_from_file(filename):
	lines=[]
	inp_load_file(lines,filename)

	my_token_lib=tokens()

	for i in range(0, len(lines)):
		token=lines[i]
		if len(token)>0:
			if token[0]=="#":
				result=my_token_lib.find(token)
				if result!=False:
					if scan_items_index_item(token)==-1:
						scan_item_add(filename,token,result.info,1)

