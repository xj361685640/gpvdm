//    General-purpose Photovoltaic Device Model - a drift diffusion base/Shockley-Read-Hall
//    model for 1st, 2nd and 3rd generation solar cells.
//    Copyright (C) 2012 Roderick C. I. MacKenzie
//
//      roderick.mackenzie@nottingham.ac.uk
//      www.roderickmackenzie.eu
//      Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <unistd.h>

#include <dlfcn.h>

#include <sys/types.h>
#include <dirent.h>
#include "util.h"
#include "inp.h"
#include "light_interface.h"
#include "const.h"
#include "device.h"
#include "dump_ctrl.h"
#include "config.h"
#include "complex_solver.h"
#include "cal_path.h"
#include "lang.h"
#include "log.h"
#include "dll_interface.h"
#include "sim.h"

static int unused __attribute__ ((unused));

static gdouble last_Psun = -1000.0;
static gdouble last_laser_eff = -1000.0;
static gdouble last_wavelength_laser = -1000.0;

void light_load_dlls(struct light *in, struct device *cell, char *output_path)
{
	light_init(in);
	char lib_path[200];
	char lib_name[100];

	printf_log(_("Light initialization\n"));
	strcpy(in->output_path, output_path);
	strcpy(in->input_path, cell->inputpath);

	sprintf(lib_name, "%s.so", in->mode);

	join_path(2, lib_path, get_light_path(), lib_name);
	printf_log("I want to open %s %s %s\n", lib_path, get_light_path(),
		   lib_name);

	char *error;

	in->lib_handle = dlopen(lib_path, RTLD_LAZY);

	if (!in->lib_handle) {
		fprintf(stderr, "%s\n", dlerror());
		exit(0);
	}

	in->fn_init = dlsym(in->lib_handle, "light_dll_init");
	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(0);
	}

	in->fn_free = dlsym(in->lib_handle, "light_dll_free");
	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(0);
	}

	in->fn_solve_lam_slice =
	    dlsym(in->lib_handle, "light_dll_solve_lam_slice");
	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(0);
	}

	in->light_ver = dlsym(in->lib_handle, "light_dll_ver");
	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(0);
	}

	in->fn_set_interface = dlsym(in->lib_handle, "set_interface");
	if ((error = dlerror()) != NULL) {
		fprintf(stderr, "%s\n", error);
		exit(0);
	}

	dll_interface_fixup();
	(*in->fn_set_interface) (dll_get_interface());
	(*in->light_ver) ();
	(*in->fn_init) ();
}

void light_solve_and_update(struct device *cell, struct light *in,
			    gdouble laser_eff_in)
{
	int i = 0;

	if (in->disable_transfer_to_electrical_mesh == FALSE) {
		if (fabs(in->device_ylen - cell->ylen) > 0.01e-9) {
			ewe("The electrical mesh (%.9le) and the optical mesh (%.9le) don't match. %le", cell->ylen, in->device_ylen);
		}
	}

	in->laser_eff = laser_eff_in;

	if ((last_laser_eff != in->laser_eff) || (last_Psun != in->Psun)
	    || (last_wavelength_laser != in->laser_wavelength)) {
		light_solve_optical_problem(in);
		last_laser_eff = in->laser_eff;
		last_Psun = in->Psun;
		last_wavelength_laser = in->laser_wavelength;
	}

	light_dump_1d(in, in->laser_pos, "");

	light_transfer_gen_rate_to_device(cell, in);

	if (in->flip_field == TRUE) {
		gdouble *Gn =
		    (gdouble *) malloc(cell->ymeshpoints * sizeof(gdouble));
		gdouble *Gp =
		    (gdouble *) malloc(cell->ymeshpoints * sizeof(gdouble));

		for (i = 0; i < cell->ymeshpoints; i++) {
			Gn[i] = cell->Gn[i];
			Gp[i] = cell->Gp[i];

		}

		getchar();
		for (i = 0; i < cell->ymeshpoints; i++) {
			cell->Gn[cell->ymeshpoints - i - 1] = Gn[i];
			cell->Gp[cell->ymeshpoints - i - 1] = Gp[i];

		}

		free(Gn);
		free(Gp);
	}

}

void light_init(struct light *in)
{
	last_Psun = -1000.0;
	last_laser_eff = -1000.0;
	last_wavelength_laser = -1000.0;
	in->laser_wavelength = 532e-9;
	in->laser_pos = 1;
	in->laser_wavelength = 1.0;
	in->lstart = 1.0;
	in->spotx = 1.0;
	in->spoty = 1.0;
	in->pulseJ = 1.0;
	in->pulse_width = 1.0;
}

void light_load_config(struct light *in)
{
	light_load_config_file(in);
	light_load_epitaxy(in, "optics_epitaxy.inp");
	light_load_materials(in);
	light_memory(in);
	light_init_mesh(in);
}

int light_solve_lam_slice(struct light *in, int lam)
{
	return (*in->fn_solve_lam_slice) (in, lam);
}

void light_free(struct light *in)
{
	light_free_memory(in);
	dlclose(in->lib_handle);
}
