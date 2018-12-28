//
//  General-purpose Photovoltaic Device Model gpvdm.com- a drift diffusion
//  base/Shockley-Read-Hall model for 1st, 2nd and 3rd generation solarcells.
//
//  Copyright (C) 2012 -2016 Roderick C. I. MacKenzie r.c.i.mackenzie at googlemail.com
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

/** @file dump.c
@brief go and dump stuff, what is dumped depends on which options have been set
*/

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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <inp.h>

static int unused __attribute__((unused));
static int dump_number;

void dump_clean_cache_files(struct simulation* sim)
{
struct inp_file inp;
char temp[200];
DIR *d;
struct dirent *dir;

	if (get_dump_status(sim,dump_remove_dos_cache)==TRUE)
	{
		d = opendir(".");
		if (d)
		{
			while ((dir = readdir(d)) != NULL)
			{
				//printf("%s\n",dir->d_name);
				if ((strcmp_end(dir->d_name,".inp.chk")==0)||(strcmp_end(dir->d_name,"_dosn.dat")==0)||(strcmp_end(dir->d_name,"_dosp.dat")==0))
				{
					remove(dir->d_name);
				}
			}

			closedir(d);
		}
	}


	inp_init(sim,&inp);
	if (inp_load(sim, &inp , "delete_files.inp")==0)
	{

		inp_reset_read(sim,&inp);
		strcpy(temp,inp_get_string(sim,&inp));
		if (strcmp(temp,"#begin")!=0)
		{
			return;
		}

		while(1)
		{
			strcpy(temp,inp_get_string(sim,&inp));
			if (strcmp(temp,"#end")==0)
			{
				break;
			}else
			{
				remove(temp);
			}

		}

		inp_free(sim,&inp);
	}



}

void dump_init(struct simulation *sim,struct device* in)
{
dump_number=0;
set_dump_status(sim,dump_lock, FALSE);
}

void buffer_add_3d_device_data(struct simulation *sim,struct buffer *buf,struct device *in,gdouble ***data)
{
int x=0;
int y=0;
int z=0;

gdouble xpos=0.0;
gdouble ypos=0.0;
gdouble zpos=0.0;

char string[200];
if (get_dump_status(sim,dump_write_headers)==TRUE)
{
	sprintf(string,"#data\n");
	buffer_add_string(buf,string);
}

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

if (get_dump_status(sim,dump_write_headers)==TRUE)
{
	sprintf(string,"#end\n");
	buffer_add_string(buf,string);
}

}

void buffer_add_3d_device_data_including_boundaries(struct simulation *sim,struct buffer *buf,struct device *in,gdouble ***data,long double left,long double right)
{
int x=0;
int y=0;
int z=0;

gdouble xpos=0.0;
gdouble ypos=0.0;
gdouble zpos=0.0;

char string[200];

if (get_dump_status(sim,dump_write_headers)==TRUE)
{
	sprintf(string,"#data\n");
	buffer_add_string(buf,string);
}

if ((in->xmeshpoints>1)&&(in->ymeshpoints>1)&&(in->zmeshpoints>1))
{
	for (z=0;z<in->zmeshpoints;z++)
	{
		for (x=0;x<in->xmeshpoints;x++)
		{
			sprintf(string,"%Le %Le\n",(long double)0.0,left);
			buffer_add_string(buf,string);

			for (y=0;y<in->ymeshpoints;y++)
			{
				sprintf(string,"%Le %Le %Le %Le\n",in->xmesh[x],in->ymesh[y],in->zmesh[z],data[z][x][y]);
				buffer_add_string(buf,string);
			}

			sprintf(string,"%Le %Le\n",in->ylen,right);
			buffer_add_string(buf,string);

		}
	}
}else
if ((in->xmeshpoints>1)&&(in->ymeshpoints>1))
{
	z=0;
	for (x=0;x<in->xmeshpoints;x++)
	{
		sprintf(string,"%Le %Le\n",(long double)0.0,left);
		buffer_add_string(buf,string);

		for (y=0;y<in->ymeshpoints;y++)
		{
			sprintf(string,"%Le %Le %Le\n",in->xmesh[x],in->ymesh[y],data[z][x][y]);
			buffer_add_string(buf,string);
		}

		sprintf(string,"%Le %Le\n",in->ylen,right);
		buffer_add_string(buf,string);

		buffer_add_string(buf,"\n");
	}
}else
{
	x=0;
	z=0;
	sprintf(string,"%Le %Le\n",(long double)0.0,left);
	buffer_add_string(buf,string);

	for (y=0;y<in->ymeshpoints;y++)
	{
		sprintf(string,"%Le %Le\n",in->ymesh[y],data[z][x][y]);
		buffer_add_string(buf,string);
	}

	sprintf(string,"%Le %Le\n",in->ylen,right);
	buffer_add_string(buf,string);

}

if (get_dump_status(sim,dump_write_headers)==TRUE)
{
	sprintf(string,"#end\n");
	buffer_add_string(buf,string);
}

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
struct buffer buf;
buffer_init(&buf);

strextract_name(sim_name,in->simmode);


int dumped=FALSE;
FILE* out;

	sprintf(postfix,"%d",dump_number);
	if ((get_dump_status(sim,dump_pl)==TRUE)||(get_dump_status(sim,dump_energy_slice_switch)==TRUE)||(get_dump_status(sim,dump_1d_slices)==TRUE)||(get_dump_status(sim,dump_optical_probe_spectrum)==TRUE))
	{
		dump_make_snapshot_dir(sim,out_dir ,in->time, get_equiv_V(sim,in), dump_number);
	}

	if (get_dump_status(sim,dump_optical_probe_spectrum)==TRUE)
	{
		dump_probe_spectrum(sim,in,out_dir,dump_number);
		dumped=TRUE;
	}
	if (get_dump_status(sim,dump_1d_slices)==TRUE)
	{
		dump_1d_slice(sim,in,out_dir);
		dump_device_map(sim,out_dir,in);
		dumped=TRUE;
	}

	if (get_dump_status(sim,dump_energy_slice_switch)==TRUE)
	{
		dump_energy_slice(sim,out_dir,in);
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

