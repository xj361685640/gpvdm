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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "dump.h"
#include "sim.h"
#include "ntricks.h"

static int glob_use_cap = 0;

static gdouble Rload;

void ntricks_externv_set_load(gdouble R)
{
	Rload = R;
}

gdouble ntricks_externv_newton(struct device *in, gdouble Vtot, int usecap)
{
	gdouble C = in->C;
	solve_all(in);
	if (glob_use_cap == FALSE)
		C = 0.0;
	return get_I(in) + in->Vapplied / in->Rshunt + C * (in->Vapplied -
							    in->Vapplied_last) /
	    in->dt;
}
