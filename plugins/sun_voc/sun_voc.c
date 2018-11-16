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


#include <sim.h>
#include <dump.h>
#include <ntricks.h>
#include <gui_hooks.h>
#include <inp.h>
#include <exp.h>
#include <plot.h>
#include "sun_voc.h"
#include <buffer.h>
#include <dynamic_store.h>
#include <cal_path.h>
#include <contacts.h>
#include <log.h>
#include <newton_voc.h>

void solve_sun_voc(struct simulation *sim,struct device *in)
{
int clever_contacts=FALSE;//in->config_kl_in_newton;

if (clever_contacts==FALSE)
{
	in->kl_in_newton=TRUE;
	in->newton_aux=&newton_aux_voc_simple;
	solver_realloc(sim,in);
	solve_all(sim,in);
}else
{
	in->kl_in_newton=FALSE;
	solver_realloc(sim,in);
	newton_sun_voc_ittr(sim,in);
}


}

gdouble newton_sun_voc_ittr(struct simulation *sim,struct device *in)
{
gdouble clamp=0.05;
gdouble step=0.05;
gdouble e0;
gdouble e1;
gdouble i0;
gdouble i1;
gdouble deriv;
gdouble Vapplied=0.0;

Vapplied=contact_get_active_contact_voltage(sim,in);

solve_all(sim,in);
i0=get_I(in);
e0=gfabs(Vapplied/in->Rshunt+Vapplied/(1e6+in->Rcontact)+i0);

Vapplied+=step;
contact_set_active_contact_voltage(sim,in,Vapplied);

solve_all(sim,in);
i1=get_I(in);
e1=gfabs(Vapplied/in->Rshunt+Vapplied/(1e6+in->Rcontact)+i1);

deriv=(e1-e0)/step;


step=-e1/deriv;
step=step/(1.0+gfabs(step/clamp));
Vapplied+=step;
contact_set_active_contact_voltage(sim,in,Vapplied);

int count=0;
int max=100;
do
{
	e0=e1;
	solve_all(sim,in);
	i1=get_I(in);

	e1=gfabs(Vapplied/in->Rshunt+Vapplied/(1e6+in->Rcontact)+i1);
	printf_log(sim,"error=%Le Vapplied=%Le \n",e1,Vapplied);
	deriv=(e1-e0)/step;
	step=-e1/deriv;
	//gdouble clamp=0.01;
	//if (e1<clamp) clamp=e1/100.0;
	//step=step/(1.0+gfabs(step/clamp));
	step=step/(1.0+gfabs(step/clamp));
	Vapplied+=step;
	contact_set_active_contact_voltage(sim,in,Vapplied);
	if (count>max)
	{
		break;
	}
	count++;
}while(e1>1e-11);

return 0.0;
}




