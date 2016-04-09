//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie
//
//      roderick.mackenzie@nottingham.ac.uk
//      www.roderickmackenzie.eu
//      Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
//
//
// This program is free software; you can redistribute it and/or modify it
// under the terms and conditions of the GNU General Public License,
// version 2, as published by the Free Software Foundation.
//
// This program is distributed in the hope it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
// more details.

#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <unistd.h>

#include <util.h>

#include <sim.h>
#include <dos.h>
#include <server.h>
#include <light_interface.h>
#include <dump.h>
#include <complex_solver.h>
#include <license.h>
#include <inp.h>
#include <gui_hooks.h>
#include <timer.h>
#include <rand.h>
#include <hard_limit.h>
#include <patch.h>
#include <cal_path.h>
#include <lang.h>
#include <dll_interface.h>
#include <log.h>
#include <device.h>
#include <fit.h>
#include <advmath.h>
#include <plot.h>

static int unused __attribute__ ((unused));

struct device cell;

void device_init(struct device *in)
{
	in->Voc = 0.0;
	in->Jsc = 0.0;
	in->FF = 0.0;
	in->Pmax = 0.0;
	in->Pmax_voltage = 0.0;
	device_get_memory(in);

}

int main(int argc, char *argv[])
{
	if (scanarg(argv, argc, "--help") == TRUE) {
		printf
		    ("gpvdm_core - General-purpose Photovoltaic Device Model\n");
		printf(copyright);
		printf("\n");
		printf("Usage: gpvdm_core [options]\n");
		printf("\n");
		printf("Options:\n");
		printf("\n");
		printf("\t--outputpath\toutput data path");
		printf("\t--inputpath\t sets the input path\n");
		printf("\t--version\tdisplays the current version\n");
		printf("\t--zip_results\t zip the results\n");
		printf("\t--optics\t runs only optical simulation\n");
		printf("\t--cpus\t sets the number of CPUs\n");
		printf("\n");
		printf
		    ("Additional information about gpvdm is available at www.gpvdm.com.\n");
		printf("\n");
		printf
		    ("Report bugs to: roderick.mackenzie@nottingham.ac.uk\n\n");
		exit(0);
	}
	if (scanarg(argv, argc, "--version") == TRUE) {
		printf("gpvdm_core, Version %s\n", gpvdm_ver);
		printf(copyright);
		printf(this_is_free_software);
		printf
		    ("There is ABSOLUTELY NO WARRANTY; not even for MERCHANTABILITY or\n");
		printf("FITNESS FOR A PARTICULAR PURPOSE.\n");
		printf("\n");
		exit(0);
	}

	device_init(&cell);
	dll_interface_fixup(&cell);
	log_init(&cell);
	dump_ctrl_init(&cell);

//solver_test();
//printf("rod\n");
//solver_ld_test();
//exit(0);
	cal_path(&cell);
	setlocale(LC_MESSAGES, "");
	bindtextdomain("gpvdm", get_lang_path());
	textdomain("gpvdm");

	timer_init(0);
	timer_init(1);
	dbus_init();

	set_ewe_lock_file("", "");
	cell.onlypos = FALSE;
	cell.root_dll_interface = dll_get_interface();
	char pwd[1000];
	if (getcwd(pwd, 1000) == NULL) {
		ewe("IO error\n");
	}

	dump_init(&cell);

	dump_load_config(&cell);

	remove("snapshots.zip");
	remove("light_dump.zip");

	hard_limit_init();

//char path[PATH_MAX];
//char dest[PATH_MAX];
//pid_t pid = getpid();
//sprintf(path, "/proc/%d/exe", pid);

//if (readlink(path, dest, PATH_MAX) == -1)
//{
//      printf("error\n");
//      exit(1);
//}
//  char *base = strrchr(dest, '/');
//*base='/';
//*(base+1)=0;
	set_plot_script_dir(pwd);

//set_plot_script_dir(char * in)

	if (geteuid() == 0) {
		ewe("Don't run me as root!\n");
	}

	set_dump_status(dump_stop_plot, FALSE);
	set_dump_status(dump_print_text, TRUE);

	srand(time(0));
	textcolor(fg_green);
	randomprint(_
		    ("General-purpose Photovoltaic Device Model (www.gpvdm.com)\n"));
	randomprint(_
		    ("You should have received a copy of the GNU General Public License\n"));
	randomprint(_
		    ("along with this software.  If not, see www.gnu.org/licenses/.\n"));
	randomprint("\n");
	randomprint(_
		    ("If you wish to collaborate in anyway please get in touch:\n"));
	randomprint(_("roderick.mackenzie@nottingham.ac.uk\n"));
	randomprint(_("www.roderickmackenzie.eu/contact.html\n"));
	randomprint("\n");
	textcolor(fg_reset);

	globalserver.on = FALSE;
	globalserver.cpus = 1;
	globalserver.readconfig = TRUE;

	if (scanarg(argv, argc, "--outputpath") == TRUE) {
		set_output_path(get_arg_plusone(argv, argc, "--outputpath"));
	} else {
		set_output_path(pwd);
	}

	if (scanarg(argv, argc, "--inputpath") == TRUE) {
		set_input_path(get_arg_plusone(argv, argc, "--inputpath"));
	} else {
		set_input_path(get_output_path());
	}

	dump_load_config(&cell);

	if (scanarg(argv, argc, "--onlypos") == TRUE) {
		cell.onlypos = TRUE;
	}

	char name[200];
	struct inp_file inp;
	inp_init(&inp);
	inp_load_from_path(&inp, get_input_path(), "ver.inp");
	inp_check(&inp, 1.0);
	inp_search_string(&inp, name, "#core");
	inp_free(&inp);

	if (strcmp(name, gpvdm_ver) != 0) {
		printf
		    ("Software is version %s and the input files are version %s\n",
		     gpvdm_ver, name);
		exit(0);
	}

	gui_start();
	if (scanarg(argv, argc, "--optics") == FALSE)
		server_init(&globalserver);

	if (scanarg(argv, argc, "--lock") == TRUE) {
		server_set_dbus_finish_signal(&globalserver,
					      get_arg_plusone(argv, argc,
							      "--lock"));
	}

	int ret = 0;

	if (scanarg(argv, argc, "--optics") == TRUE) {
		gui_start();
		struct light two;
		light_init(&two, &cell);
		light_load_config(&two);
		light_load_dlls(&two, &cell);
		//light_set_dx(&cell.mylight,cell.ymesh[1]-cell.ymesh[0]);

		two.disable_transfer_to_electrical_mesh = TRUE;
		set_dump_status(dump_lock, FALSE);
		set_dump_status(dump_optics, TRUE);
		set_dump_status(dump_optics_verbose, TRUE);
		gdouble Psun;
		inp_init(&inp);
		inp_load_from_path(&inp, pwd, "light.inp");
		inp_search_gdouble(&inp, &(Psun), "#Psun");
		inp_free(&inp);
		light_set_sun(&two, 1.0);
		light_solve_and_update(&cell, &two, 0.0);
		light_dump(&two);
		light_free(&two);
		complex_solver_free();
	} else {

		gen_dos_fd_gaus_fd();

		server_add_job(&globalserver, cell.outputpath, cell.inputpath);
		print_jobs(&globalserver);

		ret = server_run_jobs(&globalserver);

	}

	server_shut_down(&globalserver);

	if (scanarg(argv, argc, "--zip_results") == TRUE) {
		printf("zipping results\n");
		int ret;
		char temp[200];
		DIR *dir = opendir("snapshots");
		if (dir) {
			closedir(dir);
			ret =
			    system("zip -r -j -q snapshots.zip ./snapshots/*");
			if (ret == -1) {
				printf("tar returned error\n");
			}

			join_path(2, temp, cell.outputpath, "snapshots");
			remove_dir(temp);

		}

		dir = opendir("light_dump");
		if (dir) {
			closedir(dir);
			ret =
			    system
			    ("zip -r -j -q light_dump.zip ./light_dump/*");
			if (ret == -1) {
				printf("tar returned error\n");
			}

			join_path(2, temp, cell.outputpath, "light_dump");
			remove_dir(temp);

		}

	}

	hard_limit_free();
	if (ret != 0) {
		return 1;
	}
	return 0;
}
