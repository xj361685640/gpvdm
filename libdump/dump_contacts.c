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

/** @file dump_contacts.c
@brief dump JV curves from the contacts.
*/

#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <i.h>
#include <exp.h>
#include <dos.h>
#include "sim.h"
#include "dump.h"
#include "buffer.h"

#include "memory.h"
#include "contacts.h"
#include <lang.h>
#include <cal_path.h>
#include "contacts_vti_store.h"

static int unused __attribute__((unused));

void dump_contacts_init(struct simulation *sim,struct device *in,struct contacts_vti_store *store)
{
	if (in->ncontacts>2)
	{
		int i=0;

		for (i=0;i<in->ncontacts;i++)
		{
			inter_init(sim,&(store->i[i]));
		}
	}
}

void dump_contacts_save(struct simulation *sim,struct device *in,struct contacts_vti_store *store)
{
	if (in->ncontacts>2)
	{
		int i;
		int sub=TRUE;
		char temp[200];
		struct buffer buf;
		buffer_init(&buf);
		for (i=0;i<in->ncontacts;i++)
		{
			buffer_malloc(&buf);
			buf.y_mul=1.0;
			buf.x_mul=1e6;
			sprintf(buf.title,"%s",_("Voltage - Current"));
			strcpy(buf.type,"xy");
			strcpy(buf.x_label,_("Voltage"));
			strcpy(buf.x_units,"V");
			strcpy(buf.data_label,_("Current"));
			strcpy(buf.data_units,"A m^{-2}");
			buf.logscale_x=0;
			buf.logscale_y=0;
			buf.x=1;
			buf.y=store->i[i].len;
			buf.z=1;
			buffer_add_info(sim,&buf);
			buffer_add_xy_data(sim,&buf,store->i[i].x, store->i[i].data, store->i[i].len);
			sprintf(temp,"contact%d_i.dat",i);
			buffer_dump_path(sim,sim->output_path,temp,&buf);
			buffer_free(&buf);
		}
	}
}

void dump_contacts_add_data(struct simulation *sim,struct device *in,struct contacts_vti_store *store)
{
	if (in->ncontacts>2)
	{
		int i=0;
		gdouble x_value=0.0;
		for (i=0;i<in->ncontacts;i++)
		{
			x_value=contact_get_active_contact_voltage(sim,in);
	//		inter_append(&(store->v),x_value,contact_get_voltage(sim,in,i));
			inter_append(&(store->i[i]),x_value,contacts_get_J(in,i));
		}

	}
}

void dump_contacts_free(struct simulation *sim,struct device *in,struct contacts_vti_store *store)
{
	if (in->ncontacts>2)
	{
		int i=0;

		for (i=0;i<in->ncontacts;i++)
		{
			//inter_free(&(store->v[i]));
			inter_free(&(store->i[i]));
		}
	}
}

