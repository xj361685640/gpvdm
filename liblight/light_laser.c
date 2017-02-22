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



#include <unistd.h>
#include "const.h"
#include "light.h"
#include "device.h"
#include "const.h"
#include "dump.h"
#include "config.h"
#include "inp.h"
#include "util.h"
#include "hard_limit.h"
#include "epitaxy.h"
#include "lang.h"
#include "log.h"
#include <cal_path.h>

static int unused __attribute__((unused));

int light_load_laser(struct simulation *sim, struct light *in,char *name)
{
	char pwd[1000];
	char file_name[255];
	struct inp_file inp;
	int ret=0;

	if (getcwd(pwd,1000)==NULL)
	{
		ewe(sim,"IO error\n");
	}

	ret=search_for_token(sim,file_name,pwd,"#laser_name",name);

	if (ret==0)
	{
		inp_init(sim,&inp);
		inp_load_from_path(sim,&inp,get_input_path(sim),file_name);
		inp_check(sim,&inp,1.0);

		inp_search_gdouble(sim,&inp,&in->laser_wavelength,"#laserwavelength");
		in->laser_pos=(int)((in->laser_wavelength-in->lstart)/in->dl);

		inp_search_gdouble(sim,&inp,&in->spotx,"#spotx");

		inp_search_gdouble(sim,&inp,&in->spoty,"#spoty");

		inp_search_gdouble(sim,&inp,&in->pulseJ,"#pulseJ");

		inp_search_gdouble(sim,&inp,&in->pulse_width,"#laser_pulse_width");

		inp_free(sim,&inp);
		printf_log(sim,"Loaded laser\n");
	}else
	{
		ewe(sim,"laser name not found\n");
	}
return 0;
}

