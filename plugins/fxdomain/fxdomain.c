//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012-2017 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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

/** @file fxdomain.c
	@brief Fx domain solver.
*/

#include <sim.h>
#include <dump.h>
#include <ntricks.h>
#include <dynamic_store.h>
#include <sys/stat.h>
#include <exp.h>
#include "fxdomain.h"
#include <inp.h>
#include <buffer.h>
#include <gui_hooks.h>
#include <lang.h>
#include <fx.h>
#include <fit_sin.h>
#include <log.h>
#include <cal_path.h>
#include <newton_externalv.h>
#include <newton_voc.h>
#include <contacts.h>
#include <memory.h>


static int unused __attribute__((unused));

struct fxdomain fxdomain_config;

void sim_fxdomain(struct simulation *sim,struct device *in)
{
light_solve_and_update(sim,in,&(in->mylight),0.0);

struct buffer buf;
buffer_init(&buf);
struct dynamic_store store;
dump_dynamic_init(sim,&store,in);

struct istruct out_j;
inter_init(sim,&out_j);

struct istruct out_j_cut;
inter_init(sim,&out_j_cut);

struct istruct out_modulation;
inter_init(sim,&out_modulation);

struct istruct out_modulation_cut;
inter_init(sim,&out_modulation_cut);

struct istruct out_fx;
inter_init(sim,&out_fx);

struct istruct real_imag;
inter_init(sim,&real_imag);

char out_dir[1000];
join_path(2, out_dir,get_output_path(sim),"frequency");

if (get_dump_status(sim,dump_fx)==TRUE)
{
	struct stat st = {0};

	

	if (stat(out_dir, &st) == -1)
	{
		mkdir(out_dir, 0700);
	}
}


char config_file_name[200];

gdouble lambda=0.0;
gdouble stop_time=0.0;

if (find_config_file(sim,config_file_name,get_input_path(sim),in->simmode,"fxdomain")!=0)
{
	ewe(sim,"%s %s %s\n",_("no fxdomain config file found"),get_input_path(sim),in->simmode);
}

fxdomain_load_config(sim,&fxdomain_config,in,config_file_name);

int number=strextract_int(config_file_name);


fx_load_mesh(sim,in,number);
gdouble fx=0.0;
gdouble i0=0;
gdouble Plight=0.0;
gdouble V=0.0;
int step=0;
gdouble cut_time=0.0;
char send_data[200];
V=fxdomain_config.fxdomain_Vexternal;
Plight=light_get_sun((&in->mylight));
int total_steps=fxdomain_config.fxdomain_points*fxdomain_config.fxdomain_n*fx_points();
int cur_total_step=0;
char temp[200];
int modulate_voltage=TRUE;
long double modulation=0.0;
long double dc=0.0;
long double i0_dc=0.0;
do
{
	V=fxdomain_config.fxdomain_Vexternal;

	fx=fx_get_fx();

	printf_log(sim,"%s %Lf\n",_("Running frequency"),fx);

	in->go_time=FALSE;

	in->time=0.0;

	time_init(sim,in);

	lambda=(1.0/fx);
	in->dt=lambda/((gdouble)fxdomain_config.fxdomain_points);
	stop_time=lambda*fxdomain_config.fxdomain_n;

	Plight=light_get_sun((&in->mylight));
	light_set_sun((&in->mylight),Plight);

	light_solve_and_update(sim,in,&(in->mylight), 0.0);
	step=0;

	if (fxdomain_config.fxdomain_sim_mode==fxdomain_load)
	{
		//printf("running sim_externalv:\n");
		//getchar();
		sim_externalv(sim,in,V);
		//printf("newton_externalv:\n");
		//getchar();
		i0=newton_externv(sim,in,V,FALSE);
		//printf("newton_externalv: %Le %Le\n",contact_get_voltage_last(sim,in,0),contact_get_active_contact_voltage(sim,in));
		//getchar();
		//newton_externv(sim,in,V,FALSE);
		//getchar();
		
	}else
	if (fxdomain_config.fxdomain_sim_mode==fxdomain_open_circuit)
	{
		contact_set_active_contact_voltage(sim,in,in->Vbi);
		//contact_set_voltage(sim,in,0,in->Vbi);
		newton_sim_voc(sim,in);
		newton_sim_voc_fast(sim,in,FALSE);
	}else
	{
		ewe(sim,"%s\n",_("fxdomain mode not known"));
	}

	i0_dc=i0;
	
	device_timestep(sim,in);

	in->go_time=TRUE;

	carrier_count_reset(in);
	reset_np_save(in);

	cut_time=0.0;

	fx_with_units(temp,fx);
	sprintf(send_data,"text:%s",temp);
	gui_send_data(sim,send_data);



	do
	{

			modulation=fxdomain_config.fxdomain_voltage_modulation_max*sin(2*PI*fx*in->time);
			V=fxdomain_config.fxdomain_Vexternal+modulation;

		if (fxdomain_config.fxdomain_sim_mode==fxdomain_load)
		{
			i0=newton_externv(sim,in,V,TRUE);
			//printf("newton_externalv: %Le\n",contact_get_active_contact_voltage(sim,in));
			//getchar();
		}else
		if (fxdomain_config.fxdomain_sim_mode==fxdomain_open_circuit)
		{
			V=contact_get_active_contact_voltage(sim,in);//contact_get_voltage(sim,in,0);
			i0=newton_sim_voc_fast(sim,in,TRUE);
		}else
		{
			ewe(sim,"%s\n",_("fxdomain mode not known"));
		}


		if (get_dump_status(sim,dump_print_text)==TRUE)
		{
			printf_log(sim,"%s=%Le %s=%d %.1Le ",_("fxdomain time"),in->time,_("step"),step,in->last_error);
			printf_log(sim,"Vtot=%Lf Plight=%Lf %s = %Le mA (%Le A/m^2)\n",V,Plight,_("current"),get_I(in)/1e-3,get_J(in));
		}

		dump_dynamic_add_data(sim,&store,in,in->time);


		inter_append(&out_j,in->time,(i0-i0_dc)/in->area);
		inter_append(&out_modulation,in->time,modulation);

		if ((fxdomain_config.fxdomain_n-fxdomain_config.periods_to_fit)*lambda>=in->time)
		{
			inter_append(&out_j_cut,cut_time,(i0-i0_dc)/in->area);
			inter_append(&out_modulation_cut,cut_time,modulation);
			cut_time+=in->dt;
		}
		//dump_write_to_disk(in);

		device_timestep(sim,in);
		cur_total_step++;
		step++;


		if (in->time>stop_time) break;

	}while(1);
	
	sprintf(send_data,"percent:%Lf",(gdouble)(cur_total_step)/(gdouble)(total_steps));
	gui_send_data(sim,send_data);

	gdouble modulation_mag=0.0;
	gdouble i_mag=0.0;
	gdouble modulation_delta=0.0;
	gdouble j_delta=0.0;
	gdouble real=0.0;
	gdouble imag=0.0;


	if (fxdomain_config.fxdomain_do_fit==TRUE)
	{
		printf("fitting i\n");
		fit_sin(sim,&i_mag,&j_delta,&out_j_cut,fx,"j");
		printf("fitting modulation\n");
		fit_sin(sim,&modulation_mag,&modulation_delta,&out_modulation_cut,fx,"modulation");
		
		printf_log(sim,"modulation_delta=%Le j_delta=%Le\n",modulation_delta,j_delta);
		gdouble dphi=2*PI*fx*(j_delta-modulation_delta);
		printf_log(sim,"delta_phi=%Lf\n",dphi);

		gdouble mag=i_mag;
		real=mag*cosl(dphi);
		imag=mag*sinl(dphi);

		inter_append(&real_imag,real,imag);
		inter_append(&out_fx,fx,fx);
		
		//inter_dft(&real,&imag,&out_j_cut,imps_config.imps_modulation_fx);
		double ang=atan2(imag,real)*360/(2.0*PI);
		printf_log(sim,"real=%Le imag=%Le ang=%lf\n",real,imag,ang);

		FILE *out;
		out=fopena(get_input_path(sim),"sim_info.dat","w");
		fprintf(out,"#fxdomain_r\n%Le\n",real*in->area);
		fprintf(out,"#fxdomain_i\n%Le\n",imag*in->area);
		fprintf(out,"#fxdomain_Jr\n%Le\n",real);
		fprintf(out,"#fxdomain_Ji\n%Le\n",imag);
		fprintf(out,"#fxdomain_fx\n%Le\n",fx);
		fprintf(out,"#fxdomain_delta_j\n%Le\n",j_delta);
		fprintf(out,"#fxdomain_delta_g\n%Le\n",modulation_delta);
		fprintf(out,"#fxdomain_delta_phase\n%Le\n",dphi);
		fprintf(out,"#ver\n");
		fprintf(out,"1.0\n");
		fprintf(out,"#end");
		//fprintf(out,"#max_nfree_to_ptrap\n%le\n",nfree_to_ptrap);
		//fprintf(out,"#max_pfree_to_ntrap\n%le\n",pfree_to_ntrap);
		//fprintf(out,"#max_nrelax\n%le\n",nrelax);
		//fprintf(out,"#max_prelax\n%le\n",prelax);
		fclose(out);

	}

	printf_log(sim,"%Lf %Lf\n",real,imag);
	///////////////cut time current///////////////////

	if (get_dump_status(sim,dump_fx)==TRUE)
	{
		buffer_malloc(&buf);
		buf.y_mul=1e3;
		buf.x_mul=1e6;
		sprintf(buf.title,"%s - %s",_("Time"),_("current"));
		strcpy(buf.type,"xy");
		strcpy(buf.x_label,_("Time"));
		strcpy(buf.data_label,_("Current"));
		strcpy(buf.x_units,"\\ms");
		strcpy(buf.data_units,"m");
		buf.logscale_x=0;
		buf.logscale_y=0;
		buf.x=1;
		buf.y=out_j_cut.len;
		buf.z=1;
		buffer_add_info(sim,&buf);
		buffer_add_xy_data(sim,&buf,out_j_cut.x, out_j_cut.data, out_j_cut.len);
		sprintf(temp,"fxdomain_j%d.dat",(int)fx);
		buffer_dump_path(sim,out_dir,temp,&buf);
		buffer_free(&buf);

		///////////////////non cut time current//////////////////////////

		char temp[2000];
		buffer_malloc(&buf);
		buf.y_mul=1e3;
		buf.x_mul=1e6;
		sprintf(buf.title,"%s - %s",_("Time"),_("current"));
		strcpy(buf.type,"xy");
		strcpy(buf.x_label,_("Time"));
		strcpy(buf.data_label,_("Current"));
		strcpy(buf.x_units,"\\ms");
		strcpy(buf.data_units,"m");
		buf.logscale_x=0;
		buf.logscale_y=0;
		buf.x=1;
		buf.y=out_j_cut.len;
		buf.z=1;
		buffer_add_info(sim,&buf);
		buffer_add_xy_data(sim,&buf,out_j.x, out_j.data, out_j.len);
		sprintf(temp,"fxdomain_j%d_full.dat",(int)fx);
		buffer_dump_path(sim,out_dir,temp,&buf);
		buffer_free(&buf);
		
		///////////////////////cut modulation intensity file/////////////
		buffer_malloc(&buf);
		buf.y_mul=1.0;
		buf.x_mul=1e6;
			sprintf(buf.title,"%s - %s",_("Time"),_("Voltage"));

		strcpy(buf.type,"xy");
		strcpy(buf.x_label,_("Time"));
			strcpy(buf.data_label,_("Voltage"));

		strcpy(buf.x_units,"\\ms");
			strcpy(buf.data_units,"V");
		buf.logscale_x=0;
		buf.logscale_y=0;
		buf.x=1;
		buf.y=out_modulation_cut.len;
		buf.z=1;
		buffer_add_info(sim,&buf);
		buffer_add_xy_data(sim,&buf,out_modulation_cut.x, out_modulation_cut.data, out_modulation_cut.len);
		sprintf(temp,"fxdomain_modulation%d.dat",(int)fx);

		buffer_dump_path(sim,out_dir,temp,&buf);
		buffer_free(&buf);

		/////////////////////////////non cut modulation intensity file/////////////////////

		buffer_malloc(&buf);
		buf.y_mul=1.0;
		buf.x_mul=1e6;
			sprintf(buf.title,"%s - %s",_("Time"),_("Voltage"));

		strcpy(buf.type,"xy");
		strcpy(buf.x_label,_("Time"));
			strcpy(buf.data_label,_("Voltage"));

		strcpy(buf.x_units,"\\ms");
			strcpy(buf.data_units,"V");
		buf.logscale_x=0;
		buf.logscale_y=0;
		buf.x=1;
		buf.y=out_modulation_cut.len;
		buf.z=1;
		buffer_add_info(sim,&buf);
		buffer_add_xy_data(sim,&buf,out_modulation.x, out_modulation.data, out_modulation.len);
		sprintf(temp,"fxdomain_modulation%d_full.dat",(int)fx);
		buffer_dump_path(sim,out_dir,temp,&buf);
		buffer_free(&buf);
		
		/////////////////////////////////////////////
	}
	fx_step();

	inter_reset(&out_j);
	inter_reset(&out_j_cut);
	inter_reset(&out_modulation);
	inter_reset(&out_modulation_cut);

}while(fx_run()==TRUE);

dump_dynamic_save(sim,get_output_path(sim),&store);
dump_dynamic_free(sim,in,&store);



buffer_malloc(&buf);
buf.y_mul=1e3;
buf.x_mul=1e3;
sprintf(buf.title,"%s - %s",_("Real"),_("Imaginary"));
strcpy(buf.type,"xy");
strcpy(buf.x_label,_("Re(i)"));
strcpy(buf.data_label,_("Im(i)"));
strcpy(buf.x_units,"mA");
strcpy(buf.data_units,"mA");
buf.logscale_x=0;
buf.logscale_y=0;
buf.x=1;
buf.y=real_imag.len;
buf.z=1;
buffer_add_info(sim,&buf);
buffer_add_xy_data_z_label(&buf,real_imag.x, real_imag.data, out_fx.x, real_imag.len);
buffer_dump_path(sim,get_output_path(sim),"fxdomain_real_imag.dat",&buf);
buffer_free(&buf);


in->go_time=FALSE;

inter_free(&out_j);
inter_free(&out_j_cut);
inter_free(&out_modulation);
inter_free(&out_modulation_cut);
inter_free(&out_fx);
inter_free(&real_imag);
time_memory_free(in);
}

