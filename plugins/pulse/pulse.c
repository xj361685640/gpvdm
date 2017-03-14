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


#include <sim.h>
#include <dump.h>
#include <ntricks.h>
#include <dynamic_store.h>
#include <i.h>
#include <exp.h>
#include "pulse.h"
#include <inp.h>
#include <buffer.h>
#include <gui_hooks.h>
#include <lang.h>
#include <log.h>
#include <plot.h>
#include <cal_path.h>
#include <probe.h>
#include <newton_voc.h>
#include <newton_externalv.h>
#include <contacts.h>


static int unused __attribute__((unused));

struct pulse pulse_config;

void sim_pulse(struct simulation *sim,struct device *in)
{
char tunits[100];
struct buffer buf;
buffer_init(&buf);

struct dynamic_store store;
dump_dynamic_init(sim,&store,in);

struct istruct out_i;
inter_init(sim,&out_i);

struct istruct out_v;
inter_init(sim,&out_v);

struct istruct out_G;
inter_init(sim,&out_G);


char config_file_name[200];

if (find_config_file(sim,config_file_name,get_input_path(sim),in->simmode,"pulse")!=0)
{
	ewe(sim,"%s %s %s\n",_("no pulse config file found"),get_input_path(sim),in->simmode);
}

printf_log(sim,"%s\n",config_file_name);

pulse_load_config(sim,&pulse_config,in,config_file_name);
int number=strextract_int(config_file_name);

in->go_time=FALSE;

time_init(sim,in);
//time_load_mesh(in,number);

time_load_mesh(sim,in,number);
//time_mesh_save();
//getchar();
//struct istruct pulseout;
//inter_init(&pulseout);

int ittr=0;

int step=0;
light_set_sun(&(in->mylight),time_get_sun(in)*pulse_config.pulse_light_efficiency);
light_solve_and_update(sim,in,&(in->mylight), time_get_laser(in)*pulse_config.pulse_light_efficiency);

gdouble V=0.0;
if (get_dump_status(sim,dump_optical_probe)==TRUE)
{
	probe_init(sim,in);
}

if (pulse_config.pulse_sim_mode==pulse_load)
{
	sim_externalv(sim,in,time_get_voltage(in)+pulse_config.pulse_bias);
	newton_externv(sim,in,time_get_voltage(in)+pulse_config.pulse_bias,FALSE);
}else
if (pulse_config.pulse_sim_mode==pulse_ideal_diode_ideal_load)
{
	ramp_externalv(sim,in,V,time_get_voltage(in)+pulse_config.pulse_bias);
	newton_externalv_simple(sim,in,time_get_voltage(in)+pulse_config.pulse_bias);
}
else
if (pulse_config.pulse_sim_mode==pulse_open_circuit)
{
	contact_set_active_contact_voltage(sim,in,in->Vbi);
	in->Rload=1e6;
	newton_sim_voc(sim,in);
	newton_sim_voc_fast(sim,in,FALSE);
}else
{
	ewe(sim,_("pulse mode not known\n"));
}

//device_timestep(sim,in);
time_store(sim,in);

in->go_time=TRUE;

gdouble i0=0;
carrier_count_reset(in);
reset_np_save(in);
gdouble Vapplied=0.0;
Vapplied=contact_get_active_contact_voltage(sim,in);//contact_get_voltage(sim,in,0);
printf_log(sim,"Vapplied=%Le\n",Vapplied);
do
{

if (get_dump_status(sim,dump_optical_probe)==TRUE)
{
probe_record_step(sim,in);
}

	light_set_sun(&(in->mylight),time_get_sun(in)*pulse_config.pulse_light_efficiency);
	light_solve_and_update(sim,in,&(in->mylight), (time_get_laser(in)+time_get_fs_laser(in))*pulse_config.pulse_light_efficiency);
	//int i;
	//FILE *t=fopen("t.dat","w");
	//for (i=0;i<in->ymeshpoints;i++)
	//{
	//	fprintf(t,"%Le %Le\n",in->ymesh[i],in->Gn[i]);
	//	printf_log(sim,"%Le %Le\n",in->ymesh[i],in->Gn[i]);
	//}
	//fclose(t);
	dump_dynamic_add_data(sim,&store,in,in->time);
	dump_write_to_disk(sim,in);

	if (pulse_config.pulse_sim_mode==pulse_load)
	{
		V=time_get_voltage(in)+pulse_config.pulse_bias;
		i0=newton_externv(sim,in,V,TRUE);
	}else
	if (pulse_config.pulse_sim_mode==pulse_ideal_diode_ideal_load)
	{
		V=time_get_voltage(in)+pulse_config.pulse_bias;
		i0=newton_externalv_simple(sim,in,V);
	}else
	if (pulse_config.pulse_sim_mode==pulse_open_circuit)
	{
		V=contact_get_active_contact_voltage(sim,in);
		newton_sim_voc_fast(sim,in,TRUE);
	}else
	{
		ewe(sim,_("pulse mode not known\n"));
	}


	if (get_dump_status(sim,dump_print_text)==TRUE)
	{
		time_with_units(tunits,in->time);
		printf_log(sim,"%s=%Le (%s) %s=%d %.1Le ",_("pulse time"),in->time,tunits,_("step"),step,in->last_error);
		printf_log(sim,"Vtot=%Lf %s = %Le mA (%Le A/m^2)\n",V,_("current"),get_I(in)/1e-3,get_J(in));
	}

	ittr++;
	gui_send_data(sim,"pulse");

	plot_now(sim,in,"pulse.plot");
	inter_append(&out_i,in->time,i0);
	inter_append(&out_v,in->time,V);
	inter_append(&out_G,in->time,in->Gn[0][0][0]);
	//printf("%Le %d %Le\n",in->time,time_test_last_point(in),in->dt);
	if (time_test_last_point(in)==TRUE) break;
	device_timestep(sim,in);
	step++;
	//getchar();

}while(1);

struct istruct out_flip;
struct istruct out_j;


dump_dynamic_save(sim,get_output_path(sim),&store);
dump_dynamic_free(sim,in,&store);


buffer_malloc(&buf);
buf.y_mul=1e3;
buf.x_mul=1e6;
sprintf(buf.title,"%s - %s",_("Time"),_("Current"));
strcpy(buf.type,"xy");
strcpy(buf.x_label,_("Time"));
strcpy(buf.data_label,_("Current"));
strcpy(buf.x_units,"\\ms");
strcpy(buf.data_units,"m");
buf.logscale_x=0;
buf.logscale_y=0;
buf.x=1;
buf.y=out_i.len;
buf.z=1;
buffer_add_info(&buf);
buffer_add_xy_data(&buf,out_i.x, out_i.data, out_i.len);
buffer_dump_path(sim,get_output_path(sim),"pulse_i.dat",&buf);
buffer_free(&buf);


inter_copy(&out_j,&out_i,TRUE);
inter_mul(&out_j,1.0/(in->xlen*in->zlen));

buffer_malloc(&buf);
buf.y_mul=1e3;
buf.x_mul=1e6;
sprintf(buf.title,"%s - %s",_("Time"),_("current density"));
strcpy(buf.type,"xy");
strcpy(buf.x_label,_("Time"));
strcpy(buf.data_label,_("Current density"));
strcpy(buf.x_units,"\\ms");
strcpy(buf.data_units,"m");
buf.logscale_x=0;
buf.logscale_y=0;
buf.x=1;
buf.y=out_j.len;
buf.z=1;
buffer_add_info(&buf);
buffer_add_xy_data(&buf,out_j.x, out_j.data, out_j.len);
buffer_dump_path(sim,get_output_path(sim),"pulse_j.dat",&buf);
buffer_free(&buf);

inter_free(&out_j);

inter_copy(&out_flip,&out_i,TRUE);
inter_mul(&out_flip,-1.0);

buffer_malloc(&buf);
buf.y_mul=1e3;
buf.x_mul=1e6;
sprintf(buf.title,"%s - %s",_("Time"),_("current"));
strcpy(buf.type,"xy");
strcpy(buf.x_label,_("Time"));
strcpy(buf.data_label,_("Current"));
strcpy(buf.x_units,"\\ms");
strcpy(buf.data_units,"mA");
buf.logscale_x=0;
buf.logscale_y=0;
buf.x=1;
buf.y=out_flip.len;
buf.z=1;
buffer_add_info(&buf);
buffer_add_xy_data(&buf,out_flip.x, out_flip.data, out_flip.len);
buffer_dump_path(sim,get_output_path(sim),"pulse_i_pos.dat",&buf);
buffer_free(&buf);

inter_free(&out_flip);



buffer_malloc(&buf);
buf.y_mul=1.0;
buf.x_mul=1e6;
sprintf(buf.title,"%s - %s",_("Time"),_("Voltage"));
strcpy(buf.type,"xy");
strcpy(buf.x_label,_("Time"));
strcpy(buf.data_label,_("Volts"));
strcpy(buf.x_units,"\\ms");
strcpy(buf.data_units,_("Voltage"));
buf.logscale_x=0;
buf.logscale_y=0;
buf.x=1;
buf.y=out_v.len;
buf.z=1;
buffer_add_info(&buf);
buffer_add_xy_data(&buf,out_v.x, out_v.data, out_v.len);
buffer_dump_path(sim,get_output_path(sim),"pulse_v.dat",&buf);
buffer_free(&buf);


buffer_malloc(&buf);
buf.y_mul=1.0;
buf.x_mul=1e6;
sprintf(buf.title,"%s - %s",_("Time"),_("Photogeneration rate"));
strcpy(buf.type,"xy");
strcpy(buf.x_label,_("Time"));
strcpy(buf.data_label,_("Generation rate"));
strcpy(buf.x_units,"s");
strcpy(buf.data_units,"m^{-3} s^{-1}");
buf.logscale_x=0;
buf.logscale_y=0;
buf.x=1;
buf.y=out_G.len;
buf.z=1;
buffer_add_info(&buf);
buffer_add_xy_data(&buf,out_G.x, out_G.data, out_G.len);
buffer_dump_path(sim,get_output_path(sim),"pulse_G.dat",&buf);
buffer_free(&buf);


if (get_dump_status(sim,dump_optical_probe)==TRUE)
{
	probe_dump(sim,in);
}

in->go_time=FALSE;

if (get_dump_status(sim,dump_optical_probe)==TRUE)
{
	probe_free(sim,in);
}
inter_free(&out_G);
inter_free(&out_i);
inter_free(&out_v);

time_memory_free(in);



}

void pulse_load_config(struct simulation *sim,struct pulse *in,struct device *dev,char *config_file_name)
{

char name[200];
char laser_name[200];
struct inp_file inp;
inp_init(sim,&inp);
inp_load_from_path(sim,&inp,get_input_path(sim),config_file_name);
inp_check(sim,&inp,1.29);

inp_search_gdouble(sim,&inp,&(in->pulse_shift),"#pulse_shift");
inp_search_string(sim,&inp,name,"#pulse_sim_mode");
inp_search_gdouble(sim,&inp,&(dev->L),"#pulse_L");
inp_search_gdouble(sim,&inp,&(dev->Rload),"#Rload");
inp_search_gdouble(sim,&inp,&(in->pulse_bias),"#pulse_bias");
inp_search_string(sim,&inp,laser_name,"#pump_laser");

inp_search_gdouble(sim,&inp,&(in->pulse_light_efficiency),"#pulse_light_efficiency");
in->pulse_light_efficiency=gfabs(in->pulse_light_efficiency);

light_load_laser(sim,(&dev->mylight),laser_name);
in->pulse_sim_mode=english_to_bin(sim,name);

inp_free(sim,&inp);

}
