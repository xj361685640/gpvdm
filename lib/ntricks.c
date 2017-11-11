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



#include <stdio.h>
#include <exp.h>
#include "sim.h"
#include "dump.h"
#include "ntricks.h"
#include "gui_hooks.h"
#include <plot.h>
#include <cal_path.h>
#include <thermal.h>
#include <contacts.h>
#include <dump.h>
#include <log.h>

static int unused __attribute__((unused));


struct newton_math_state math_save_state;

void newton_push_state(struct device *in)
{
math_save_state.min_cur_error=in->min_cur_error;
math_save_state.max_electrical_itt=in->max_electrical_itt;
math_save_state.newton_min_itt=in->newton_min_itt;
math_save_state.electrical_clamp=in->electrical_clamp;
math_save_state.newton_clever_exit=in->newton_clever_exit;
}

void newton_pop_state(struct device *in)
{
in->min_cur_error=math_save_state.min_cur_error;
in->max_electrical_itt=math_save_state.max_electrical_itt;
in->newton_min_itt=math_save_state.newton_min_itt;
in->electrical_clamp=math_save_state.electrical_clamp;
in->newton_clever_exit=math_save_state.newton_clever_exit;
}

void ramp_externalv(struct simulation *sim,struct device *in,gdouble from,gdouble to)
{
gdouble V=from;
gdouble dV=0.1;
if ((to-from)<0.0) dV*= -1.0;
printf_log(sim,"dV=%Le\n",dV);
printf_log(sim,"Ramping: from=%Le to=%Le\n",from,to);

if (fabs(to-from)<=fabs(dV)) return;

do
{
	V+=dV;
	if (get_dump_status(sim,dump_print_text)==TRUE) printf_log(sim,"ramp: %Lf %Lf %d\n",V,to,in->kl_in_newton);
	sim_externalv(sim,in,V);

	plot_now(sim,in,"jv.plot");
	gui_send_data(sim,"pulse");

	if (fabs(V-to)<fabs(dV))
	{
		break;
	}

}while(1);

if (V!=to)
{
	V=to;
	sim_externalv(sim,in,V);
}

return;
}

void ramp(struct simulation *sim,struct device *in,gdouble from,gdouble to,gdouble steps)
{
gdouble Vapplied=0.0;
in->kl_in_newton=FALSE;
solver_realloc(sim,in);

Vapplied=from;
contact_set_active_contact_voltage(sim,in,Vapplied);

newton_push_state(in);
gdouble dV=0.20;
in->min_cur_error=1e-5;
in->max_electrical_itt=12;
in->newton_min_itt=3;
in->electrical_clamp=2.0;
in->newton_clever_exit=FALSE;
if ((to-from)<0.0) dV*= -1.0;
printf_log(sim,"dV=%Le\n",dV);
printf_log(sim,"Ramping: from=%Le to=%Le\n",from,to);

if (fabs(to-from)<=fabs(dV)) return;

do
{
	Vapplied+=dV;
	contact_set_active_contact_voltage(sim,in,Vapplied);
	//if (in->Vapplied<-4.0) dV= -0.3;

	if (get_dump_status(sim,dump_print_text)==TRUE)
	{
		printf_log(sim,"ramp: %Lf %Lf %d\n",Vapplied,to,in->kl_in_newton);
	}

	solve_all(sim,in);
	plot_now(sim,in,"jv_vars.plot");
	//sim_externalv(in,in->cevoltage);

	if (fabs(Vapplied-to)<fabs(dV))
	{
	//save_state(in,to);
		break;
	}

}while(1);

newton_pop_state(in);

if (Vapplied!=to)
{
	Vapplied=to;
	contact_set_active_contact_voltage(sim,in,Vapplied);
	solve_all(sim,in);
}


printf_log(sim,"Finished with ramp\n");
return;
}


void save_state(struct simulation *sim,struct device *in,gdouble to)
{
//<clean>
printf_log(sim,"Dumping state\n");
int i;
int band;
FILE *state;
state=fopena(get_output_path(sim),"state.dat","w");

fprintf(state,"%Le ",to);


for (i=0;i<in->ymeshpoints;i++)
{
	fprintf(state,"%Le %Le %Le ",in->phi[i],in->x[i],in->xp[i]);

	for (band=0;band<in->srh_bands;band++)
	{
		fprintf(state,"%Le %Le ",in->xt[i][band],in->xpt[i][band]);
	}

}
fclose(state);
//</clean>
}

