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


#include <dump.h>
#include <sim.h>
#include <exp.h>
#include <log.h>
#include <contacts.h>
#include <newton_voc.h>

void newton_aux_voc(struct simulation *sim,struct device *in,gdouble V,gdouble* i,gdouble* didv,gdouble* didphi,gdouble* didxil,gdouble* didxipl,gdouble* didphir,gdouble* didxir,gdouble* didxipr)
{
return;
}


gdouble newton_sim_voc_fast(struct simulation *sim,struct device *in,int do_LC)
{
gdouble Vapplied=0.0;
gdouble Vapplied_last=0.0;

Vapplied=contact_get_voltage(sim,in,0);
Vapplied_last=contact_get_voltage_last(sim,in,0);

newton_sim_voc(sim,in);

return get_I(in)+in->C*(Vapplied-Vapplied_last)+Vapplied/in->Rshunt;
}

gdouble newton_sim_voc(struct simulation *sim, struct device *in)
{
printf_log(sim,"Looking for Voc\n");
gdouble C=in->C;
gdouble clamp=0.1;
gdouble step=0.01;
gdouble e0;
gdouble e1;
gdouble i0;
gdouble i1;
gdouble deriv;
gdouble Rdrain=in->Rload+in->Rcontact;
gdouble Vapplied=0.0;
gdouble Vapplied_last=0.0;

Vapplied=contact_get_voltage(sim,in,0);
Vapplied_last=contact_get_voltage_last(sim,in,0);

solve_all(sim,in);
i0=get_I(in);
e0=fabs(i0+Vapplied*(1.0/in->Rshunt-1.0/Rdrain));

Vapplied+=step;
contact_set_voltage(sim,in,0,Vapplied);
solve_all(sim,in);
i1=get_I(in);
e1=fabs(i1+Vapplied*(1.0/in->Rshunt-1.0/Rdrain));

deriv=(e1-e0)/step;
step=-e1/deriv;

step=step/(1.0+fabs(step/clamp));
Vapplied+=step;
contact_set_voltage(sim,in,0,Vapplied);

int count=0;
int max=200;
do
{
	e0=e1;
	solve_all(sim,in);
	i1=get_I(in);
	e1=fabs(i1+Vapplied*(1.0/in->Rshunt-1.0/Rdrain));
	deriv=(e1-e0)/step;
	step=-e1/deriv;

	step=step/(1.0+fabs(step/clamp));
	Vapplied+=step;
	contact_set_voltage(sim,in,0,Vapplied);

	if (get_dump_status(sim,dump_print_text)==TRUE)
	{
		printf_log(sim,"%d voc find Voc Vapplied=%Lf step=%Le error=%Le\n",count,Vapplied,step,e1);
	}
	if (count>max) break;
	count++;

}while(e1>1e-12);

gdouble ret=Vapplied-C*(i1-in->Ilast)/in->dt;
return ret;
}

void set_light_for_voc(struct simulation *sim,struct device *in,gdouble Voc)
{
}

