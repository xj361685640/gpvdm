//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012-2016 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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

/** @file equilibrium.c
	@brief Plugin to calculate equilibrium conditions.
*/

#include <sim.h>
#include <exp.h>
#include "equilibrium.h"
#include <dump.h>
#include "ntricks.h"
#include <inp.h>
#include <buffer.h>
#include <gui_hooks.h>
#include <pl.h>
#include <log.h>
#include <lang.h>
#include <remesh.h>
#include <plot.h>
#include <cal_path.h>
#include <contacts.h>
#include <contacts_vti_store.h>

static int unused __attribute__((unused));

void sim_equilibrium(struct simulation *sim,struct device *in)
{
char temp[PATHLEN];
//if (get_dump_status(sim,dump_equilibrium)==TRUE)
//{
join_path(2,temp,get_output_path(sim),"equilibrium");
dump_1d_slice(sim,in,temp);

printf_log(sim,"Ef=%Le\n",in->Fi[0][0][0]);	
printf_log(sim,"%s = %Le\n", _("Holes on left contact"),in->l_holes);
printf_log(sim,"%s = %Le\n",_("Electrons on left contact"), in->l_electrons);

printf_log(sim,"%s = %Le\n",_("Holes on right contact"), in->r_holes);
printf_log(sim,"%s = %Le\n",_("Electrons on right contact"), in->r_electrons);

FILE *out=fopena(get_output_path(sim),"sim_info.dat","w");
fprintf (out,"#left_holes\n");
fprintf (out,"%Le\n", in->l_holes);
fprintf (out,"#left_electrons\n");
fprintf (out,"%Le\n", in->l_electrons);
fprintf (out,"#right_holes\n");
fprintf (out,"%Le\n", in->r_holes);
fprintf (out,"#right_electrons\n");
fprintf (out,"%Le\n", in->r_electrons);
fprintf (out,"#Vbi\n");
fprintf (out,"%Le\n", in->vbi);
fprintf (out,"#electron_affinity_left\n");
fprintf (out,"%Le\n", in->electron_affinity_left);
fprintf (out,"#electron_affinity_right\n");
fprintf (out,"%Le\n", in->electron_affinity_right);
fprintf (out,"#end\n");
fclose(out);
//}
/*int lam=0;

printf_log(sim,_("Running equilibrium simulation\n"));


struct contacts_vti_store contact_store;
dump_contacts_init(sim,in,&contact_store);


struct equilibrium config;
equilibrium_load_config(sim,&config,in);
gdouble V=0.0;
int ittr=0;
gdouble J;
gdouble Pden;
int first=TRUE;
gdouble Vlast;
gdouble Jlast;
gdouble Pdenlast;
gdouble Vexternal;

lam=light_get_pos_from_wavelength(sim,&(in->mylight),in->led_wavelength);//light_find_wavelength(&(in->mylight),in->led_wavelength);

gdouble Vapplied=0.0;
contact_set_active_contact_voltage(sim,in,Vapplied);


remesh_reset(in,Vapplied);


gdouble sun_orig=light_get_sun(&(in->mylight));
light_set_sun(&(in->mylight),sun_orig);
light_solve_and_update(sim,in,&(in->mylight),0.0);

newton_push_state(in);

newton_set_min_ittr(in,30);

contact_set_active_contact_voltage(sim,in,Vapplied);
V=Vapplied;
newton_sim_jv(sim,in);

newton_pop_state(in);


in->FF=gfabs(in->Pmax/(in->Jsc*in->Voc));

if (get_dump_status(sim,dump_print_text)==TRUE)
{
	//printf_log(sim,"Voc= %Lf (V)\n",in->Voc);
	//printf_log(sim,"Jsc= %Lf (A/m^2)\n",in->Jsc);
	//printf_log(sim,"Pmax= %Lf (W/m^2)\n",in->Pmax);
	//printf_log(sim,"Pmax %s= %Lf (V)\n",_("Voltage"),in->Pmax_voltage);
	//printf_log(sim,"FF= %Lf\n",in->FF*100.0);
	//printf_log(sim,"%s= %Lf percent\n",_("Efficiency"),gfabs(in->Pmax/light_get_sun(&(in->mylight))/1000)*100.0);
}

	FILE *out;
	out=fopena(get_input_path(sim),"sim_info.dat","w");
	//fprintf(out,"#ff\n%Lf\n",in->FF);
	fprintf(out,"#end");
	fclose(out);


light_set_sun(&(in->mylight),sun_orig);
*/
}




void equilibrium_load_config(struct simulation *sim,struct equilibrium* in,struct device *dev)
{
/*struct inp_file inp;
inp_init(sim,&inp);
inp_load_from_path(sim,&inp,get_input_path(sim),"jv.inp");
inp_check(sim,&inp,1.21);
inp_search_gdouble(sim,&inp,&(in->Vstart),"#Vstart");
inp_search_gdouble(sim,&inp,&(in->Vstop),"#Vstop");
inp_search_gdouble(sim,&inp,&(in->Vstep),"#Vstep");
inp_search_gdouble(sim,&inp,&(in->jv_step_mul),"#jv_step_mul");
inp_search_gdouble(sim,&inp,&(in->jv_light_efficiency),"#jv_light_efficiency");
inp_search_gdouble(sim,&inp,&(in->jv_max_j),"#jv_max_j");
in->jv_light_efficiency=gfabs(in->jv_light_efficiency);
inp_free(sim,&inp);
*/
}
