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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include "util.h"
#include "const.h"
#include "light.h"
#include "device.h"
#include "const.h"
#include "dump.h"
#include "config.h"
#include "inp.h"
#include "util.h"
#include "hard_limit.h"
#include "epitaxy.h"
#include "lang.h"
#include "log.h"

static int unused __attribute__ ((unused));

void light_free_epitaxy(struct light *in)
{
	int i = 0;
	free(in->thick);
	free(in->G_percent);

	for (i = 0; i < in->layers; i++) {
		free(in->material_dir_name[i]);
	}

	free(in->material_dir_name);

}

void light_load_epitaxy(struct light *in, char *epi_file)
{
	struct epitaxy my_epitaxy;

	epitaxy_load(&my_epitaxy, "epitaxy.inp");

	int i = 0;

	in->layers = my_epitaxy.layers;
	in->thick = (gdouble *) malloc(in->layers * sizeof(gdouble));
	in->G_percent = (gdouble *) malloc(in->layers * sizeof(gdouble));

	in->material_dir_name = (char **)malloc(in->layers * sizeof(char *));
	for (i = 0; i < in->layers; i++) {
		in->material_dir_name[i] = (char *)malloc(300 * sizeof(char));
	}

	in->ylen = 0.0;
	in->device_ylen = 0.0;

	for (i = 0; i < my_epitaxy.layers; i++) {
		in->thick[i] = my_epitaxy.width[i];
		in->thick[i] = fabs(in->thick[i]);

		strcpy(in->material_dir_name[i], my_epitaxy.mat_file[i]);

		in->ylen += in->thick[i];
	}

	in->device_start = epitaxy_get_device_start(&my_epitaxy);
	in->device_start_layer = epitaxy_get_device_start_i(&my_epitaxy);
	in->device_ylen = epitaxy_get_electrical_length(&my_epitaxy);

}

void light_load_config_file(struct light *in)
{
	char path_temp[1000];
	gdouble temp = 0.0;
	struct inp_file inp;

	in->disable_transfer_to_electrical_mesh = FALSE;

	join_path(2, path_temp, in->output_path, "light_dump");
	remove_dir(path_temp);

	join_path(2, in->config_file, in->output_path, "light.inp");

	printf_log("Load optics config %s\n", in->config_file);

	inp_init(&inp);
	inp_load_from_path(&inp, in->input_path, "light.inp");

	printf("%s\n", inp.data);
	inp_check(&inp, 1.28);

	inp_search_string(&inp, in->suns_spectrum_file, "#sun");

	inp_search_int(&inp, &in->align_mesh, "#alignmesh");

	inp_search_int(&inp, &in->points, "#meshpoints");

	inp_search_int(&inp, &in->flip_field, "#flip_field");

	inp_search_int(&inp, &in->lpoints, "#lpoints");

	inp_search_gdouble(&inp, &in->lstart, "#lstart");

	inp_search_gdouble(&inp, &in->lstop, "#lstop");

	inp_search_gdouble(&inp, &(in->Eg), "#Eg");

	inp_search_gdouble(&inp, &(in->electron_eff), "#electron_eff");
	in->electron_eff = fabs(in->electron_eff);

	inp_search_gdouble(&inp, &(in->hole_eff), "#hole_eff");
	in->hole_eff = fabs(in->hole_eff);

	inp_search_gdouble(&inp, &(temp), "#Psun");
	in->Psun = fabs(temp);

	inp_search_string(&inp, in->mode, "#light_model");

	inp_search_gdouble(&inp, &(in->Dphotoneff), "#Dphotoneff");
	in->Dphotoneff = fabs(in->Dphotoneff);

	inp_search_gdouble(&inp, &(in->ND), "#NDfilter");

	inp_search_gdouble(&inp, &(temp), "#high_sun_scale");

	in->Psun *= fabs(temp);

	inp_free(&inp);

}
