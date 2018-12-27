//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
//
//  Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
//
//	www.rodmack.com
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

/** @file remesh.h
@brief remeshing functions, sometimes it is good to change the mesh during a simulation for speed.
*/

#ifndef h_remesh
#define h_remesh
#include "sim.h"

struct remesh
{
gdouble *x;
int len;
gdouble new_dx;
};

void remesh_shrink(struct device *in);
void remesh_reset(struct device *in,gdouble voltage);
#endif
