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


#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <exp.h>
#include "sim.h"
#include "dump.h"
#include <cal_path.h>
#include <pl.h>
#include <probe.h>
#include <string.h>



static int unused __attribute__((unused));
static int dump_number;
void dump_init(struct simulation *sim,struct device* in)
{
dump_number=0;
set_dump_status(sim,dump_lock, FALSE);
}

void buffer_add_3d_device_data(struct buffer *buf,struct device *in,gdouble ***data)
{
int x=0;
int y=0;
int z=0;

gdouble xpos=0.0;
gdouble ypos=0.0;
gdouble zpos=0.0;

char string[200];

sprintf(string,"#data\n");
buffer_add_string(buf,string);
			
if ((in->xmeshpoints>1)&&(in->ymeshpoints>1)&&(in->zmeshpoints>1))
{
	for (z=0;z<in->zmeshpoints;z++)
	{
		for (x=0;x<in->xmeshpoints;x++)
		{
			for (y=0;y<in->ymeshpoints;y++)
			{
				sprintf(string,"%Le %Le %Le %Le\n",in->xmesh[x],in->ymesh[y],in->zmesh[z],data[z][x][y]);
				buffer_add_string(buf,string);
			}
		}
	}
}else
if ((in->xmeshpoints>1)&&(in->ymeshpoints>1))
{
	z=0;
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			sprintf(string,"%Le %Le %Le\n",in->xmesh[x],in->ymesh[y],data[z][x][y]);
			buffer_add_string(buf,string);
		}
		buffer_add_string(buf,"\n");
	}
}else
{
	x=0;
	z=0;
	for (y=0;y<in->ymeshpoints;y++)
	{
		sprintf(string,"%Le %Le\n",in->ymesh[y],data[z][x][y]);
		buffer_add_string(buf,string);
	}
}

sprintf(string,"#end\n");
buffer_add_string(buf,string);
}

void buffer_set_graph_type(struct buffer *buf,struct device *in)
{
	if ((in->xmeshpoints>1)&&(in->ymeshpoints>1)&&(in->zmeshpoints>1))
	{
		strcpy(buf->type,"4d");
	}else
	if ((in->xmeshpoints>1)&&(in->ymeshpoints>1))
	{
		strcpy(buf->type,"3d");
	}else
	{
		strcpy(buf->type,"xy");
	}
}

void dump_write_to_disk(struct simulation *sim,struct device* in)
{
char temp[200];
char postfix[100];
char snapshots_dir[PATH_MAX];
char out_dir[PATH_MAX];
char slice_info_file[PATH_MAX];
char snapshot_dir[PATH_MAX];
char sim_name[PATH_MAX];

strextract_name(sim_name,in->simmode);

sprintf(snapshot_dir,"snapshots");

int dumped=FALSE;
FILE* out;
struct stat st = {0};

	sprintf(postfix,"%d",dump_number);
	if ((get_dump_status(sim,dump_pl)==TRUE)||(get_dump_status(sim,dump_energy_slice_switch)==TRUE)||(get_dump_status(sim,dump_1d_slices)==TRUE)||(get_dump_status(sim,dump_optical_probe_spectrum)==TRUE))
	{
		join_path(2,snapshots_dir,get_output_path(sim),snapshot_dir);

		if (stat(snapshots_dir, &st) == -1)
		{
				mkdir(snapshots_dir, 0700);
			
		}

		join_path(2,temp,snapshots_dir,"snapshots.inp");
		out=fopen(temp,"w");
		fprintf(out,"#end");
		fclose(out);

		join_path(2,out_dir,snapshots_dir,postfix);

		if (stat(out_dir, &st) == -1)
		{
			mkdir(out_dir, 0700);
		}

		join_path(2,slice_info_file,out_dir,"snapshot_info.dat");

		out=fopen(slice_info_file,"w");
		if (out!=NULL)
		{
			fprintf(out,"#dump_voltage\n");
			fprintf(out,"%Lf\n",get_equiv_V(sim,in));
			fprintf(out,"#dump_time\n");
			fprintf(out,"%Lf\n",in->time);
			fprintf(out,"#ver\n");
			fprintf(out,"1.0\n");
			fprintf(out,"#end\n");
			fclose(out);
		}else
		{
			ewe(sim,"Can't write to file %s\n",slice_info_file);
		}

	}

	if (get_dump_status(sim,dump_optical_probe_spectrum)==TRUE)
	{
		dump_probe_spectrum(sim,in,out_dir,dump_number);
		dumped=TRUE;
	}
	if (get_dump_status(sim,dump_1d_slices)==TRUE)
	{
		dump_1d_slice(sim,in,out_dir);
		dump_device_map(out_dir,in);
		dumped=TRUE;
	}

	if (get_dump_status(sim,dump_energy_slice_switch)==TRUE)
	{
		dump_energy_slice(out_dir,in);
		dumped=TRUE;
	}

	if (get_dump_status(sim,dump_pl)==TRUE)
	{
		exp_cal_emission(sim,dump_number,in);
		dumped=TRUE;
	}


	if (dumped==TRUE)
	{
		dump_number++;
	}


}

