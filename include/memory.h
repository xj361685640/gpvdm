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

#ifndef memory_h
#define memory_h

#include <device.h>

void three_d_copy_gdouble(struct device *in, gdouble ***dst, gdouble ***src);
void three_d_mul_gdouble(struct device *in, gdouble ***src, gdouble val);
void malloc_zx_gdouble(struct device *in, gdouble * (**var));
void free_zx_gdouble(struct device *in, gdouble **var);
void free_srh_bands(struct device *in, gdouble **** var);
void malloc_3d_gdouble(struct device *in, gdouble * (***var));
void free_3d_gdouble(struct device *in, gdouble ***var);
void malloc_3d_int(struct device *in, int * (***var));
void free_3d_int(struct device *in, int ***var);
void malloc_srh_bands(struct device *in, gdouble * (****var));
void malloc_zx_int(struct device *in, int * (**var));
void free_zx_int(struct device *in, int **var);

//3d opps
void three_d_set_gdouble(struct device *in, gdouble ***var, gdouble val);
void three_d_sub_gdouble(struct device *in, gdouble ***var, gdouble ***sub);
void three_d_add_gdouble(struct device *in, gdouble ***var, gdouble ***add);

#endif
