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
#include "epitaxy.h"
#include "inp.h"
#include "util.h"
#include "const.h"

void epitaxy_load(struct epitaxy *in, char *file)
{
	int i;
	char dos_file[20];
	char pl_file[20];
	struct inp_file inp;
	in->electrical_layers = 0;

	inp_init(&inp);
	inp_load(&inp, file);
	inp_check(&inp, 1.2);
	inp_reset_read(&inp);
	inp_get_string(&inp);
	sscanf(inp_get_string(&inp), "%d", &(in->layers));

	if (in->layers > 20) {
		ewe("Too many material layers\n");
	}

	if (in->layers < 1) {
		ewe("No material layers\n");
	}

	for (i = 0; i < in->layers; i++) {
		inp_get_string(&inp);	//layer name
		strcpy(in->name[i], inp_get_string(&inp));
		sscanf(inp_get_string(&inp), "%Le", &(in->width[i]));
		in->width[i] = fabs(in->width[i]);
		strcpy(in->mat_file[i], inp_get_string(&inp));
		strcpy(dos_file, inp_get_string(&inp));
		strcpy(pl_file, inp_get_string(&inp));

		char temp[20];
		if (strcmp(dos_file, "none") != 0) {
			strcpy(temp, dos_file);
			strcat(temp, ".inp");
			in->electrical_layer[i] = TRUE;
			if (inp_isfile(temp) != 0) {
				ewe("dos file %s does not exist", temp);
			}
			strcpy(in->dos_file[in->electrical_layers], dos_file);

			strcpy(temp, pl_file);
			strcat(temp, ".inp");
			if (inp_isfile(temp) != 0) {
				ewe("pl file %s does not exist", temp);
			}
			strcpy(in->pl_file[in->electrical_layers], pl_file);

			in->electrical_layers++;
		} else {
			in->electrical_layer[i] = FALSE;
		}

	}

	char *ver = inp_get_string(&inp);
	if (strcmp(ver, "#ver") != 0) {
		ewe("No #ver tag found in file\n");
	}

	inp_free(&inp);
}

gdouble epitaxy_get_electrical_length(struct epitaxy *in)
{
	int i = 0;
	gdouble tot = 0.0;

	for (i = 0; i < in->layers; i++) {
		if (in->electrical_layer[i] == TRUE) {
			tot += in->width[i];
		}
	}
//if (tot>300e-9)
//{
//      ewe("Can't simulate structures bigger than 300 nm\n");
//}
	return tot;
}

gdouble epitaxy_get_optical_length(struct epitaxy * in)
{
	int i = 0;
	gdouble tot = 0.0;

	for (i = 0; i < in->layers; i++) {
		tot += in->width[i];
	}

	return tot;
}

int epitaxy_get_optical_material_layer(struct epitaxy *in, gdouble pos)
{
	int i = 0;
	gdouble layer_end = 0.0;
	for (i = 0; i < in->layers; i++) {
		layer_end += in->width[i];

		if (pos < layer_end) {
			return i;
		}

	}

	return -1;
}

int epitaxy_get_electrical_material_layer(struct epitaxy *in, gdouble pos)
{
	int i = 0;
	gdouble layer_end = 0.0;
	int electrical_layer = 0;

	for (i = 0; i < in->layers; i++) {
		if (in->electrical_layer[i] == TRUE) {
			layer_end += in->width[i];

			if (pos < layer_end) {
				return electrical_layer;
			}
			electrical_layer++;
		}

	}

	return -1;
}

gdouble epitaxy_get_device_start(struct epitaxy * in)
{
	int i = 0;
	gdouble pos = 0.0;
	for (i = 0; i < in->layers; i++) {

		if (in->electrical_layer[i] == TRUE) {
			return pos;
		}
		pos += in->width[i];

	}

	return -1;
}

gdouble epitaxy_get_device_start_i(struct epitaxy * in)
{
	int i = 0;
	for (i = 0; i < in->layers; i++) {

		if (in->electrical_layer[i] == TRUE) {
			return i;
		}

	}

	return -1;
}
