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


#ifndef solver_h
#define solver_h
#include <sim_struct.h>

void solver_test();
void set_solver_dump_every_matrix(int dump);
void solver_precon(int col,int nz,int *Ti,int *Tj, double *Tx,double *b);
int solver(struct simulation *sim,int col,int nz,int *Ti,int *Tj, double *Tx,double *b);
void solver_dump_matrix(int col,int nz,int *Ti,int *Tj, double *Tx,double *b,char *index);
void solver_dump_matrix_ld(int col,int nz,char **Tdebug,int *Ti,int *Tj, long double *Tx,long double *b,char *index);
void solver_print_time();
#endif
