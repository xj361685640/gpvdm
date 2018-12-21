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

/** @file newton_voc.c
	@brief Run the newton solver to calculate Voc.
*/

#include <dump.h>
#include <sim.h>
#include <exp.h>
#include <log.h>
#include <contacts.h>
#include <newton_voc.h>

void newton_aux_voc_simple(struct simulation *sim,struct device *in,gdouble V,gdouble* i,gdouble* didv,gdouble* didphi,gdouble* didxil,gdouble* didxipl,gdouble* didphir,gdouble* didxir,gdouble* didxipr)
{
gdouble i0=*i;
gdouble didv0=*didv;
gdouble didphi0=*didphi;
gdouble didxil0=*didxil;
gdouble didxipl0=*didxipl;
gdouble didphir0=*didphir;
gdouble didxir0=*didxir;
gdouble didxipr0=*didxipr;

gdouble S=1.0/(in->Rload+in->Rcontact);

*i=i0+V/in->Rshunt;
*didv= didv0+1.0/in->Rshunt;
*didphi= didphi0;
*didxil= didxil0;
*didxipl= didxipl0;
*didphir= didphir0;
*didxir= didxir0;
*didxipr= didxipr0;
return;
}

void newton_aux_voc(struct simulation *sim,struct device *in,gdouble V,gdouble* i,gdouble* didv,gdouble* didphi,gdouble* didxil,gdouble* didxipl,gdouble* didphir,gdouble* didxir,gdouble* didxipr)
{


/*double C=in->C;
gdouble i0=*i;
gdouble Vapplied_last=0.0;
Vapplied_last=contact_get_active_contact_voltage_last(sim,in);
gdouble didv0=*didv;
gdouble didphi0=*didphi;
gdouble didxil0=*didxil;
gdouble didxipl0=*didxipl;
gdouble didphir0=*didphir;
gdouble didxir0=*didxir;
gdouble didxipr0=*didxipr;
gdouble S=1.0/(pulse_config.pulse_Rload+in->Rcontact);
gdouble L=pulse_config.pulse_L;
*i=i0+C*(V-Vapplied_last)/in->dt+V/in->Rshunt-(V-L*(i0+C*(V-Vapplied_last)/in->dt+V/in->Rshunt-in->ilast)/in->dt)*S;
*didv=didv0+C*(1.0)/in->dt+1.0/in->Rshunt-(1.0-L*(didv0+C*(1.0)/in->dt+1.0/in->Rshunt)/in->dt)*S;//didv0+C*(1.0)/in->dt-S;
*didphi=didphi0-(-L*(didphi0)/in->dt)*S;//didphi0;
*didxil=didxil0-(-L*(didxil0)/in->dt)*S;//didxil0;
*didxipl=didxipl0-(-L*(didxipl0)/in->dt)*S;//didxipl0;
*didphir=didphir0-(-L*(didphir0)/in->dt)*S;//didphir0;
*didxir=didxir0-(-L*(didxir0)/in->dt)*S;//didxir0;
*didxipr=didxipr0-(-L*(didxipr0)/in->dt)*S;//didxipr0;*/

gdouble C=in->C;
gdouble i0=*i;
gdouble Vapplied_last=0.0;
Vapplied_last=contact_get_active_contact_voltage_last(sim,in);
gdouble didv0=*didv;
gdouble didphi0=*didphi;
gdouble didxil0=*didxil;
gdouble didxipl0=*didxipl;
gdouble didphir0=*didphir;
gdouble didxir0=*didxir;
gdouble didxipr0=*didxipr;
gdouble S=1.0/(in->Rload+in->Rcontact);
*i=i0+C*(V-Vapplied_last)/in->dt+V/in->Rshunt-V*S;
*didv=didv0+C*(1.0)/in->dt+1.0/in->Rshunt-S;
*didphi=didphi0;
*didxil=didxil0;
*didxipl=didxipl0;
*didphir=didphir0;
*didxir=didxir0;
*didxipr=didxipr0;

/*gdouble C=in->C;
gdouble i0=*i;
gdouble didv0=*didv;
gdouble didphi0=*didphi;
gdouble didxil0=*didxil;
gdouble didxipl0=*didxipl;
gdouble didphir0=*didphir;
gdouble didxir0=*didxir;
gdouble didxipr0=*didxipr;
*i=i0+C*(V-Vapplied_last)/in->dt+V/in->Rshunt;
*didv=didv0+C*(1.0)/in->dt+1.0/in->Rshunt;
*didphi=didphi0;
*didxil=didxil0;
*didxipl=didxipl0;
*didphir=didphir0;
*didxir=didxir0;
*didxipr=didxipr0;*/
/*
gdouble Rdrain=1.0/(1.0/in->Rshunt+1.0/(in->pulse_Rload+in->Rcontact));
*i=(i0+C*(V-in->Vapplied_last)/in->dt)-V/Rdrain;
*didv=(didv0+C*(1.0)/in->dt)-1.0/Rdrain;
*didphi=didphi0;
*didxil=didxil0;
*didxipl=didxipl0;
*didphir=didphir0;
*didxir=didxir0;
*didxipr=didxipr0;*/

return;
}


