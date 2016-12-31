#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2016 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
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

from clone import gpvdm_clone
from export_as import export_as
from import_archive import import_archive
from util import gpvdm_copy_src

from import_archive import clean_scan_dirs
from ver import ver
from ver import version
from import_archive import import_scan_dirs
from make_man import make_man
from scan_tree import tree_load_program
from scan_tree import tree_gen

from server import server
from cal_path import get_exe_command
from plot_state import plot_state
from plot_io import plot_load_info
from scan_plot import scan_gen_plot_data
from server_io import server_find_simulations_to_run
from clean_sim import clean_sim_dir
from ver import ver_sync_ver
from code_ctrl import enable_cluster
from win_lin import running_on_linux
from inp import inp_update
from device_lib import device_lib_replace

import i18n
_ = i18n.language.gettext

import argparse
parser = argparse.ArgumentParser(epilog=_("Additional information about gpvdm is available at")+" https://www.gpvdm.com"+"\n"+_("Report bugs to:")+" roderick.mackenzie@nottingham.ac.uk")
parser.add_argument("--version", help=_("displays the current version"), action='store_true')
parser.add_argument("--ver", help=_("displays the current version"), action='store_true')
parser.add_argument("--replace", help=_("replaces file in device lib"), nargs=1)
parser.add_argument("--clean", help=_("cleans the current simulation directory deleting .dat files but not  scan dirs"), action='store_true')
parser.add_argument("--export", help=_("export a simulation to a gz file"), nargs=1)
parser.add_argument("--syncver", help=_("Synchronizes the saved file version to that of the source code."), action='store_true')
parser.add_argument("--makeman", help=_("Generate the manual pages referring to the output files.."), action='store_true')
parser.add_argument("--importscandirs", help=_("Only imports the scan directories."), nargs=1)
parser.add_argument("--cleanscandirs", help=_("Deletes the content of all scan directories."), nargs=1)
parser.add_argument("--patch", help=_("Patch a .gpvdm file with an older .gpvdm file."), nargs=2)
parser.add_argument("--importfile", help=_("usage --import abc.gpvdm ./path/to/output/ "), nargs=2)
parser.add_argument("--dumptab", help=_("Dumps simulation parameters as jpg, usage: --dump-tab output_path"), nargs=1)
parser.add_argument("--clone", help=_("Generate a clean simulation in the current directory"), action='store_true')
parser.add_argument("--clonesrc", help=_("Clone the source code."), action='store_true')
parser.add_argument("--editvalue", help=_("edits a value in a .gpvdm archive. Usage --edit-value /path/to/sim.gpvdm #token_to_change new_value "), nargs=3)
parser.add_argument("--scanplot", help=_("Runs an oplot file, usage --scanplot /path/to/oplot/file.oplot "), nargs=1)
parser.add_argument("--run-scan", help=_("Runs a scan, usage --run-scan /path/containing/base/files/ /path/to/scan/dir/ "), nargs=2)

args = parser.parse_args()




def command_args(argc,argv):
	if argc>=2:
		if args.version:
			print(version())
			sys.exit(0)
		elif args.ver:
			print(ver())
			sys.exit(0)
		elif args.syncver:
			ver_sync_ver()
			sys.exit(0)
		elif args.importscandirs:
			import_scan_dirs(os.getcwd(),args.importscandirs[0])
			exit(0)
		elif args.replace:
			device_lib_replace(args.replace[0])
			exit(0)
		elif args.clean:
			clean_sim_dir()
			sys.exit(0)
		elif args.export:
			export_as(args.export[0])
			sys.exit(0)
		elif args.makeman:
			make_man()
			sys.exit(0)
		elif args.cleanscandirs:
			clean_scan_dirs(os.getcwd())
			sys.exit(0)
		elif args.importfile:
			import_archive(args.importfile[0],os.path.join(os.getcwd(),"sim.gpvdm"),False)
			sys.exit(0)
		elif args.dumptab:
			export_as(args.dumptab[0])
			sys.exit(0)
		elif args.patch:
			import_archive(args.patch[0],args.patch[1],True)
			sys.exit(0)
		elif args.clone:
			gpvdm_clone(os.getcwd(),True)
			sys.exit(0)
		elif args.clonesrc:
			gpvdm_copy_src(clone-src[0])
			sys.exit(0)
		elif args.editvalue:
			inp_update(args.editvalue[0], args.editvalue[1], args.editvalue[2])
			sys.exit(0)
		elif args.scanplot:
			plot_token=plot_state()
			oplot_file=args.scan-plot[0]
			if plot_load_info(plot_token,oplot_file)==True:
				print("file0=",plot_token.file0,"<")
				plot_files, plot_labels, save_file = scan_gen_plot_data(plot_token,os.path.dirname(oplot_file))
				print("written data to",save_file)
			else:
				print("Problem loading oplot file")
			sys.exit(0)
		if args.run-scan:
			scan_dir_path=args.run-scan[1]	#program file
			program_list=[]
			base_dir=args.run-scan[0]				#base dir
			exe_command   =  get_exe_command()
			tree_load_program(program_list,scan_dir_path)

			watch_dir=os.path.join(os.getcwd(),scan_dir_path)
			#print(program_list,pwd,scan_dir_path)
			#sys.exit(0)
			#print(pwd,scan_dir_path)
			#print(os.getcwd(),os.path.join(scan_dir_path))
			#tree_gen(program_list,os.getcwd(),os.path.join(os.getcwd(),"suns"))
			flat_list=[]
			tree_gen(flat_list,program_list,base_dir,scan_dir_path)
			commands=[]
			server_find_simulations_to_run(commands,scan_dir_path)
			myserver=server()
			myserver.init(watch_dir)
			myserver.clear_cache()
			for i in range(0, len(commands)):
				myserver.add_job(commands[i],"")
				print("Adding job"+commands[i])
			myserver.simple_run(exe_command)

			sys.exit(0)
			
		#if check_params(argv,"--file_info",0)==True:
		#	data=plot_data()
		#	data.dump_file()
		#	sys.exit(0)




