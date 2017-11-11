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


#include <stdio.h>
#include <gsl/gsl_multimin.h>

#include "sim.h"
#include "dump.h"
#include "inp.h"
#include "buffer.h"
#include "dynamic_store.h"
#include "gui_hooks.h"
#include "fit_sin.h"
#include "i.h"
#include "log.h"
#include <cal_path.h>

struct istruct out_i;
struct istruct fit_data;

char *fit_file_prefix;

gdouble last_d=0.0;
gdouble last_magnitude=0.0;
gdouble last_error=0.0;

struct simulation *local_sim;
double sin_f (const gsl_vector *v, void *params)
{
struct buffer buf;
buffer_init(&buf);
buf.norm_x_axis=FALSE;
char name[200];

gdouble e0;
gdouble mag = (gdouble)gsl_vector_get(v, 0);
gdouble delta = (gdouble)gsl_vector_get(v, 1);
double *p = params;
gdouble fit_fx=(gdouble)p[0];
struct istruct test_i;
inter_copy(&test_i,&fit_data,TRUE);
gdouble avg=inter_avg(&test_i);
inter_sub_gdouble(&test_i,avg);
//char name[200];

if (get_dump_status(local_sim,dump_fx)==TRUE)
{
	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1.0;
	strcpy(buf.title,"Current origonal- Time");
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,"Time");
	strcpy(buf.data_label,"Current");
	strcpy(buf.x_units,"$s$");
	strcpy(buf.data_units,"$A$");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=fit_data.len;
	buf.z=1;
	buffer_add_info(local_sim,&buf);
	buffer_add_xy_data(local_sim,&buf,fit_data.x, fit_data.data, fit_data.len);
	sprintf(name,"fx_fit_%s_orig.dat",fit_file_prefix);
	buffer_dump_path(local_sim,get_output_path(local_sim),name,&buf);
	buffer_free(&buf);
}

inter_sin(&test_i,mag,fit_fx,(gdouble)delta);

if (get_dump_status(local_sim,dump_fx)==TRUE)
{
	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1.0;
	strcpy(buf.title,"Current guess - Time");
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,"Time");
	strcpy(buf.data_label,"Current");
	strcpy(buf.x_units,"$s$");
	strcpy(buf.data_units,"$A$");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=test_i.len;
	buf.z=1;
	buffer_add_info(local_sim,&buf);
	buffer_add_xy_data(local_sim,&buf,test_i.x, test_i.data, test_i.len);
	sprintf(name,"fx_fit_%s_guess.dat",fit_file_prefix);
	buffer_dump_path(local_sim,get_output_path(local_sim),name,&buf);
	buffer_free(&buf);
}

inter_sub(local_sim,&test_i,&fit_data);

if (get_dump_status(local_sim,dump_fx)==TRUE)
{
	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1.0;
	strcpy(buf.title,"Current fit error - Time");
	strcpy(buf.type,"xy");
	strcpy(buf.x_label,"Time");
	strcpy(buf.data_label,"Current");
	strcpy(buf.x_units,"$s$");
	strcpy(buf.data_units,"$A$");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=1;
	buf.y=test_i.len;
	buf.z=1;
	buffer_add_info(local_sim,&buf);
	buffer_add_xy_data(local_sim,&buf,test_i.x, test_i.data, test_i.len);
	sprintf(name,"fx_fit_%s_delta.dat",fit_file_prefix);
	buffer_dump_path(local_sim,get_output_path(local_sim),name,&buf);
	buffer_free(&buf);
}

inter_mod(&test_i);
e0=inter_intergrate(&test_i);
inter_free(&test_i);
last_d=delta;
last_magnitude=mag;
last_error=e0;
return (double)e0;
}


void fit_sin(struct simulation *sim,gdouble *ret_mag,gdouble *ret_delta,struct istruct *in,gdouble fx,char * prefix)
{
local_sim=sim;
gdouble size=0.0;
int status=0;
int fitvars=2;
double shift=1.0;
fit_file_prefix=prefix;

//move data to base line
inter_copy(&fit_data,in,TRUE);
gdouble avg=inter_avg(&fit_data);
inter_sub_gdouble(&fit_data,avg);

struct istruct peaks;
inter_init(local_sim,&peaks);
inter_find_peaks(&peaks,&fit_data,TRUE);

double mag=(double)peaks.data[0];
double time_quater=(1.0/((double)fx))/4.0;

double delta=(double)peaks.x[0]-time_quater;
//delta=delta*-1.0;
double par[5] = {1.0, 2.0, 10.0, 20.0, 30.0};
par[0]=(double)fx;
gsl_vector *x;
gsl_vector *ss;

x = gsl_vector_alloc (fitvars);
ss = gsl_vector_alloc (fitvars);

int pos=0;
gsl_vector_set (x, pos, mag);
gsl_vector_set (ss, pos, fabs(mag*shift));
pos++;
gsl_vector_set (x, pos, delta);
gsl_vector_set (ss, pos, fabs(delta*shift));
pos++;

	const gsl_multimin_fminimizer_type *T =	gsl_multimin_fminimizer_nmsimplex;
	gsl_multimin_function minex_func;
	gsl_multimin_fminimizer *s = NULL;

	minex_func.n = fitvars;
	minex_func.f = sin_f;
	minex_func.params = par;

	s = gsl_multimin_fminimizer_alloc (T, fitvars);
	gsl_multimin_fminimizer_set (s, &minex_func, x, ss);
	int stop=FALSE;
	int ittr=0;
	do
	{
		status = gsl_multimin_fminimizer_iterate(s);

		size = gsl_multimin_fminimizer_size (s);
		status = gsl_multimin_test_size (size, 1e-8);

		//if (status == GSL_SUCCESS)
		//{
		//	printf_log (local_sim,"converged to minimum at\n");
		//}
		printf_log(local_sim,"step=%d error=%Le mag=%le delta=%le fx=%Le\n",ittr,last_error,mag, delta,fx);


		if (status != GSL_CONTINUE)
		{
			printf_log(local_sim,"Fitting stopped\n");
			stop=TRUE;
		}
		mag=gsl_vector_get(s->x, 0);
		delta=gsl_vector_get(s->x, 1);
		//printf_log(local_sim,"%s %le %10.3e delta=%10.3e\n",prefix,s->fval,mag,delta);

		if (ittr>200)
		{
			stop=TRUE;
		}
		ittr++;
	}while (stop==FALSE);

//getchar();
*ret_mag=(gdouble)last_magnitude;
*ret_delta=(gdouble)last_d;
gsl_multimin_fminimizer_free (s);
gsl_vector_free(x);
gsl_vector_free(ss);
inter_free(&peaks);
inter_free(&fit_data);

}

