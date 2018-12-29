//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
// 
//  Copyright (C) 2012-2016 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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

/** @file fdtd_dump.c
	@brief Dumps the fdtd fields.
*/

#include <math.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <lang.h>
#include <cal_path.h>
#include <buffer.h>
#include <inp.h>
#include <sim.h>
#include <log.h>
#include <fdtd.h>
#include <dump.h>


#include "vec.h"

void fdtd_dump(struct simulation *sim,struct fdtd_data *data)
{
	int z=0;
	int x=0;
	int y=0;
	char temp[1000];
	struct buffer buf;
	FILE *out;
	buffer_init(&buf);
	char out_dir[PATH_MAX];

	//strcpy(out_dir,get_output_path(sim));

	dump_make_snapshot_dir(sim,out_dir ,(double)data->time, 0.0, data->step);

	//////Ex
	buffer_malloc(&buf);
	buf.z_mul=1e9;
	buf.x_mul=1e9;
	buf.y_mul=1e9;
	sprintf(buf.title,"%s ",_("Ex - Electric field"));
	strcpy(buf.type,"heat");
	strcpy(buf.x_label,_("X position"));
	strcpy(buf.y_label,_("Y position"));

	strcpy(buf.data_label,_("E-field"));
	strcpy(buf.x_units,_("nm"));
	strcpy(buf.y_units,_("nm"));


	strcpy(buf.data_units,"V/m");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=data->xlen;
	buf.y=data->ylen;
	buf.data_max=1.0;
	buf.data_min=-1.0;
	buf.z=1;
	buffer_add_info(sim,&buf);

	for (z=0;z<data->zlen;z++)
	{
		for (x=0;x<data->xlen;x++)
		{

			for (y=0;y<data->ylen;y++)
			{
				sprintf(temp,"%le %le %le\n",data->x_mesh[x],data->y_mesh[y],data->Ex[z][x][y]);
				buffer_add_string(&buf,temp);
			}

			buffer_add_string(&buf,"\n");

		}
	
	}

	buffer_dump_path(sim,get_output_path(sim),"Ex.dat",&buf);
	buffer_dump_path(sim,out_dir,"Ex.dat",&buf);
	buffer_free(&buf);

	//////Ey
	buffer_malloc(&buf);
	buf.z_mul=1e9;
	buf.x_mul=1e9;
	buf.y_mul=1e9;
	sprintf(buf.title,"%s ",_("Ey - Electric field"));
	strcpy(buf.type,"3d");
	strcpy(buf.x_label,_("X position"));
	strcpy(buf.y_label,_("Y position"));

	strcpy(buf.data_label,_("E-field"));
	strcpy(buf.x_units,_("nm"));
	strcpy(buf.y_units,_("nm"));


	strcpy(buf.data_units,"V/m");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=data->xlen;
	buf.y=data->ylen;
	buf.data_max=1.0;
	buf.data_min=-1.0;
	buf.z=1;
	buffer_add_info(sim,&buf);

	for (z=0;z<data->zlen;z++)
	{
		for (x=0;x<data->xlen;x++)
		{

			for (y=0;y<data->ylen;y++)
			{
				sprintf(temp,"%le %le %le\n",data->x_mesh[x],data->y_mesh[y],data->Ey[z][x][y]);
				buffer_add_string(&buf,temp);
			}

			buffer_add_string(&buf,"\n");

		}
	
	}

	buffer_dump_path(sim,get_output_path(sim),"Ey.dat",&buf);
	buffer_dump_path(sim,out_dir,"Ey.dat",&buf);
	buffer_free(&buf);


	//////Ez
	buffer_malloc(&buf);
	buf.z_mul=1e9;
	buf.x_mul=1e9;
	buf.y_mul=1e9;
	sprintf(buf.title,"%s ",_("Ez - Electric field"));
	strcpy(buf.type,"3d");
	strcpy(buf.x_label,_("X position"));
	strcpy(buf.y_label,_("Y position"));

	strcpy(buf.data_label,_("E-field"));
	strcpy(buf.x_units,_("nm"));
	strcpy(buf.y_units,_("nm"));


	strcpy(buf.data_units,"V/m");
	buf.logscale_x=0;
	buf.logscale_y=0;
	buf.x=data->xlen;
	buf.y=data->ylen;
	buf.data_max=1.0;
	buf.data_min=-1.0;
	buf.z=1;
	buffer_add_info(sim,&buf);

	for (z=0;z<data->zlen;z++)
	{
		for (x=0;x<data->xlen;x++)
		{

			for (y=0;y<data->ylen;y++)
			{
				sprintf(temp,"%le %le %le\n",data->x_mesh[x],data->y_mesh[y],data->Ez[z][x][y]);
				buffer_add_string(&buf,temp);
			}

			buffer_add_string(&buf,"\n");

		}
	
	}

	buffer_dump_path(sim,get_output_path(sim),"Ez.dat",&buf);
	buffer_dump_path(sim,out_dir,"Ez.dat",&buf);
	buffer_free(&buf);

	out=fopen("./epsilonr.dat","w");
	for (z=0;z<data->zlen;z++)
	{
		for (x=0;x<data->xlen;x++)
		{

			for (y=0;y<data->ylen;y++)
			{
				fprintf(out,"%le %le %le\n",data->x_mesh[x],data->y_mesh[y],data->epsilon_r[z][x][y]);
			}
		
			fprintf(out,"\n");
		}


	}

	fclose(out);

	out=fopen("./sigma.dat","w");
	for (z=0;z<data->zlen;z++)
	{
		for (x=0;x<data->xlen;x++)
		{

			for (y=0;y<data->ylen;y++)
			{
				fprintf(out,"%le %le %le\n",data->x_mesh[x],data->y_mesh[y],data->sigma[z][x][y]);
			}
		
			fprintf(out,"\n");
		}


	}

	fclose(out);
	
}
