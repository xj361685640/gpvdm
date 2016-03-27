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

#ifndef fith
#define fith
#include <server.h>
#include <advmath.h>

struct fitvars
{
int fit_names_len;
char **fit_names;
double fit_names_error[100];
double simplexmul;
int simplexreset;
int fitvars;
char fit_file[200][100];
int fit_pos[200];
int newton;
int randomize;
int random_reset_time;
double disable_reset_at;
double converge_error;
};

int fit_simplex(int *oppcount);
int fit_newton(int *oppcount);
void fit_build_jobs(struct server *myserver,char **name,int n);
double get_all_error(struct fitvars *myfit);
double get_constraints_error(struct fitvars *config);
int fit_read_config(struct fitvars *config);
double fit_run_sims(struct fitvars *fitconfig,struct server *myserver);
int fit_now();
double fit_load_plugin(struct fitvars *config,char *name);
#endif
