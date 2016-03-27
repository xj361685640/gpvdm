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
#include "sim.h"
#include "inp.h"

static int unused __attribute__ ((unused));

static int enable_everything = FALSE;

void time_enable_everything(int in)
{
	enable_everything = in;
}

void time_mesh_save()
{
}

void time_load_mesh(struct device *in, int number)
{
}

void time_init(struct device *in)
{
}

void device_timestep(struct device *in)
{
}

int time_run()
{
	return 0;
}

gdouble time_get_voltage()
{
	return 0.0;
}

gdouble time_get_fs_laser()
{
	return 0.0;
}

gdouble time_get_sun()
{
	return 0.0;
}

gdouble time_get_laser()
{
	return 0.0;
}

void time_memory_free()
{
}
