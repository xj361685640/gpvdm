//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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

/** @file light.c
	@brief Read an optical generation profile from a file.
*/


#include <util.h>
#include <dump_ctrl.h>
#include <complex_solver.h>
#include <const.h>
#include <light.h>
#include <device.h>
#include <light_interface.h>
#include <string.h>
#include <functions.h>
#include <log.h>

EXPORT void light_dll_ver(struct simulation *sim)
{
        printf_log(sim,"External field light model\n");
}

EXPORT int light_dll_solve_lam_slice(struct simulation *sim,struct light *in,int lam)
{

if (in->sun_E[lam]==0.0)
{
	return 0;
}

int i=0;
in->disable_cal_photon_density=TRUE;

if (lam==2)
{
	if (get_dump_status(sim,dump_optics)==TRUE)
	{
		char one[300];
		sprintf(one,"Reading in generation rate data from file %s\n",in->light_file_generation);
		waveprint(sim,one,532);
		
	}


		
		
	struct istruct data;

	inter_load(sim,&data,in->light_file_generation);

	inter_add_x(&data,in->light_file_generation_shift);

	for (i=0;i<in->points;i++)
	{
		in->Gn[i]=inter_get_hard(&data,in->x[i]);
		in->Gp[i]=in->Gn[i];
	}

	inter_free(&data);

}

return 0;
}


