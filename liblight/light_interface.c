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

#include <dlfcn.h>

#include "util.h"
#include "inp.h"
#include "light_interface.h"
#include "const.h"
#include "device.h"
#include "dump_ctrl.h"
#include "config.h"
#include "cal_path.h"
#include "lang.h"
#include "log.h"
#include "dll_interface.h"
#include "sim.h"

static int unused __attribute__ ((unused));

static gdouble last_Psun = -1000.0;
static gdouble last_laser_eff = -1000.0;
static gdouble last_wavelength_laser = -1000.0;

void light_load_dlls(struct simulation *sim, struct light *in,
		     struct device *cell)
{
	char lib_path[200];
	char lib_name[100];

	printf_log(sim, _("Light initialization\n"));

	sprintf(lib_name, "%s.so", in->mode);

	join_path(2, lib_path, get_plugins_path(sim), lib_name);
	printf_log(sim, "I want to open %s %s %s\n", lib_path,
		   get_plugins_path(sim), lib_name);

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

	(*in->fn_set_interface) (cell->root_dll_interface);
	(*in->light_ver) ();
	(*in->fn_init) ();
}

void light_solve_and_update(struct simulation *sim, struct device *cell,
			    struct light *in, gdouble laser_eff_in)
{
	int i = 0;

	if (in->disable_transfer_to_electrical_mesh == FALSE) {
		if (fabs(in->device_ylen - cell->ylen) > 0.01e-9) {
			ewe(sim,
			    "The electrical mesh (%.9le) and the optical mesh (%.9le) don't match. %le",
			    cell->ylen, in->device_ylen);
		}
	}

	in->laser_eff = laser_eff_in;

	if ((last_laser_eff != in->laser_eff) || (last_Psun != in->Psun)
	    || (last_wavelength_laser != in->laser_wavelength)) {
		light_solve_optical_problem(sim, in);
		last_laser_eff = in->laser_eff;
		last_Psun = in->Psun;
		last_wavelength_laser = in->laser_wavelength;
	}

	light_dump_1d(sim, in, in->laser_pos, "");

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

void light_init(struct light *in, struct device *cell)
{
	last_Psun = -1.0;
	last_laser_eff = -1.0;
	last_wavelength_laser = -1.0;
	in->laser_wavelength = -1.0;
	in->laser_pos = -1;
	in->laser_wavelength = -1.0;
	in->lstart = -1.0;
	in->spotx = -1.0;
	in->spoty = -1.0;
	in->pulseJ = -1.0;
	in->pulse_width = -1.0;
}

void light_load_config(struct simulation *sim, struct light *in)
{
	light_load_config_file(sim, in);
	light_load_epitaxy(sim, in, "optics_epitaxy.inp");
	light_load_materials(sim, in);
	light_memory(in);
	light_init_mesh(in);
}

int light_solve_lam_slice(struct light *in, int lam)
{
	return (*in->fn_solve_lam_slice) (in, lam);
}

void light_free(struct simulation *sim, struct light *in)
{
	light_free_memory(sim, in);
	dlclose(in->lib_handle);
}
