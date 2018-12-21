#    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
#    model for 1st, 2nd and 3rd generation solar cells.
#    Copyright (C) 2012-2016 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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


## @package command_args
#  Handle command line arguments.
#

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

from server import base_server
from cal_path import get_exe_command
from dat_file_class import dat_file
from plot_io import plot_load_info
from scan_plot import scan_gen_plot_data
from server_io import server_find_simulations_to_run
from clean_sim import clean_sim_dir
from ver import ver_sync_ver
from code_ctrl import enable_cluster
from win_lin import running_on_linux
from inp import inp_update_token_value
from device_lib_io import device_lib_replace
from device_lib_io import device_lib_delete
from cal_path import test_arg_for_sim_file
from cal_path import set_sim_path
from import_archive import patch_file
from inp import inp_encrypt
from util_zip import archive_decompress
from scan_io import build_scan
from scan_io import scan_build_nested_simulation
from scan_tree import tree_load_flat_list

from scan_item import scan_items_clear
from scan_item import scan_items_populate_from_known_tokens
from scan_item import scan_items_populate_from_files

from scan_ml import scan_ml_build_vector

from scan_io import scan_archive

from gui_enable import set_gui
from gui_enable import gui_get

from util_zip import archive_unpack
from materials_io import archive_materials

import i18n
_ = i18n.language.gettext

import argparse
parser = argparse.ArgumentParser(epilog=_("Additional information about gpvdm is available at")+" https://www.gpvdm.com"+"\n"+_("Report bugs to:")+" roderick.mackenzie@nottingham.ac.uk")
parser.add_argument("--version", help=_("displays the current version"), action='store_true')
parser.add_argument("--ver", help=_("displays the current version"), action='store_true')
parser.add_argument("--replace", help=_("replaces file in device lib --replace file.inp path_to_device_lib"), nargs=2)
parser.add_argument("--delete", help=_("deletes file in device lib --delete file.inp path_to_device_lib"), nargs=2)
parser.add_argument("--clean", help=_("cleans the current simulation directory deleting .dat files but not  scan dirs"), action='store_true')
parser.add_argument("--export", help=_("export a simulation to a gz file"), nargs=1)
parser.add_argument("--syncver", help=_("Synchronizes the saved file version to that of the source code."), action='store_true')
parser.add_argument("--makeman", help=_("Generate the manual pages referring to the output files.."), action='store_true')
parser.add_argument("--importscandirs", help=_("Only imports the scan directories."), nargs=1)
parser.add_argument("--cleanscandirs", help=_("Deletes the content of all scan directories."), nargs=1)
parser.add_argument("--patch", help=_("Patch a .gpvdm file with an older .gpvdm file."), nargs=2)
parser.add_argument("--patchfile", help=_("Patch an .inp file with an older .inp file. usage --patchfile dest_file base_file input_file"), nargs=3)
parser.add_argument("--importfile", help=_("usage --import abc.gpvdm ./path/to/output/ "), nargs=2)
parser.add_argument("--dumptab", help=_("Dumps simulation parameters as jpg, usage: --dump-tab output_path"), nargs=1)
parser.add_argument("--clone", help=_("Generate a clean simulation in the current directory"), action='store_true')
parser.add_argument("--clonesrc", help=_("Clone the source code."), action='store_true')
parser.add_argument("--editvalue", help=_("edits a value in a .gpvdm archive. Usage --edit-value /path/to/sim.gpvdm #token_to_change new_value "), nargs=3)
parser.add_argument("--scanplot", help=_("Runs an oplot file, usage --scanplot /path/to/oplot/file.oplot "), nargs=1)
parser.add_argument("--runscan", help=_("Runs a scan, usage --runscan /path/to/scan/dir/ "), nargs=1)
parser.add_argument("--buildscan", help=_("Builds a scan, usage --buildscan /path/to/scan/dir/ /path/containing/base/files/"), nargs=2)
parser.add_argument("--buildnestedscan", help=_("Builds a nested scan, usage --buildnestedscan /path/to/scan/dir/ sim_to_nest"), nargs=2)
parser.add_argument("--load", help=_("Loads a simulation --load /path/containing/simulation/sim.gpvdm"), nargs=1)
parser.add_argument("--encrypt", help=_("Encrypt a gpvdm file --file sim.gpvdm"), nargs=1)
parser.add_argument("--extract", help=_("Extract the sim.gpvdm archive --extract"), action='store_true')
parser.add_argument("--scanarchive", help=_("Compress a scandir --scanarchive path_to_scan_dir"), nargs=1)
parser.add_argument("--scanbuildvectors", help=_("Build vectors from scan dir --scanbuildvectors path_to_scan_dir"), nargs=1)
parser.add_argument("--unpack", help=_("Unpacks a gpvdm archive --unpack path/to/gpvdm_file.gpvdm"), nargs=1)
parser.add_argument("--matcompress", help=_("Compresses the materials dir"), action='store_true')


