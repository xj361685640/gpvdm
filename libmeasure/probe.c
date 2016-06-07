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


#include "util.h"
#include "const.h"
#include "light.h"
#include "device.h"
#include "const.h"
#include "dump.h"
#include "config.h"
#include "inp.h"
#include "util.h"
#include "cal_path.h"
#include "lang.h"
#include "log.h"
#include <probe.h>
#include <buffer.h>
#include <sys/stat.h>

static int unused __attribute__((unused));

struct probe_config config;

static struct istruct spectrum_first;
//static struct istruct spectrum_reflect_first;
//static struct istruct reflect;
static int first=FALSE;

gdouble probe_cal(struct simulation *sim,struct device *in,	struct istruct *probe_mode)
{
}

void dump_probe_spectrum(struct simulation *sim,struct device *in,char *out_dir,int dump_number)
{
	int i;
	char full_name[100];
	struct istruct probe_mode;
	struct istruct spectrum;
	//struct istruct spectrum_reflect;
	struct buffer buf;

	struct stat st = {0};

	if (stat(out_dir, &st) == -1)
	{
		mkdir(out_dir, 0700);
	}

	buffer_init(&buf);
	inter_init(&spectrum);
	//inter_init(&spectrum_reflect);

	inter_import_array(&probe_mode,in->ymesh,in->ymesh,in->ymeshpoints,TRUE);
	long double val=0.0;
	long double mul=0.0;
	for (i=0;i<in->probe_modes.lpoints;i++)
	{

		light_get_mode(&probe_mode,i,&(in->probe_modes));
		val=probe_cal(sim,in,&probe_mode);
		//inter_append(&spectrum,1240.0/(in->probe_modes.l[i]*1e9),val);
		inter_append(&spectrum,in->probe_modes.l[i],val);
		//mul=inter_get_noend(&(reflect),in->probe_modes.l[i]);
		//val*=mul;
		//inter_append(&spectrum_reflect,1240.0/(in->probe_modes.l[i]*1e9),val);


	}


	if (first==FALSE)
	{
		inter_copy(&spectrum_first,&spectrum,TRUE);		
		//inter_copy(&spectrum_reflect_first,&spectrum,TRUE);		
	}

	first=TRUE;

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e9;
	strcpy(buf.title,_("Wavelength DR/R"));
	strcpy(buf.type,_("xy"));
	strcpy(buf.x_label,_("Wavelength"));
	strcpy(buf.y_label,_("DR/R"));
	strcpy(buf.x_units,_("nm"));
	strcpy(buf.y_units,_("au"));
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.time=in->time;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,spectrum.x, spectrum.data, spectrum.len);

	sprintf(full_name,"stark_spectrum_abs%d.dat",dump_number);
	buffer_dump_path(out_dir,full_name,&buf);
	buffer_free(&buf);

	inter_sub(&spectrum,&spectrum_first);


	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e9;
	strcpy(buf.title,_("Wavelength DR/R"));
	strcpy(buf.type,_("xy"));
	strcpy(buf.x_label,_("Wavelength"));
	strcpy(buf.y_label,_("DR/R"));
	strcpy(buf.x_units,_("nm"));
	strcpy(buf.y_units,_("au"));
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.time=in->time;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,spectrum.x, spectrum.data, spectrum.len);

	sprintf(full_name,"stark_spectrum%d.dat",dump_number);
	buffer_dump_path(out_dir,full_name,&buf);
	buffer_free(&buf);


	/*inter_sub(&spectrum_reflect,&spectrum_reflect_first);

	buffer_malloc(&buf);
	buf.y_mul=1.0;
	buf.x_mul=1e9;
	strcpy(buf.title,_("Wavelength DR/R"));
	strcpy(buf.type,_("xy"));
	strcpy(buf.x_label,_("Wavelength"));
	strcpy(buf.y_label,_("DR/R"));
	strcpy(buf.x_units,_("nm"));
	strcpy(buf.y_units,_("au"));
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.time=in->time;
	buffer_add_info(&buf);
	buffer_add_xy_data(&buf,spectrum_reflect.x, spectrum_reflect.data, spectrum_reflect.len);

	sprintf(full_name,"stark_spectrum_reflect%d.dat",dump_number);
	buffer_dump_path(out_dir,full_name,&buf);
	buffer_free(&buf);*/


	inter_free(&probe_mode);
	inter_free(&spectrum);
	//inter_free(&spectrum_reflect);

}

void probe_init(struct simulation *sim,struct device *in)
{
}

void probe_free(struct simulation *sim,struct device *in)
{
}

