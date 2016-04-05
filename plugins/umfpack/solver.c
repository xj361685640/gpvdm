//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie
//
//      roderick.mackenzie@nottingham.ac.uk
//      www.roderickmackenzie.eu
//      Room B86 Coates, University Park, Nottingham, NG7 2RD, UK
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

#include <umfpack.h>
#include <solver_interface.h>
#include <dll_export.h>
#include <util.h>
#include "lib.h"

struct dll_interface *fun;

EXPORT void set_interface(struct dll_interface *in)
{
	fun = in;
}

EXPORT void dll_matrix_solve(int col, int nz, int *Ti, int *Tj, long double *Tx,
			     long double *b)
{
	umfpack_solver(col, nz, Ti, Tj, Tx, b);
}

EXPORT void dll_matrix_dump(int col, int nz, int *Ti, int *Tj, long double *Tx,
			    long double *b, char *index)
{
	printf("hello\n");
}

EXPORT void dll_matrix_solver_free()
{
	umfpack_solver_free();
}
