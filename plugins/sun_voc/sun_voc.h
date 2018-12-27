//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
//
//  Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
//
//	www.rodmack.com
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

/** @file sun_voc.h
@brief header fiel for the suns voc plugin
*/



#ifndef find_voc_h
#define find_voc_h
#include <sim.h>

struct sun_voc
{
	int sun_voc_single_point;
	gdouble sun_voc_Psun_start;
	gdouble sun_voc_Psun_stop;
	gdouble sun_voc_Psun_mul;
};

gdouble plugin_sim_voc(struct simulation *sim,struct device *in);
void sun_voc_load_config(struct simulation *sim,struct sun_voc* in,struct device *dev);
void solve_sun_voc(struct simulation *sim,struct device *in);
gdouble newton_sun_voc_ittr(struct simulation *sim,struct device *in);
#endif
