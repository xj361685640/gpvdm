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

	for (i=0;i<in->ncontacts;i++)
	{
		inp_get_string(sim,&inp);	//width
		sscanf(inp_get_string(sim,&inp),"%Le",&(in->contacts[i].width));

		inp_get_string(sim,&inp);	//depth
		sscanf(inp_get_string(sim,&inp),"%Le",&(in->contacts[i].depth));

		inp_get_string(sim,&inp);	//voltage
		sscanf(inp_get_string(sim,&inp),"%Le",&(in->contacts[i].voltage));
		in->contacts[i].voltage_last=in->contacts[i].voltage;

	}

	char * ver = inp_get_string(sim,&inp);
	if (strcmp(ver,"#ver")!=0)
	{
			ewe(sim,"No #ver tag found in file\n");
	}

	inp_free(sim,&inp);
}