if test_arg_for_sim_file()==False:
	args = parser.parse_args()

def command_args(argc,argv):
	if test_arg_for_sim_file()!=False:
		return

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
			device_lib_replace(args.replace[0],dir_name=args.replace[1])
			exit(0)
		elif args.delete:
			device_lib_delete(args.delete[0],dir_name=args.delete[1])
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
		elif args.patchfile:
			patch_file(args.patchfile[0],args.patchfile[1],args.patchfile[2])
			sys.exit(0)
		elif args.clone:
			gpvdm_clone(os.getcwd(),copy_dirs=True)
			sys.exit(0)
		elif args.matcompress:
			archive_materials(os.path.join(os.getcwd(),"materials"))
			sys.exit(0)
		elif args.clonesrc:
			gpvdm_copy_src(clone-src[0])
			sys.exit(0)
		elif args.editvalue:
			inp_update_token_value(args.editvalue[0], args.editvalue[1], args.editvalue[2])
			sys.exit(0)
		elif args.load:
			set_sim_path(os.path.dirname(args.load[0]))
			#print("a")
		elif args.encrypt:
			inp_encrypt(args.encrypt[0])
			sys.exit(0)
		elif args.extract:
			archive_decompress("sim.gpvdm")
			sys.exit(0)
		elif args.scanplot:
			plot_token=dat_file()
			oplot_file=args.scan-plot[0]
			if plot_load_info(plot_token,oplot_file)==True:
				print("file0=",plot_token.file0,"<")
				plot_files, plot_labels, save_file = scan_gen_plot_data(plot_token,os.path.dirname(oplot_file))
				print("written data to",save_file)
			else:
				print("Problem loading oplot file")
			sys.exit(0)

		if args.unpack:
			archive_unpack(args.unpack[0])
			sys.exit()
		if args.runscan:
			set_gui(False)
			scan_dir_path=args.runscan[0]	#program file
			exe_command=get_exe_command()
			program_list=tree_load_program(scan_dir_path)
	
			watch_dir=os.path.join(os.getcwd(),scan_dir_path)

			commands=[]
			#server_find_simulations_to_run(commands,scan_dir_path)
			commands=tree_load_flat_list(scan_dir_path)
			print(commands)
			
			myserver=base_server()
			myserver.base_server_init(watch_dir)

			for i in range(0, len(commands)):
				myserver.base_server_add_job(commands[i],"")
				print("Adding job"+commands[i])

			myserver.print_jobs()
			myserver.simple_run()
			#simple_run(exe_command)

			sys.exit(0)

		if args.scanarchive:
			set_gui(False)
			scan_archive(args.scanarchive[0])
			sys.exit(0)

		if args.buildscan:
			set_gui(False)
			scan_items_clear()
			scan_items_populate_from_known_tokens()
			scan_items_populate_from_files()

			scan_dir_path=args.buildscan[0]	#program file
			base_dir=args.buildscan[1]				#base dir

			build_scan(scan_dir_path,base_dir)

			sys.exit(0)

		if args.scanbuildvectors:
			set_gui(False)
			scan_ml_build_vector(args.scanbuildvectors[0])
			sys.exit(0)

		if args.buildnestedscan:
			set_gui(False)

			scan_items_clear()
			scan_items_populate_from_known_tokens()
			scan_items_populate_from_files()

			scan_dir_path=os.path.abspath(args.buildnestedscan[0])	#program file
			sim_to_nest=os.path.abspath(args.buildnestedscan[1])	#program file
			scan_build_nested_simulation(scan_dir_path,os.path.join(os.getcwd(),sim_to_nest))

			sys.exit(0)