void fxdomain_load_config(struct simulation *sim,struct fxdomain *in,struct device *dev,char *config_file_name)
{

char name[200];
struct inp_file inp;
inp_init(sim,&inp);
inp_load_from_path(sim,&inp,get_input_path(sim),config_file_name);
inp_check(sim,&inp,1.1);

inp_search_string(sim,&inp,name,"#fxdomain_sim_mode");
in->fxdomain_sim_mode=english_to_bin(sim,name);

inp_search_gdouble(sim,&inp,&(dev->Rload),"#fxdomain_Rload");
inp_search_int(sim,&inp,&(in->fxdomain_points),"#fxdomain_points");
inp_search_int(sim,&inp,&(in->fxdomain_n),"#fxdomain_n");
inp_search_gdouble(sim,&inp,&(in->fxdomain_Vexternal),"#fxdomain_Vexternal");
inp_search_gdouble(sim,&inp,&(in->fxdomain_voltage_modulation_max),"#fxdomain_voltage_modulation_max");
inp_search_gdouble(sim,&inp,&(in->fxdomain_light_modulation_depth),"#fxdomain_light_modulation_depth");
inp_search_string(sim,&inp,in->fxdomain_modulation_type,"#fx_modulation_type");

inp_search_int(sim,&inp,&(in->periods_to_fit),"#periods_to_fit");
in->fxdomain_do_fit=inp_search_english(sim,&inp,"#fxdomain_do_fit");
inp_search_gdouble(sim,&inp,&(in->fxdomain_L),"#fxdomain_L");

in->fxdomain_modulation_rolloff_enable=inp_search_english(sim,&inp,"#fxdomain_modulation_rolloff_enable");
inp_search_gdouble(sim,&inp,&(in->fxdomain_modulation_rolloff_start_fx),"#fxdomain_modulation_rolloff_start_fx");
inp_search_gdouble(sim,&inp,&(in->fxdomain_modulation_rolloff_speed),"#fxdomain_modulation_rolloff_speed");


inp_free(sim,&inp);

}
