//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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


#ifndef h_complex_solver
#define h_complex_solver
#include <sim_struct.h>

void error_report(int status, const char *file, const char *func, int line);
void complex_solver_dump_matrix(struct simulation *sim,int col,int nz,int *Ti,int *Tj, double *Tx, double *Txz,double *b,double *bz);
void complex_solver_free(struct simulation *sim);
int complex_solver(struct simulation *sim,int col,int nz,int *Ti,int *Tj, double *Tx, double *Txz,double *b,double *bz);
void complex_solver_print(struct simulation *sim,int col,int nz,int *Ti,int *Tj, double *Tx, double *Txz,double *b,double *bz);
#endif