gdouble newton_sim_voc_fast(struct simulation *sim,struct device *in,int do_LC)
{
gdouble Vapplied=0.0;
gdouble Vapplied_last=0.0;
//long double photon_density=0.0;

//photon_density=three_d_avg(in, in->Gn);
//if (photon_density<1.0)
//{
//	return 0.0;
//}

Vapplied=contact_get_active_contact_voltage(sim,in);
Vapplied_last=contact_get_active_contact_voltage_last(sim,in);

newton_sim_voc(sim,in);

return get_I(in)+in->C*(Vapplied-Vapplied_last)+Vapplied/in->Rshunt;
}

gdouble newton_sim_voc(struct simulation *sim, struct device *in)
{
printf_log(sim,"Looking for Voc\n");
//long double photon_density=0.0;
//photon_density=three_d_avg(in, in->Gn);
//if (photon_density<1.0)
//{
//	return 0.0;
//}

gdouble C=in->C;
gdouble clamp=0.05;
gdouble step=0.05;
gdouble e0;
gdouble e1;
gdouble i0;
gdouble i1;
gdouble deriv;
gdouble Rdrain=in->Rload+in->Rcontact;
gdouble Vapplied=0.0;
gdouble Vapplied_last=0.0;

Vapplied=contact_get_active_contact_voltage(sim,in);
Vapplied_last=contact_get_active_contact_voltage_last(sim,in);

solve_all(sim,in);
i0=get_I(in);
e0=fabs(i0+Vapplied*(1.0/in->Rshunt-1.0/Rdrain));

Vapplied+=step;
contact_set_active_contact_voltage(sim,in,Vapplied);
solve_all(sim,in);
i1=get_I(in);
e1=fabs(i1+Vapplied*(1.0/in->Rshunt-1.0/Rdrain));

deriv=(e1-e0)/step;
step=-e1/deriv;

step=step/(1.0+fabs(step/clamp));
Vapplied+=step;
contact_set_active_contact_voltage(sim,in,Vapplied);

int count=0;
int max=200;
long double error_diff=0.0;
do
{
	e0=e1;
	solve_all(sim,in);
	i1=get_I(in);
	e1=fabs(i1+Vapplied*(1.0/in->Rshunt-1.0/Rdrain));
	deriv=(e1-e0)/step;
	step=-e1/deriv;
	error_diff=e1-e0;

	step=step/(1.0+fabs(step/clamp));
	Vapplied+=step;
	contact_set_active_contact_voltage(sim,in,Vapplied);

	if (get_dump_status(sim,dump_print_text)==TRUE)
	{
		printf_log(sim,"%d voc find Voc Vapplied=%Lf step=%Le error=%Le\n",count,Vapplied,step,e1);
	}
	if (count>max) break;
	count++;

	
	if (error_diff>0)
	{
		clamp/=1.1;
		printf_log(sim,"*");
	}
		
	}while(e1>1e-12);

gdouble ret=Vapplied-C*(i1-in->Ilast)/in->dt;
return ret;
}

void set_light_for_voc(struct simulation *sim,struct device *in,gdouble Voc)
{
}

