//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012-2016 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
//
//	https://www.gpvdm.com
//	Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
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




#ifndef epitaxy_h
#define epitaxy_h
#include "advmath.h"
#include <sim_struct.h>

struct epitaxy
{
	int layers;
	int electrical_layers;
	gdouble width[20];
	char name[20][100];
	char mat_file[20][100];
	char dos_file[20][100];
	char pl_file[20][100];
	char electrical_layer[20];

	struct istruct *mat;
	struct istruct *mat_n;
};

void epitaxy_load(struct simulation *sim,struct epitaxy *in, char *file);
gdouble epitaxy_get_electrical_length(struct epitaxy *in);
gdouble epitaxy_get_optical_length(struct epitaxy *in);
int epitaxy_get_optical_material_layer(struct epitaxy *in,gdouble pos);
int epitaxy_get_electrical_material_layer(struct epitaxy *in,gdouble pos);
gdouble epitaxy_get_device_start(struct epitaxy *in);
gdouble epitaxy_get_device_stop(struct epitaxy *in);
gdouble epitaxy_get_device_start_i(struct epitaxy *in);
int epitaxy_get_epitaxy_layer_using_electrical_pos(struct epitaxy *in,gdouble pos);
void epitaxy_load_materials(struct simulation *sim,struct epitaxy *in);
void epitaxy_free(struct epitaxy *in);
void epitaxy_free_materials(struct epitaxy *in);
#endif
