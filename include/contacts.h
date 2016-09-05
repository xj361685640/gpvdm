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




#ifndef contacts_h
#define contacts_h

#include "contact_struct.h"

void contacts_load(struct simulation *sim,struct device *in);
void contacts_update(struct simulation *sim,struct device *in);
gdouble contact_get_voltage(struct simulation *sim,struct device *in,int contact);
void contact_set_voltage(struct simulation *sim,struct device *in,int contact,gdouble voltage);
gdouble contact_get_voltage_last(struct simulation *sim,struct device *in,int contact);
void contacts_time_step(struct simulation *sim,struct device *in);
void contacts_force_value(struct simulation *sim,struct device *in,gdouble value);
void contact_set_all_voltages(struct simulation *sim,struct device *in,gdouble voltage);
void contact_set_voltage_if_active(struct simulation *sim,struct device *in,gdouble voltage);
long double contact_get_active_contact_voltage(struct simulation *sim,struct device *in);
#endif
