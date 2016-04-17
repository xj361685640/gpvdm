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
#include <log.h>
#include <device.h>
#include <fit.h>
#include <advmath.h>
#include <plot.h>

static int unused __attribute__ ((unused));

int main(int argc, char *argv[])
{
	struct simulation sim;
	sim_init(&sim);
	int log_level = 0;
	set_logging_level(&sim, log_level_screen);
	cal_path(&sim);

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

//solver_test();
//printf("rod\n");
//solver_ld_test();
//exit(0);
	setlocale(LC_MESSAGES, "");
	bindtextdomain("gpvdm", get_lang_path(&sim));
	textdomain("gpvdm");

	timer_init(0);
	timer_init(1);
	dbus_init();

	set_ewe_lock_file("", "");

	char pwd[1000];
	if (getcwd(pwd, 1000) == NULL) {
		ewe(&sim, "IO error\n");
	}

	remove("snapshots.zip");
	remove("light_dump.zip");

	hard_limit_init(&sim);

	set_plot_script_dir(pwd);

//set_plot_script_dir(char * in)

	if (geteuid() == 0) {
		ewe(&sim, "Don't run me as root!\n");
	}

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
		strcpy(sim.output_path,
		       get_arg_plusone(argv, argc, "--outputpath"));
	}

	if (scanarg(argv, argc, "--inputpath") == TRUE) {
		strcpy(sim.input_path,
		       get_arg_plusone(argv, argc, "--inputpath"));
	}

	char name[200];
	struct inp_file inp;
	inp_init(&sim, &inp);
	inp_load_from_path(&sim, &inp, sim.input_path, "ver.inp");
	inp_check(&sim, &inp, 1.0);
	inp_search_string(&sim, &inp, name, "#core");
	inp_free(&sim, &inp);

	if (strcmp(name, gpvdm_ver) != 0) {
		printf
		    ("Software is version %s and the input files are version %s\n",
		     gpvdm_ver, name);
		exit(0);
	}

	gui_start();
	if (scanarg(argv, argc, "--optics") == FALSE)
		server_init(&sim, &globalserver);

	if (scanarg(argv, argc, "--lock") == TRUE) {
		server_set_dbus_finish_signal(&globalserver,
					      get_arg_plusone(argv, argc,
							      "--lock"));
	}

	int ret = 0;

	gen_dos_fd_gaus_fd(&sim);

	server_add_job(&sim, &globalserver, sim.output_path, sim.input_path);
	print_jobs(&sim, &globalserver);

	ret = server_run_jobs(&sim, &globalserver);

	server_shut_down(&sim, &globalserver);

	hard_limit_free(&sim);
	if (ret != 0) {
		return 1;
	}
	return 0;
}
