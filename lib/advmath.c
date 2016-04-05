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

#include <math.h>

//Taken from mathworld and wikipidia
long double dB(long double x)
{
	long double ret;
	if (fabsl(x) > 1e-40) {
		ret =
		    (expl(x) - 1.0 - x * expl(x)) / (expl(x) - 1) / (expl(x) -
								     1);
	} else {
		ret =
		    -1.0 / 2.0 + x / 6.0 - powl(x, 3.0) / 180.0 + powl(x,
								       5.0) /
		    5040;
	}

	return ret;
}

long double B(long double x)
{
	long double ret;
	if (fabsl(x) > 1e-40) {
		ret = x / (expl(x) - 1.0);
//From mathworld and wikipidia
	} else {
		ret =
		    1 - x / 2.0 + powl(x, 2.0) / 12.0 - powl(x,
							     4.0) / 720.0 +
		    powl(x, 6.0) / 30240.0;

	}
	return ret;
}
