//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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

/** @file contacts.h
	@brief Header to handle complex contacts.
*/


#ifndef contacts_h
#define contacts_h

#include "contact_struct.h"

gdouble contact_get_active_contact_voltage_last(struct simulation *sim,struct device *in);
void contacts_passivate(struct simulation *sim,struct device *in);
void contacts_load(struct simulation *sim,struct device *in);
void contacts_update(struct simulation *sim,struct device *in);
gdouble contact_get_voltage(struct simulation *sim,struct device *in,int contact);
void contact_set_voltage(struct simulation *sim,struct device *in,int contact,gdouble voltage);
gdouble contact_get_voltage_last(struct simulation *sim,struct device *in,int contact);
void contacts_time_step(struct simulation *sim,struct device *in);
void contacts_force_to_zero(struct simulation *sim,struct device *in);
void contact_set_all_voltages(struct simulation *sim,struct device *in,gdouble voltage);
void contact_set_active_contact_voltage(struct simulation *sim,struct device *in,gdouble voltage);
long double contact_get_active_contact_voltage(struct simulation *sim,struct device *in);
long double contacts_get_J(struct device *in, int n);
long double contacts_get_Jleft(struct device *in);
long double contacts_get_Jright(struct device *in);
int contacts_get_active_contact_left_right(struct device *in);
void contacts_dump(struct simulation *sim,struct device *in);
#endif
