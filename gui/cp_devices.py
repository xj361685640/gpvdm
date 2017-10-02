#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2017 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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


import os
import os.path
import argparse
import shutil
import glob
from inp import inp_get_token_value
from cp_gasses import copy_gasses
from util import str2bool

parser = argparse.ArgumentParser(epilog="copy materials database")
parser.add_argument("--copy", help="Copy materials file", nargs=2)
args = parser.parse_args()

			
if args.copy:
	src=args.copy[0]
	dest=args.copy[1]

	if not os.path.exists(dest):
		os.makedirs(dest)
				
	files=glob.glob(src+"*.gpvdm")
#	for dirpath, dirnames, filenames in os.walk(args.copy[0]):
	for i in range(0,len(files)):
		private=inp_get_token_value(os.path.join(os.path.dirname(files[i]),"info.inp"), "#private",archive=os.path.basename(files[i]))
		print(files[i],"private=",private)

		if str2bool(private)==False:
			shutil.copyfile(files[i],os.path.join(dest,os.path.basename(files[i])))