int load_state(struct simulation *sim,struct device *in,gdouble voltage)
{
//<clean>
printf_log(sim,"Load state\n");
int i;
int band;
gdouble vtest;
FILE *state;
state=fopena(get_output_path(sim),"state.dat","r");
if (!state)
{
printf_log(sim,"State not found\n");
return FALSE;
}

unused=fscanf(state,"%Le",&vtest);
printf_log(sim,"%Le %Le",voltage,vtest);
if (vtest!=voltage)
{
printf_log(sim,"State not found\n");
return FALSE;
}
printf_log(sim,"Loading state\n");

contact_set_active_contact_voltage(sim,in,vtest);

for (i=0;i<in->ymeshpoints;i++)
{
	unused=fscanf(state,"%Le %Le %Le ",&(in->phi[i]),&(in->x[i]),&(in->xp[i]));

	for (band=0;band<in->srh_bands;band++)
	{
		unused=fscanf(state,"%Le %Le ",&(in->xt[i][band]),&(in->xpt[i][band]));
	}

}
fclose(state);
return TRUE;
//</clean>
}


gdouble sim_externalv_ittr(struct simulation *sim,struct device *in,gdouble wantedv)
{
gdouble Vapplied=0.0;
gdouble clamp=0.1;
gdouble step=0.01;
gdouble e0;
gdouble e1;
gdouble i0;
gdouble i1;
gdouble deriv;
gdouble Rs=in->Rcontact;
solve_all(sim,in);
i0=get_I(in);

Vapplied=contact_get_voltage(sim,in,0);
gdouble itot=i0+Vapplied/in->Rshunt;

e0=fabs(itot*Rs+Vapplied-wantedv);
Vapplied+=step;
contact_set_active_contact_voltage(sim,in,Vapplied);

solve_all(sim,in);

i1=get_I(in);
itot=i1+Vapplied/in->Rshunt;

e1=fabs(itot*Rs+Vapplied-wantedv);

deriv=(e1-e0)/step;
step= -e1/deriv;
//step=step/(1.0+fabs(step/clamp));
Vapplied+=step;
contact_set_active_contact_voltage(sim,in,Vapplied);
int count=0;
int max=1000;
do
{
e0=e1;
solve_all(sim,in);
itot=i1+Vapplied/in->Rshunt;
e1=fabs(itot*Rs+Vapplied-wantedv);

deriv=(e1-e0)/step;
step= -e1/deriv;
//gdouble clamp=0.01;
//if (e1<clamp) clamp=e1/100.0;
//step=step/(1.0+fabs(step/clamp));
step=step/(1.0+fabs(step/clamp));
Vapplied+=step;
contact_set_active_contact_voltage(sim,in,Vapplied);
if (count>max) break;
count++;
}while(e1>1e-8);


gdouble ret=get_I(in)+Vapplied/in->Rshunt;
//getchar();
return ret;
}

gdouble sim_externalv(struct simulation *sim,struct device *in,gdouble wantedv)
{
in->kl_in_newton=FALSE;
solver_realloc(sim,in);
sim_externalv_ittr(sim,in,wantedv);
return 0.0;
}



void solve_all(struct simulation *sim,struct device *in)
{
int z=0;
int x=0;
int ittr=0;
int cont=TRUE;

for (z=0;z<in->zmeshpoints;z++)
{
//	for (x=0;x<in->xmeshpoints;x++)
//	{

		if (in->newton_enable_external_thermal==FALSE)
		{
			solve_cur(sim,in,z,x);
		}else
		{
			do
			{
				solve_cur(sim,in,z,x);
		
				//plot_now(sim,"thermal.plot");
				//getchar();
				solve_thermal(sim,in);
				//plot_now(sim,"thermal.plot");
				//getchar();

				//plot_now(in);
				///getchar();
				if (((in->thermal_conv==TRUE)&&(in->dd_conv==TRUE))||(ittr>10)) cont=FALSE;
				//getchar();
				ittr++;
			}while(cont==TRUE);
		}
//	}
}

}

void newton_sim_simple(struct simulation  *sim,struct device *in)
{
in->kl_in_newton=FALSE;
solver_realloc(sim,in);

solve_all(sim,in);
}

