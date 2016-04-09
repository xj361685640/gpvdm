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
#import shutil
from clone import gpvdm_clone
from export_as import export_as
from import_archive import import_archive
from util import gpvdm_copy_src
#import fnmatch
#import logging
#import time
from import_archive import clean_scan_dirs
from ver import ver
from import_archive import import_scan_dirs
from udp_server import udp_server
from udp_client import udp_client
from make_man import make_man
from scan_tree import tree_load_program
from scan_tree import tree_gen
from scan_item import scan_item_load
#from scan_item import scan_items_index_item
from server import server
from cal_path import get_exe_command
#import gtk
from plot_state import plot_state
from plot_io import plot_load_info
from scan_plot import scan_gen_plot_data
from server import server_find_simulations_to_run
from clean_sim import clean_sim_dir
from ver import ver_sync_ver
from device_lib import device_lib_replace
import i18n
_ = i18n.language.gettext

def check_params(argv,token,values):
	for i in range(0,len(argv)):
			if argv[i]==token:
				if (i+values)<len(argv):
					return True
				else:
					print _("More parameters needed see --help for info")

				sys.exit(0)

def command_args(argc,argv):
	if argc>=2:
		if argv[1]=="--help":
			print _("Usage: gpvdm [option] src_file dest_file")
			print ""
			print _("Options:")
			print _("\t--version\tdisplays the current version")
			print _("\t--help\t\tdisplays the help")
			print _("\t--export\texport a simulation to a gz file")
			print _("\t--import\timport a simulation from a .gpvdm file")
			print _("\t--patch\tpatch an .gpvdm file with an older .gpvdm file")
			print _("\t\t\tusage --import abc.gpvdm ./path/to/output/ ")
			print _("\t--clone\t\tgenerate a clean simulation in the current directory")
			print _("\t--clean\t\tcleans the current simulation directory deleting .dat files but not  scan dirs")
			print _("\t--dump-tab (output file)\t\tdumps simulation parameters as jpg")
			print _("\t--import-scandirs\t\tOnly imports the scan directories")
			print _("\t--clean-scandirs\t\tDeletes the content of all scan dirs")
			print _("\t--scan-plot\t\truns an oplot file")
			print _("\t\t\tusage --scan-plot /path/to/oplot/file.oplot ")
			print _("\t--run-scan\t\truns a scan")
			print _("\t\t\tusage --run-scan /path/containing/base/files/ /path/to/scan/dir/ ")
			print _("\t--sync-ver\t\truns a scan")
			print _("\t\t\tchanges the version of input file")
			print _("\t--replace\t\treplaces file in device lib")
			print "\t\t\t"
			print ""
			print _("Additional information about gpvdm is available at http://www.gpvdm.com.")
			print ""
			print _("Report bugs to: roderick.mackenzie@nottingham.ac.uk")
			sys.exit(0)

		if 	check_params(argv,"--version",0)==True:
			print ver()
			sys.exit(0)
		if check_params(argv,"--import-scandirs",1)==True:
			import_scan_dirs(os.getcwd(),argv[2])
			exit(0)
		if check_params(argv,"--replace",1)==True:
			device_lib_replace(argv[2])
			exit(0)
		if check_params(argv,"--export",1)==True:
			export_as(argv[2])
			sys.exit(0)
		if check_params(argv,"--dump-tab",1)==True:
			export_as(argv[2])
			sys.exit(0)
		if check_params(argv,"--import",1)==True:
			import_archive(argv[2],os.path.join(os.getcwd(),"sim.gpvdm"),False)
			sys.exit(0)
		if check_params(argv,"--patch",2)==True:
			import_archive(argv[2],argv[3],True)
			sys.exit(0)
		if check_params(argv,"--clone",0)==True:
			gpvdm_clone(os.getcwd(),True)
			sys.exit(0)
		if check_params(argv,"--clone-src",1)==True:
			gpvdm_copy_src(argv[2])
			sys.exit(0)

		#if check_params(argv,"--file_info",0)==True:
		#	data=plot_data()
		#	data.dump_file()
		#	sys.exit(0)
		if check_params(argv,"--clean",0)==True:
			clean_sim_dir()
			sys.exit(0)
		if check_params(argv,"--clean-scandirs",0)==True:
			clean_scan_dirs(os.getcwd())
			sys.exit(0)

		if check_params(argv,"--server",0)==True:
			obj=udp_server()
			obj.start()

		if check_params(argv,"--client",0)==True:
			client=udp_client()
			client.init()

		if check_params(argv,"--make-man",1)==True:
			make_man()
			sys.exit(0)

		if check_params(argv,"--sync-ver",0)==True:
			ver_sync_ver()
			sys.exit(0)

		if check_params(argv,"--run-scan",2)==True:
			scan_dir_path=argv[3]	#program file
			program_list=[]
			base_dir=argv[2]				#base dir
			exe_command   =  get_exe_command()
			scan_item_load(os.path.join(scan_dir_path,"scan_items.inp"))
			tree_load_program(program_list,scan_dir_path)

			watch_dir=os.path.join(os.getcwd(),scan_dir_path)
			#print program_list,pwd,scan_dir_path
			#sys.exit(0)
			#print pwd,scan_dir_path
			#print os.getcwd(),os.path.join(scan_dir_path)
			#tree_gen(program_list,os.getcwd(),os.path.join(os.getcwd(),"suns"))
			flat_list=[]
			tree_gen(flat_list,program_list,base_dir,scan_dir_path)
			commands=[]
			server_find_simulations_to_run(commands,scan_dir_path)
			myserver=server()
			myserver.init(watch_dir)
			myserver.clear_cache()
			for i in range(0, len(commands)):
				myserver.add_job(commands[i])
				print "Adding job"+commands[i]
			myserver.simple_run(exe_command)

			sys.exit(0)

		if check_params(argv,"--scan-plot",1)==True:
			plot_token=plot_state()
			oplot_file=argv[2]
			if plot_load_info(plot_token,oplot_file)==True:
				print "file0=",plot_token.file0,"<"
				plot_files, plot_labels, save_file = scan_gen_plot_data(plot_token,os.path.dirname(oplot_file))
				print "written data to",save_file
			else:
				print "Problem loading oplot file"
			sys.exit(0)


