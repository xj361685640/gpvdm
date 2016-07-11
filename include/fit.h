//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
//
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


#ifndef fith
#define fith
#include <advmath.h>
#include <sim_struct.h>

#ifndef windows
	#include <gsl/gsl_multimin.h>
#endif

#define FIT_RUN 0
#define FIT_FINISH 1
#define FIT_RESET 2

#define FIT_MAX 10
struct fits_struct
{
int enabled;
char fit_name[200];
char fit_path[200];
char fit_plugin[200];
double error;
};

struct fitvars
{
struct fits_struct data_set[FIT_MAX];

double simplexmul;
int simplexreset;
int fitvars;
char fit_file[200][100];
char fit_token[200][100];
int fit_line[200];

int randomize;
int random_reset_ittr;
double disable_reset_at;
double converge_error;
int enable_simple_reset;
double constraints_error[100];
int n_constraints;
int iterations;
int sub_iterations;
int sub_iterations_two;
int stall_steps;
int fit_method;
};

int fit_simplex(struct simulation *sim,struct fitvars *fitconfig);
void fit_build_jobs(struct simulation *sim,struct fitvars *fitconfig);
double get_all_error(struct simulation *sim,struct fitvars *myfit);
double get_constraints_error(struct simulation *sim,struct fitvars *config);
int fit_read_config(struct simulation *sim,struct fitvars *fitconfig);
double fit_run_sims(struct simulation *fit,struct fitvars *fitconfig);
int fit_now(struct simulation *sim,struct fitvars *fitconfig);
double fit_load_plugin(struct simulation *sim,struct fitvars *config,int i);
void duplicate(struct simulation *sim);
int get_fit_crashes(struct simulation *sim,struct fitvars *fitconfig);
void fit_init(struct fitvars *fitconfig);
void my_f_set_globals(struct simulation *sim, struct fitvars *config);
int fit_newton(struct simulation *sim,struct fitvars *fitconfig);

#ifndef windows
	double my_f (const gsl_vector *v, void *params);
	void  my_df (const gsl_vector *v, void *params,  gsl_vector *df);
	void my_fdf (const gsl_vector *x, void *params, double *f, gsl_vector *df) ;
	void fit_dump_log(struct simulation *sim,struct fitvars *fitconfig,double error,double size,gsl_vector *old_vector,gsl_vector *old_shift,gsl_vector *ss,gsl_vector *x);

#endif

#endif
