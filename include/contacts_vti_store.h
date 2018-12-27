//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2017 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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

/** @file contacts_vti_store.h
	@brief No idea what this does.
*/
#ifndef contacts_vti_store_h
#define contacts_vti_store_h
#include "i.h"


struct contacts_vti_store
{
	struct istruct x[10];
	struct istruct v[10];
	struct istruct i[10];
};

void dump_contacts_init(struct simulation *sim,struct device *in,struct contacts_vti_store *store);
void dump_contacts_save(struct simulation *sim,struct device *in,struct contacts_vti_store *store);
void dump_contacts_add_data(struct simulation *sim,struct device *in,struct contacts_vti_store *store);
void dump_contacts_free(struct simulation *sim,struct device *in,struct contacts_vti_store *store);

#endif
