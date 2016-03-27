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

#ifndef dll_interface_h
#define dll_interface_h

#include "light.h"
#include "device.h"
#include "solver_interface.h"

struct dll_interface
{
	int (*get_dump_status)(int);
	void (*light_dump_1d)(struct light *, int , char * );
	void (*light_solve_optical_problem)(struct light *);
	void (*light_free_memory)(struct light *);
	void (*light_transfer_gen_rate_to_device)(struct device *,struct light *);
	int (*complex_solver)(int col,int nz,int *Ti,int *Tj, double *Tx, double *Txz,double *b,double *bz);
	gdouble (*get_n_den)(gdouble top,gdouble T, int mat);
	gdouble (*get_dn_den)(gdouble top,gdouble T, int mat);
	gdouble (*get_n_w)(gdouble top,gdouble T,int mat);
	gdouble (*get_p_den)(gdouble top,gdouble T, int mat);
	gdouble (*get_dp_den)(gdouble top,gdouble T, int mat);
	gdouble (*get_p_w)(gdouble top,gdouble T,int mat);
	void (*dump_matrix)(int col,int nz,int *Ti,int *Tj, long double *Tx,long double *b,char *index);
	int (*ewe)( const char *format, ...);
	void (*solver)(int col,int nz,int *Ti,int *Tj, long double *Tx,long double *b);
	void (*dump_1d_slice)(struct device *in,char *out_dir);

	void (*update_arrays)(struct device *in);
	void (*remesh_reset)(struct device *in,gdouble voltage);
	void (*newton_set_min_ittr)(int ittr);
	void (*solver_realloc)(struct device * in);
	void (*solve_all)(struct device *in);

	void (*ntricks_externv_set_load)(gdouble R);
	gdouble (*sim_externalv)(struct device *in,gdouble wantedv);
	gdouble (*ntricks_externv_newton)(struct device *in,gdouble Vtot,int usecap);
	void (*device_timestep)(struct device *in);
	gdouble (*sim_i)(struct device *in,gdouble wantedi);
	struct device *in;
};


struct dll_interface *dll_get_interface();
void dll_interface_fixup(struct device* in);

#endif
