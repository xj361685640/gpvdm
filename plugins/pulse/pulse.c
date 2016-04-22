//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie
//
//	roderick.mackenzie@nottingham.ac.uk
//	www.roderickmackenzie.eu
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

static int unused __attribute__((unused));

struct pulse pulse_config;

void sim_pulse(struct simulation *sim,struct device *in)
{
struct buffer buf;
buffer_init(&buf);

struct dynamic_store store;
dump_dynamic_init(sim,&store,in);

struct istruct out_i;
inter_init(&out_i);

struct istruct out_v;
inter_init(&out_v);

struct istruct out_G;
inter_init(&out_G);

struct istruct lost_charge;
inter_init(&lost_charge);

char config_file_name[200];

if (find_config_file(sim,config_file_name,get_input_path(sim),in->simmode,"pulse")!=0)
{
	ewe(sim,"%s %s %s\n",_("no pulse config file found"),get_input_path(sim),in->simmode);
}

printf("%s\n",config_file_name);

pulse_load_config(sim,&pulse_config,in,config_file_name);
int number=strextract_int(config_file_name);
ntricks_externv_set_load(pulse_config.pulse_Rload);

in->go_time=FALSE;

in->time=0.0;

time_init(in);
//time_load_mesh(in,number);

time_load_mesh(sim,in,number);
//time_mesh_save();
//getchar();
//struct istruct pulseout;
//inter_init(&pulseout);

int ittr=0;

int step=0;
light_set_sun(&(in->mylight),time_get_sun());
light_solve_and_update(sim,in,&(in->mylight), time_get_laser());

gdouble V=0.0;

if (pulse_config.pulse_sim_mode==pulse_load)
{
	sim_externalv(sim,in,time_get_voltage());
	ntricks_externv_newton(sim,in,time_get_voltage(),FALSE);
}else
if (pulse_config.pulse_sim_mode==pulse_open_circuit)
{
	in->Vapplied=in->Vbi;
	pulse_config.pulse_Rload=1e6;
	pulse_newton_sim_voc(sim,in);
	pulse_newton_sim_voc_fast(sim,in,FALSE);
}else
{
	ewe(sim,_("pulse mode not known\n"));
}

device_timestep(sim,in);

in->go_time=TRUE;

gdouble extracted_through_contacts=0.0;
gdouble i0=0;
carrier_count_reset(in);
reset_np_save(in);
do
{
	light_set_sun(&(in->mylight),time_get_sun());
	light_solve_and_update(sim,in,&(in->mylight), time_get_laser()+time_get_fs_laser());
	dump_dynamic_add_data(sim,&store,in,in->time);

	if (pulse_config.pulse_sim_mode==pulse_load)
	{
		V=time_get_voltage();
		i0=ntricks_externv_newton(sim,in,V,TRUE);
	}else
	if (pulse_config.pulse_sim_mode==pulse_open_circuit)
	{
		V=in->Vapplied;
		pulse_newton_sim_voc_fast(sim,in,TRUE);
	}else
	{
		ewe(sim,_("pulse mode not known\n"));
	}


	if (get_dump_status(sim,dump_print_text)==TRUE)
	{
		printf_log(sim,"%s=%Le %s=%d %.1Le ",_("pulse time"),in->time,_("step"),step,in->last_error);
		printf_log(sim,"Vtot=%Lf %s = %Le mA (%Le A/m^2)\n",V,_("current"),get_I(in)/1e-3,get_J(in));


	}

	ittr++;

	gui_send_data("pulse");
	dump_write_to_disk(sim,in);

	plot_now(sim,"pulse.plot");

	inter_append(&out_i,in->time,i0);
	inter_append(&out_v,in->time,V);
	inter_append(&out_G,in->time,in->Gn[0]);
	inter_append(&lost_charge,in->time,extracted_through_contacts-fabs(get_extracted_n(in)+get_extracted_p(in))/2.0);

	device_timestep(sim,in);
	step++;
	
	if (time_run()==FALSE) break;
	//getchar();

}while(1);

struct istruct out_flip;

dump_dynamic_save(sim,get_output_path(sim),&store);
dump_dynamic_free(sim,&store);

buffer_malloc(&buf);
buf.y_mul=1e3;
buf.x_mul=1e6;
strcpy(buf.title,_("Time - current"));
strcpy(buf.type,_("xy"));
strcpy(buf.x_label,_("Time"));
strcpy(buf.y_label,_("Current"));
strcpy(buf.x_units,_("us"));
strcpy(buf.y_units,_("m"));
buf.logscale_x=0;
buf.logscale_y=0;
buffer_add_info(&buf);
buffer_add_xy_data(&buf,out_i.x, out_i.data, out_i.len);
buffer_dump_path(get_output_path(sim),"pulse_i.dat",&buf);
buffer_free(&buf);

inter_copy(&out_flip,&out_i,TRUE);
inter_mul(&out_flip,-1.0);

buffer_malloc(&buf);
buf.y_mul=1e3;
buf.x_mul=1e6;
strcpy(buf.title,_("Time - -current"));
strcpy(buf.type,_("xy"));
strcpy(buf.x_label,_("Time"));
strcpy(buf.y_label,_("-Current"));
strcpy(buf.x_units,_("us"));
strcpy(buf.y_units,_("mA"));
buf.logscale_x=0;
buf.logscale_y=0;
buffer_add_info(&buf);
buffer_add_xy_data(&buf,out_flip.x, out_flip.data, out_flip.len);
buffer_dump_path(get_output_path(sim),"pulse_i_pos.dat",&buf);
buffer_free(&buf);

inter_free(&out_flip);



buffer_malloc(&buf);
buf.y_mul=1.0;
buf.x_mul=1e6;
strcpy(buf.title,_("Time - Voltage"));
strcpy(buf.type,_("xy"));
strcpy(buf.x_label,_("Time"));
strcpy(buf.y_label,_("Volts"));
strcpy(buf.x_units,_("us"));
strcpy(buf.y_units,_("Voltage"));
buf.logscale_x=0;
buf.logscale_y=0;
buffer_add_info(&buf);
buffer_add_xy_data(&buf,out_v.x, out_v.data, out_v.len);
buffer_dump_path(get_output_path(sim),"pulse_v.dat",&buf);
buffer_free(&buf);


buffer_malloc(&buf);
buf.y_mul=1.0;
buf.x_mul=1e6;
strcpy(buf.title,_("Time - Photogeneration rate"));
strcpy(buf.type,_("xy"));
strcpy(buf.x_label,_("Time"));
strcpy(buf.y_label,_("Generation rate"));
strcpy(buf.x_units,_("s"));
strcpy(buf.y_units,_("m^{-3} s^{-1}"));
buf.logscale_x=0;
buf.logscale_y=0;
buffer_add_info(&buf);
buffer_add_xy_data(&buf,out_G.x, out_G.data, out_G.len);
buffer_dump_path(get_output_path(sim),"pulse_G.dat",&buf);
buffer_free(&buf);


//sprintf(outpath,"%s%s",get_output_path(sim),"pulse_lost_charge.dat");
//inter_save(&lost_charge,outpath);



in->go_time=FALSE;

inter_free(&out_G);
inter_free(&out_i);
inter_free(&out_v);
time_memory_free();
}

void pulse_load_config(struct simulation *sim,struct pulse *in,struct device *dev,char *config_file_name)
{

char name[200];
char laser_name[200];
struct inp_file inp;
inp_init(sim,&inp);
inp_load_from_path(sim,&inp,get_input_path(sim),config_file_name);
inp_check(sim,&inp,1.28);

inp_search_gdouble(sim,&inp,&(in->pulse_shift),"#pulse_shift");
inp_search_string(sim,&inp,name,"#pulse_sim_mode");
inp_search_gdouble(sim,&inp,&(in->pulse_L),"#pulse_L");
inp_search_gdouble(sim,&inp,&(in->pulse_Rload),"#Rload");
inp_search_string(sim,&inp,laser_name,"#pump_laser");
light_load_laser(sim,(&dev->mylight),laser_name);
in->pulse_sim_mode=english_to_bin(sim,name);

inp_free(sim,&inp);

}
