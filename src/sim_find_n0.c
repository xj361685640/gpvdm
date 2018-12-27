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

/** @file sim_find_n0.c
@brief calculate 0V in the dark using the newton solver.
*/

#include <stdio.h>
#include <exp.h>
#include "util.h"
#include "sim.h"
#include "dos.h"
#include "dump.h"
#include "complex_solver.h"
#include "log.h"
#include <cal_path.h>
#include <contacts.h>
#include <lang.h>

void find_n0(struct simulation *sim,struct device *in)
{
int x;
int y;
int z;

printf_log(sim,"%s\n",_("Finding equilibrium conditions"));
gdouble oldsun=light_get_sun(&(in->mylight));

contacts_force_to_zero(sim,in);



light_set_sun(&(in->mylight),0);

light_solve_and_update(sim,in,&(in->mylight),0.0);



for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			in->B[z][x][y]=0.0;
		}
	}
}


gdouble save_clamp=in->electrical_clamp;
int save_ittr=in->max_electrical_itt;
gdouble save_electricalerror=in->min_cur_error;

in->electrical_clamp=in->electrical_clamp0;
in->max_electrical_itt=in->max_electrical_itt0;
in->min_cur_error=in->electrical_error0;

solve_all(sim,in);



in->max_electrical_itt=save_ittr;
in->electrical_clamp=save_clamp;
in->min_cur_error=save_electricalerror;

solve_all(sim,in);

for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			in->B[z][x][y]=get_dos_B(in,in->imat[z][x][y]);
		}
	}
}

reset_np_save(in);
reset_npequlib(in);

light_set_sun(&(in->mylight),oldsun);

contacts_update(sim,in);		//This should restore the value of the contacts

printf_log(sim,"%s\n",_("Finished finding equilibrium conditions"));
}
