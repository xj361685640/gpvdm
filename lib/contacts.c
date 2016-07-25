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


#include <string.h>
#include "epitaxy.h"
#include "inp.h"
#include "util.h"
#include "const.h"
#include <cal_path.h>
#include "contacts.h"

void contacts_time_step(struct simulation *sim,struct device *in)
{
	int i;

	for (i=0;i<in->ncontacts;i++)
	{
		in->contacts[i].voltage_last=in->contacts[i].voltage;
	}
}

void contacts_load(struct simulation *sim,struct device *in)
{
	int i;
	struct inp_file inp;
	in->ncontacts=0;

	inp_init(sim,&inp);
	inp_load(sim, &inp , "contacts.inp");

	inp_check(sim,&inp,1.0);
	inp_reset_read(sim,&inp);
	inp_get_string(sim,&inp);
	sscanf(inp_get_string(sim,&inp),"%d",&(in->ncontacts));

	if (in->ncontacts>10)
	{
		ewe(sim,"Too many contacts\n");
	}

	if (in->ncontacts<1)
	{
		ewe(sim,"No contacts\n");
	}

	gdouble pos=0.0;
	for (i=0;i<in->ncontacts;i++)
	{
		inp_get_string(sim,&inp);	//start
		sscanf(inp_get_string(sim,&inp),"%Le",&(in->contacts[i].start));

		inp_get_string(sim,&inp);	//width
		sscanf(inp_get_string(sim,&inp),"%Le",&(in->contacts[i].width));

		inp_get_string(sim,&inp);	//depth
		sscanf(inp_get_string(sim,&inp),"%Le",&(in->contacts[i].depth));

		inp_get_string(sim,&inp);	//voltage
		sscanf(inp_get_string(sim,&inp),"%Le",&(in->contacts[i].voltage));
		in->contacts[i].voltage_last=in->contacts[i].voltage;

		pos+=in->contacts[i].width;
	}

	char * ver = inp_get_string(sim,&inp);
	if (strcmp(ver,"#ver")!=0)
	{
			ewe(sim,"No #ver tag found in file\n");
	}

	inp_free(sim,&inp);

	contacts_update(sim,in);
}

void contacts_force_value(struct simulation *sim,struct device *in,gdouble value)
{
int x;
int z;

for (x=0;x<in->xmeshpoints;x++)
{
	for (z=0;z<in->zmeshpoints;z++)
	{
		in->Vapplied[z][x]=value;
	}

}

}

void contacts_update(struct simulation *sim,struct device *in)
{
int i;
int x;
int z;
int n;
int found=FALSE;

gdouble value=0.0;

if (in->xmeshpoints==1)
{
	for (z=0;z<in->zmeshpoints;z++)
	{
		in->Vapplied[z][0]=in->contacts[0].voltage;
	}

	return;
}

for (x=0;x<in->xmeshpoints;x++)
{
	found=FALSE;
	for (i=0;i<in->ncontacts;i++)
	{
		if ((in->xmesh[x]>=in->contacts[i].start)&&(in->xmesh[x]<in->contacts[i].start+in->contacts[i].width))
		{
			value=in->contacts[i].voltage;
			n=i;
			found=TRUE;
			break;
		}
	}

	if (found==FALSE)
	{
		ewe(sim,"contact does not extend over whole device\n");
	}

	for (z=0;z<in->zmeshpoints;z++)
	{
		in->Vapplied[z][x]=value;
		in->n_contact[z][x]=n;
	}
}

}

gdouble contact_get_voltage_last(struct simulation *sim,struct device *in,int contact)
{
	return in->contacts[contact].voltage_last;
}

gdouble contact_get_voltage(struct simulation *sim,struct device *in,int contact)
{
	return in->contacts[contact].voltage;
}

void contact_set_voltage(struct simulation *sim,struct device *in,int contact,gdouble voltage)
{
	in->contacts[contact].voltage=voltage;
	contacts_update(sim,in);
}

void contact_set_all_voltages(struct simulation *sim,struct device *in,gdouble voltage)
{
int i;
	for (i=0;i<in->ncontacts;i++)
	{
		in->contacts[i].voltage=voltage;
	}

	contacts_update(sim,in);
}
