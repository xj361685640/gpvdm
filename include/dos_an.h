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


#ifndef dos_an_h
#define dos_an_h

#include <sim_struct.h>

struct dos_an_data{
int items;
char file[100];
int type[100];
int enable[100];
double a[100];
double b[100];
double c[100];
};

void dos_an_load(struct simulation *sim,struct dos_an_data *in,char *name);
double dos_an_get_value(struct dos_an_data *in,double E);
#endif
