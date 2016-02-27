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

static int unused __attribute__ ((unused));

void light_transfer_gen_rate_to_device(struct device *cell, struct light *in)
{
	int i = 0;
	gdouble Gn = 0.0;
	gdouble Gp = 0.0;

	if (in->align_mesh == FALSE) {
		for (i = 0; i < cell->ymeshpoints; i++) {

			Gn = inter_get_raw(in->x, in->Gn, in->points,
					   in->device_start +
					   cell->ymesh[i]) * in->Dphotoneff;
			Gp = inter_get_raw(in->x, in->Gp, in->points,
					   in->device_start +
					   cell->ymesh[i]) * in->Dphotoneff;
			cell->Gn[i] = Gn * in->electron_eff;
			cell->Gp[i] = Gp * in->hole_eff;
			cell->Habs[i] = 0.0;

		}
	} else {
		for (i = 0; i < cell->ymeshpoints; i++) {
			cell->Gn[i] =
			    in->Gn[in->device_start_i + i] * in->Dphotoneff;
			cell->Gp[i] =
			    in->Gp[in->device_start_i + i] * in->Dphotoneff;
		}

	}

}

void light_init(struct light *in, struct device *cell, char *output_path)
{
	printf_log(_("Light initialization\n"));
	in->pulse_width = 0.0;
	strcpy(in->output_path, output_path);
	strcpy(in->input_path, cell->inputpath);

	char lib_path[200];
	gdouble ver;
	gdouble temp;
	in->disable_transfer_to_electrical_mesh = FALSE;
	struct inp_file inp;
	inp_init(&inp);
	inp_load_from_path(&inp, cell->inputpath, "light.inp");
	inp_check(&inp, 1.25);

	inp_search_gdouble(&inp, &(temp), "#Psun");
	cell->Psun = fabs(temp);

	inp_search_string(&inp, in->mode, "#light_model");

	inp_search_gdouble(&inp, &(in->Dphotoneff), "#Dphotoneff");
	in->Dphotoneff = fabs(in->Dphotoneff);

	inp_search_gdouble(&inp, &(in->ND), "#NDfilter");

	inp_search_gdouble(&inp, &(temp), "#high_sun_scale");

	cell->Psun *= fabs(temp);

	inp_search_gdouble(&inp, &(ver), "#ver");

	inp_free(&inp);

	char lib_name[100];
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

	in->fn_solve_and_update =
	    dlsym(in->lib_handle, "light_dll_solve_and_update");
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
			    gdouble Psun_in, gdouble laser_eff_in)
{
	int i = 0;

	if (in->disable_transfer_to_electrical_mesh == FALSE) {
		if (fabs(in->device_ylen - cell->ylen) > 0.01e-9) {
			ewe("The electrical mesh (%.9le) and the optical mesh (%.9le) don't match. %le", cell->ylen, in->device_ylen);
		}
	}

	(*in->fn_solve_and_update) (cell, in, Psun_in, laser_eff_in);

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

void light_load_config(struct light *in)
{
	light_load_epitaxy(in, "optics_epitaxy.inp");
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
