#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2017 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
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

## @package uid_gen
#  Create a uid for a user.
#


import uuid
import os
from inp import inp_load_file
from cal_path import get_exe_path
from inp import inp_save
from inp import inp_search_token_value
from os.path import expanduser
from win_lin import running_on_linux

def uid_get():
	uid=""
	if running_on_linux()==True:
		path=os.path.join(expanduser("~"),".gpvdm_uid.inp")
	else:
		path=os.path.join(get_exe_path(),"uid.inp")

	try:
		lines=[]
		found=False
		
		lines=inp_load_file(path)
		if lines!=False:
			uid=inp_search_token_value(lines, "#uid")
			found=True

		if found==False:
			uid=str(uuid.uuid4())[0:8]
			lines=[]
			lines.append("#uid")
			lines.append(uid)
			lines.append("#ver")
			lines.append("1.0")
			lines.append("#end")

			inp_save(path,lines)
	except:
		print("uid error")

	return uid