gdouble plugin_sim_voc(struct simulation *sim,struct device *in)
{

struct buffer buf;
buffer_init(&buf);
char name[200];

struct sun_voc config;
sun_voc_load_config(sim,&config,in);

struct dynamic_store store;
dump_dynamic_init(sim,&store,in);

if (config.sun_voc_single_point)
{
	light_solve_and_update(sim,in,&(in->mylight),0.0);


	solve_sun_voc(sim,in);
	printf_log(sim,"Psun= %Le Voc=%Le\n",light_get_sun(&(in->mylight)),get_equiv_V(sim,in));


	gdouble n_voc=get_total_np(in);
	gdouble r_voc=get_avg_Rn(in);//get_avg_recom(in);
	gdouble J=get_avg_J(in);
	gdouble cur=get_I(in);
	FILE *out;
	out=fopena(get_output_path(sim),"sim_info.dat","w");
	fprintf(out,"#voc\n%Le\n",get_equiv_V(sim,in));
	fprintf(out,"#voc_nt\n%Le\n",get_n_trapped_charge(in));
	fprintf(out,"#voc_pt\n%Le\n",get_p_trapped_charge(in));
	fprintf(out,"#voc_nf\n%Le\n",get_free_n_charge(in));
	fprintf(out,"#voc_pf\n%Le\n",get_free_p_charge(in));
	fprintf(out,"#voc_np_tot\n%Le\n",n_voc);
	fprintf(out,"#voc_tau\n%Le\n",n_voc/r_voc);
	fprintf(out,"#voc_R\n%Le\n",r_voc);
	fprintf(out,"#voc_Jr\n%Le\n",r_voc*in->ylen);
	fprintf(out,"#voc_J\n%Le\n",J);
	fprintf(out,"#voc_J_to_Jr\n%Le\n",gfabs(J)/(Q*r_voc*in->ylen));
	fprintf(out,"#voc_i\n%Le\n",gfabs(cur));
	fprintf(out,"#end");

	fclose(out);

	dump_dynamic_add_data(sim,&store,in,in->time);
	dump_write_to_disk(sim,in);
}else
{

	struct istruct sun_voc_out;
	inter_init(sim,&sun_voc_out);
	set_dump_status(sim,dump_optics,FALSE);
	gdouble sun=config.sun_voc_Psun_start;
	do
	{
		light_set_sun(&(in->mylight),sun);
		light_solve_and_update(sim,in,&(in->mylight),0.0);

		solve_sun_voc(sim,in);
		printf_log(sim,"Psun= %Le Voc=%Le\n",light_get_sun(&(in->mylight)),get_equiv_V(sim,in));
		inter_append(&sun_voc_out,light_get_sun(&(in->mylight)),get_equiv_V(sim,in));
		if (sun>config.sun_voc_Psun_stop)
		{
			break;
		}
		dump_dynamic_add_data(sim,&store,in,in->time);
		sun*=config.sun_voc_Psun_mul;

		dump_write_to_disk(sim,in);
		plot_now(sim,in,"sun_voc.plot");
		stop_start(sim,in);
	}while(1);

	buffer_malloc(&buf);
	sprintf(name,"%s","suns_voc.dat");
	buf.y_mul=1.0;
	buf.x_mul=1.0;
	strcpy(buf.title,"Sun - Voc");
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,"Suns");
	strcpy(buf.data_label,"V_{oc}");
	strcpy(buf.x_units,"Volts");
	strcpy(buf.data_units,"(Suns)");
	buf.logscale_x=1;
	buf.logscale_y=1;
	buffer_add_info(sim,&buf);
	buffer_add_xy_data(sim,&buf,sun_voc_out.x, sun_voc_out.data, sun_voc_out.len);
	buffer_dump_path(sim,get_output_path(sim),name,&buf);
	buffer_free(&buf);

	inter_free(&sun_voc_out);
	dump_dynamic_save(sim,get_output_path(sim),&store);
	dump_dynamic_free(sim,in,&store);

}

/*FILE *test=fopen("test.dat","w");
int band;
for (band=0;band<in->srh_bands;band++)
{
	fprintf(test,"%d %Le\n",band,in->nt[10][band]);

}
fclose(test);
gdouble E=-1.0;
FILE *test2=fopen("test2.dat","w");
do
{
	fprintf(test2,"%Le ",E);
	for (band=0;band<in->srh_bands;band++)
	{
		fprintf(test2,"%Le ",get_n_pop_srh(E,in->Te[0],band,0));
	}

fprintf(test2,"\n");
E+=1e-3;
}while(E<0.0);

fclose(test2);*/
return 0.0;
}

void sun_voc_load_config(struct simulation *sim,struct sun_voc* in,struct device *dev)
{
struct inp_file inp;
inp_init(sim,&inp);
inp_load_from_path(sim,&inp,get_input_path(sim),"sun_voc.inp");
inp_check(sim,&inp,1.0);
in->sun_voc_single_point=inp_search_english(sim,&inp,"#sun_voc_single_point");
inp_search_gdouble(sim,&inp,&(in->sun_voc_Psun_start),"#sun_voc_Psun_start");
inp_search_gdouble(sim,&inp,&(in->sun_voc_Psun_stop),"#sun_voc_Psun_stop");
inp_search_gdouble(sim,&inp,&(in->sun_voc_Psun_mul),"#sun_voc_Psun_mul");
inp_free(sim,&inp);

}
