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

/** @file advmath.h
	@brief Math functions which have been moved away from the main code for compiler optimization.
*/
#ifndef advmath_h
#define advmath_h
#include <math.h>

#define gdouble long double
#define gpow powl
#define gcabs cabsl
#define gcreal creall
#define gcimag cimagl
#define gfabs fabsl
#define gexp expl
#define gsqrt sqrtl

long double B(long double x);
long double dB(long double x);
#endif
