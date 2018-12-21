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

/** @file light_materials.c
	@brief This loads in any physical spectra for the light model, not alpha/n data is stored in the epitaxy.
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "util.h"
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
#include <lang.h>
#include "log.h"
#include <cal_path.h>
#include <buffer.h>

static int unused __attribute__((unused));

void light_load_materials(struct simulation *sim,struct light *in)
{
printf_log(sim,"%s\n",_("load: materials"));
char temp[100];
char file_path[400];

DIR *theFolder;

struct inp_file inp;

/////////////////////////////////////////////////////
theFolder = opendir(get_spectra_path(sim));
if (theFolder==NULL)
{
	ewe(sim,_("Optical spectra directory not found\n"));
}
closedir (theFolder);
inp_init(sim,&inp);

join_path(3,file_path,get_spectra_path(sim),in->suns_spectrum_file,"mat.inp");

inp_load(sim,&inp,file_path);
inp_search_string(sim,&inp,temp,"#spectra_equation_or_data");

inp_free(sim,&inp);
	

	if (strcmp(temp,"equation")==0)
	{
		join_path(3, file_path,get_spectra_path(sim),in->suns_spectrum_file,"spectra_gen.inp");
	}else
	if (strcmp(temp,"data")==0)
	{
		join_path(3,file_path,get_spectra_path(sim),in->suns_spectrum_file,"spectra.inp");
	}else
	{
		ewe(sim,"%s: %s\n",_("spectrum file: I do not know what to do with %s"),temp);		
	}

	if (isfile(file_path)!=0)
	{
		ewe(sim,"%s: %s\n",_("File not found"),file_path);
	}
	
inter_load(sim,&(in->sun_read),file_path);
inter_sort(&(in->sun_read));
inter_mod(&(in->sun_read));
long double sun_value=0.0;

int ret=0;
ret=inter_search_token(sim,&sun_value,"#Psun",file_path);
if (ret==0)
{
	in->Psun0=sun_value;
}else
{
	in->Psun0=1000.0;		//The 1000 is because it is 1000 W/m2	
}


}


