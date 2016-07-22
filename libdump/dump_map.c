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


#include <stdio.h>
#include <string.h>
#include <dos.h>
#include "sim.h"
#include "dump.h"
#include "buffer.h"

static int unused __attribute__((unused));


void dump_device_map(char* out_dir,struct device *in)
{
struct buffer buf;
char name[200];
char temp[1000];
int i;
int band;
buffer_init(&buf);


buffer_malloc(&buf);
join_path(2,name,out_dir,"nt_map.dat");


buf.y_mul=1.0;
buf.x_mul=1e9;
strcpy(buf.title,"Charge carrier density - position");
strcpy(buf.type,"xy");
strcpy(buf.x_label,"Position");
strcpy(buf.y_label,"Carrier density");
strcpy(buf.x_units,"nm");
strcpy(buf.y_units,"m^{-3} eV^{-1}");
buf.logscale_x=0;
buf.logscale_y=0;
buffer_add_info(&buf);
for (i=0;i<in->ymeshpoints;i++)
{
	for (band=0;band<in->srh_bands;band++)
	{
		sprintf(temp,"%Le %Le %Le\n",in->ymesh[i],in->Ec[0][0][i]+dos_get_band_energy_n(in,band,in->imat[0][0][i]),in->nt[0][0][i][band]);
		buffer_add_string(&buf,temp);
	}
}
buffer_dump(name,&buf);
buffer_free(&buf);


buffer_malloc(&buf);

join_path(2,name,out_dir,"nt_map.dat");
buf.y_mul=1.0;
buf.x_mul=1e9;
strcpy(buf.title,"Charge carrier density - position");
strcpy(buf.type,"xy");
strcpy(buf.x_label,"Position");
strcpy(buf.y_label,"Carrier density");
strcpy(buf.x_units,"nm");
strcpy(buf.y_units,"m^{-3} eV^{-1}");
buf.logscale_x=0;
buf.logscale_y=0;
buffer_add_info(&buf);
for (i=0;i<in->ymeshpoints;i++)
{
	for (band=0;band<in->srh_bands;band++)
	{
		sprintf(temp,"%Le %Le %Le\n",in->ymesh[i],in->Ev[0][0][i]-dos_get_band_energy_p(in,band,in->imat[0][0][i]),in->pt[0][0][i][band]);
		buffer_add_string(&buf,temp);
	}
}
buffer_dump(name,&buf);
buffer_free(&buf);

}



