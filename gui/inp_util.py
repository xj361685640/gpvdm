#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012 Roderick C. I. MacKenzie
#
#	roderick.mackenzie@nottingham.ac.uk
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


import sys
import os
import shutil
import signal

def inp_search_token_value(lines, token):

	for i in range(0, len(lines)):
		if lines[i]==token:
			return lines[i+1]

	return False

def inp_search_token_value_multiline(lines, token):
	ret=[]
	for i in range(0, len(lines)):
		if lines[i]==token:
			pos=i+1
			while (lines[pos][0]!="#"):
				ret.append(lines[pos])
				pos=pos+1

			return ret

	return False

def inp_merge(dest,src):
	ret=[]
	for i in range(0,len(dest)):
		if dest[i].startswith("#") and dest[i]!="#ver" and dest[i]!="#end":
			lookfor=dest[i]
			found=False
			for ii in range(0,len(src)):
				if src[ii]==lookfor:
					#print "Found",dest_lines[i],orig_lines[ii]
					dest[i+1]=src[ii+1]
					found=True
					break
			if found==False:
				ret.append("Warning: token "+lookfor+" not found in archive")

	return ret

