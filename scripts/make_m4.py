#!/usr/bin/python
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

import os
import sys
import argparse

def make_m4(hpc=False, win=False,usear=False):
	config_files=[]
	link_libs=""

	config_files.append("")

	config_files.append("lang")

	config_files.append("libi")
	link_libs=link_libs+" -lgpvdm_i"

	config_files.append("librpn")
	link_libs=link_libs+" -lgpvdm_rpn"

	config_files.append("libmemory")
	link_libs=link_libs+" -lgpvdm_memory"

	config_files.append("libdos")
	link_libs=link_libs+" -lgpvdm_dos"

	config_files.append("liblight")
	link_libs=link_libs+" -lgpvdm_light"

	config_files.append("libmeasure")
	link_libs=link_libs+" -lgpvdm_measure"

	config_files.append("libcontacts")
	link_libs=link_libs+" -lgpvdm_contacts"

	config_files.append("lib")
	link_libs=link_libs+" -lgpvdm_lib"

	config_files.append("libdump")
	link_libs=link_libs+" -lgpvdm_dump"

	config_files.append("libdumpctrl")
	link_libs=link_libs+" -lgpvdm_dumpctrl"

	config_files.append("libserver")
	link_libs=link_libs+" -lgpvdm_server"

	config_files.append("libmesh")
	link_libs=link_libs+" -lgpvdm_mesh"


	if win==False:
		if os.path.isdir("libfit"):
			config_files.append("libfit")
			link_libs=link_libs+" -lgpvdm_fit"


	if os.path.isdir("libfdtd"):
		config_files.append("libfdtd")
		link_libs=link_libs+" -lgpvdm_fdtd -lOpenCL"

	for root, dirs, files in os.walk("./plugins"):
		for file in files:
			if file.endswith("Makefile.am"):
				name=os.path.join(root, file)[2:-12]
				config_files.append(name)

	config_files.append("src")
	if hpc==False:
		config_files.append("images/16x16")
		config_files.append("images/32x32")
		config_files.append("images/48x32")
		config_files.append("images/64x64")

		config_files.append("images/icons/16x16")
		config_files.append("images/icons/32x32")
		config_files.append("images/icons/48x48")
		config_files.append("images/icons/64x64")
		config_files.append("images/icons/128x128")
		config_files.append("images/icons/256x256")
		config_files.append("images/icons/512x512")

		config_files.append("css")
		config_files.append("html")
		#config_files.append("cluster")
		#config_files.append("cluster_")
		config_files.append("docs")
		config_files.append("desktop")
		if win==False:
			config_files.append("man")

	f = open("config_files.m4", "w")
	for i in range(0,len(config_files)):
		f.write( "AC_CONFIG_FILES(["+os.path.join(config_files[i],"Makefile")+"])\n")

	f.close()

	f = open("make_files.m4", "w")
	f.write( "AC_SUBST(BUILD_DIRS,\"")
	for i in range(0,len(config_files)):
		f.write(config_files[i]+" ")

	f.write("\")")

	f.close()


	f = open("local_link.m4", "w")
	f.write( "AC_SUBST(LOCAL_LINK,\"")
	f.write(link_libs)
	f.write("\")")

	f.close()

	f = open("ar.m4", "w")

	if usear==True:
		f.write("AM_PROG_AR")
	else:
		f.write("")

	f.close()


if __name__ == "__main__":
	parser = argparse.ArgumentParser()
	parser.add_argument("--noar", help="no archiver for old Cent OS", action='store_true')
	parser.add_argument("--hpc", help="Set up for Nottingham hpc", action='store_true')
	parser.add_argument("--win", help="compile for windows", action='store_true')
	parser.add_argument("--verbosity", help="increase output verbosity", action='store_true')

	args = parser.parse_args()

	hpc=False
	win=False
	usear=True

	if args.hpc:
		hpc=True

	if args.win:
		win=True

	if args.noar:
		usear=False

	make_m4(hpc=hpc, win=win,usear=usear)



