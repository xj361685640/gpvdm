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



#ifndef h_dll_export
#define h_dll_export

#ifdef windows
	#ifdef BUILDING_EXAMPLE_DLL
	#define EXPORT __declspec(dllexport)
	#else
	#define EXPORT __declspec(dllimport)
	#endif
#else
	#define EXPORT
#endif

#include <sim_struct.h>
#include <light.h>
#include <device.h>

extern struct dll_interface *fun;

EXPORT void set_interface();

//Matrix solver
EXPORT void dll_matrix_solve(struct simulation *sim,int col,int nz,int *Ti,int *Tj, long double *Tx,long double *b);
EXPORT void dll_matrix_dump(struct simulation *sim,int col,int nz,int *Ti,int *Tj, long double *Tx,long double *b,char *index);
EXPORT void dll_matrix_solver_free(struct simulation *sim);

//Light
EXPORT void light_dll_init(struct simulation *sim);
EXPORT int light_dll_solve_lam_slice(struct simulation *sim,struct light *in,int lam);
EXPORT void light_dll_ver(struct simulation *sim);
EXPORT void light_fixup(char *name,void (*in));

//Newton solver
EXPORT void dll_newton_set_min_ittr(int ittr);
EXPORT int dll_solve_cur(struct simulation *sim,struct device *in);
EXPORT void dll_solver_realloc(struct simulation *sim,struct device *in);
EXPORT void dll_solver_free_memory(struct device *in);

//electrical plugin
EXPORT void dll_run_simulation(struct simulation *sim,struct device *in);

#endif
