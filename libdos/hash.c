//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
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

/** @file hash.c
	@brief Hashing function for fast lookup in tables, but the arrays are now linear so you don't need to hash.
*/

#include <stdio.h>
#include "sim.h"

int hashget(gdouble *x,int N,gdouble find)
{
static gdouble *x_=NULL;
static gdouble find_=0.0;
static int steps_=0.0;
if (N==1) return 0;
if ((x_==x)&&(find_==find)) return steps_;
gdouble x0=x[0];
gdouble x1=x[1];
gdouble delta=find-x0;
gdouble step=x1-x0;
int steps=delta/step;

if (steps>(N-2)) steps=N-2;
if (steps<0) steps=0;
x_=x;
find_=find;
steps_=steps;
return steps;
}

