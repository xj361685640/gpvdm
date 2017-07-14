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
from cal_path import get_inp_file_path
from cal_path import get_share_path
from cal_path import get_bin_path
from inp import inp_load_file
from i18n import yes_no
from inp_util import inp_search_token_value

store_enable_webupdates=False
store_enable_webbrowser=False
store_enable_cluster=False
store_enable_betafeatures=False

def enable_webupdates():
	return store_enable_webupdates

def enable_webbrowser():
	return store_enable_webbrowser

def enable_cluster():
	return store_enable_cluster

def enable_betafeatures():
	return store_enable_betafeatures



def code_ctrl_load():
	lines=[]
	global store_enable_webupdates
	global store_enable_webbrowser
	global store_enable_cluster
	global store_enable_betafeatures
	lines=inp_load_file(os.path.join(get_inp_file_path(),"ver.inp"),archive="base.gpvdm")
	if lines!=False:
		store_enable_webupdates=yes_no(inp_search_token_value(lines, "#enable_webupdates"))
		store_enable_webbrowser=yes_no(inp_search_token_value(lines, "#enable_webbrowser"))
		store_enable_cluster=yes_no(inp_search_token_value(lines, "#enable_cluster"))
		store_enable_betafeatures=yes_no(inp_search_token_value(lines, "#enable_betafeatures"))
		if os.path.isdir(os.path.join(get_inp_file_path(),"enablebeta"))==True:
			store_enable_betafeatures=True
	else:
		print("Can not load ver.inp file")
		store_enable_webupdates=False
		store_enable_webbrowser=False
		store_enable_cluster=False
		store_enable_betafeatures=False




