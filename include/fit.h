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

struct fitvars
{
int fit_enabled[10];
double fit_names_error[10];
char fit_name[200][100];
char fit_path[200][100];
double simplexmul;
int simplexreset;
int fitvars;
char fit_file[200][100];
char fit_token[200][100];
char fit_plugin[200][100];
int randomize;
int random_reset_time;
double disable_reset_at;
double converge_error;
};

int fit_simplex(struct simulation *sim,int *oppcount);
int fit_newton(struct simulation *sim,int *oppcount);
void fit_build_jobs(struct simulation *sim,struct fitvars *fitconfig);
double get_all_error(struct simulation *sim,struct fitvars *myfit);
double get_constraints_error(struct simulation *sim,struct fitvars *config);
int fit_read_config(struct simulation *sim,struct fitvars *fitconfig);
double fit_run_sims(struct simulation *fit,struct fitvars *fitconfig);
int fit_now(struct simulation *sim,int *oppcount);
double fit_load_plugin(struct simulation *sim,struct fitvars *config,int i);
void duplicate(struct simulation *sim);
int get_fit_crashes(struct simulation *sim,struct fitvars *fitconfig);
#endif
