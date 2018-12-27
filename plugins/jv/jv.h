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

/** @file jv.h
@brief JV curve header file.
*/


#ifndef jv_h
#define jv_h
#include <sim.h>

struct jv
{
	gdouble Vstart;
	gdouble Vstop;
	gdouble Vstep;
	gdouble jv_step_mul;
	gdouble jv_light_efficiency;
	gdouble jv_max_j;
	long double jv_Rshunt;
	long double jv_Rcontact;
};

void sim_jv(struct simulation *sim,struct device *in);
void jv_load_config(struct simulation *sim,struct jv* in,struct device *dev, char* config_file_name);
#endif
