//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie <r.c.i.mackenzie@googlemail.com>
//
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

#ifndef memory_h
#define memory_h

#include <device.h>

void free_srh_bands(struct device *in, gdouble **** var);
void malloc_3d_gdouble(struct device *in, gdouble * (***var));
void free_3d_gdouble(struct device *in, gdouble ***var);
void malloc_3d_int(struct device *in, int * (***var));
void free_3d_int(struct device *in, int ***var);
void malloc_srh_bands(struct device *in, gdouble * (****var));

#endif
