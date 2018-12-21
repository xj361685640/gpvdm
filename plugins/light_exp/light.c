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
	@brief Exponential light solver.
*/


#include <util.h>
#include <dump_ctrl.h>
#include <complex_solver.h>
#include <const.h>
#include <light.h>
#include <device.h>
#include <light_interface.h>

#include <functions.h>
#include <log.h>


EXPORT void light_dll_ver(struct simulation *sim)
{
        printf_log(sim,"Exponential light model\n");
}

EXPORT int light_dll_solve_lam_slice(struct simulation *sim,struct light *in,int lam)
{
if (in->sun_E[lam]==0.0)
{
	return 0;
}

if (get_dump_status(sim,dump_optics)==TRUE)
{
	char one[100];
	sprintf(one,"Solve light optical slice at %Lf nm\n",in->l[lam]*1e9);

	waveprint(sim,one,in->l[lam]*1e9);
}

int i;

gdouble complex n0=0.0+0.0*I;

//complex gdouble r=0.0+0.0*I;
complex gdouble t=0.0+0.0*I;
gdouble complex beta0=0.0+0.0*I;
complex gdouble Ep=in->sun_E[lam]+0.0*I;
complex gdouble En=0.0+0.0*I;
gdouble dx=in->x[1]-in->x[0];

for (i=0;i<in->points;i++)
{

	n0=in->nbar[lam][i];
	beta0=(2*PI*n0/in->l[lam]);
	Ep=Ep*cexp(-beta0*dx*I);

	t=in->t[lam][i];



	in->Ep[lam][i]=creal(Ep);
	in->Epz[lam][i]=cimag(Ep);
	in->En[lam][i]=creal(En);
	in->Enz[lam][i]=cimag(En);

	if (i!=(in->points-1))
	{
		if ((in->n[lam][i]!=in->n[lam][i+1])||(in->alpha[lam][i]!=in->alpha[lam][i+1]))
		{
			Ep=Ep*t;
		}
	}
}

return 0;
}


