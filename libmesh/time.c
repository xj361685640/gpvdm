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

/** @file time.c
	@brief Meshing in time domain.
*/


#include <stddef.h>
#include <lang.h>
#include <exp.h>
#include "sim.h"
#include "inp.h"
#include <cal_path.h>
#include <contacts.h>
#include <log.h>


static int unused __attribute__((unused));



void time_mesh_save(struct simulation *sim,struct device *in)
{

int i;
FILE *out;
out=fopen("mesh_save.dat","w");
if (out==NULL)
{
	ewe(sim,"can not save time mesh file\n");
}

fprintf(out, "%d\n",in->tm_mesh_len);

for (i=0;i<in->tm_mesh_len;i++)
{
	fprintf(out, "%Le %Le %Le %Le %Le\n",in->tm_time_mesh[i],in->tm_laser[i],in->tm_sun[i],in->tm_voltage[i],in->tm_fs_laser[i]);
}
fprintf(out, "#ver\n");
fprintf(out, "1.0\n");
fprintf(out, "#end\n");

fclose(out);

}

void time_load_mesh(struct simulation *sim,struct device *in,int number)
{

int i;
struct inp_file inp;
char mesh_file[200];
gdouble start_time=0.0;
gdouble fs_laser_time=0.0;
int segments=0;
gdouble read_len=0.0;
gdouble dt=0.0;
gdouble v_start=0.0;
gdouble v_stop=0.0;
gdouble mul=0.0;
gdouble read_sun=0.0;
gdouble read_laser=0.0;
gdouble laser_pulse_width=0.0;
int buffer_len=2000;
gdouble time=0.0;
int ii=0;
gdouble end_time=0;
int fired=FALSE;
gdouble v=0.0;
gdouble dv=0.0;

in->tm_time_mesh=(gdouble *)malloc(buffer_len*sizeof(gdouble));
in->tm_sun=(gdouble *)malloc(buffer_len*sizeof(gdouble));
in->tm_voltage=(gdouble *)malloc(buffer_len*sizeof(gdouble));
in->tm_laser=(gdouble *)malloc(buffer_len*sizeof(gdouble));
in->tm_fs_laser=(gdouble *)malloc(buffer_len*sizeof(gdouble));

in->tm_mesh_pos=0;

if (in->mylight.pulse_width==-1)
{
	ewe(sim,_("You must load the optical plugin before making the time mesh"));
}else
{
laser_pulse_width=in->mylight.pulse_width;
}

sprintf(mesh_file,"time_mesh_config%d.inp",number);

inp_init(sim,&inp);
inp_load_from_path(sim,&inp,get_input_path(sim),mesh_file);
inp_check(sim,&inp,1.1);

inp_reset_read(sim,&inp);

inp_get_string(sim,&inp);
sscanf(inp_get_string(sim,&inp),"%Le",&start_time);

inp_get_string(sim,&inp);
sscanf(inp_get_string(sim,&inp),"%Le",&fs_laser_time);

inp_get_string(sim,&inp);
sscanf(inp_get_string(sim,&inp),"%d",&segments);
time=start_time;


for (i=0;i<segments;i++)
{
	inp_get_string(sim,&inp);
	sscanf(inp_get_string(sim,&inp),"%Le",&read_len);

	inp_get_string(sim,&inp);
	sscanf(inp_get_string(sim,&inp),"%Le",&dt);

	inp_get_string(sim,&inp);
	sscanf(inp_get_string(sim,&inp),"%Le",&v_start);

	inp_get_string(sim,&inp);
	sscanf(inp_get_string(sim,&inp),"%Le",&v_stop);

	inp_get_string(sim,&inp);
	sscanf(inp_get_string(sim,&inp),"%Le",&mul);

	inp_get_string(sim,&inp);
	sscanf(inp_get_string(sim,&inp),"%Le",&read_sun);

	inp_get_string(sim,&inp);
	sscanf(inp_get_string(sim,&inp),"%Le",&read_laser);

	if ((dt!=0.0)&&(mul!=0.0))
	{
		v=v_start;
		end_time=time+read_len;


		while(time<end_time)
		{
			dv=(v_stop-v_start)*dt/read_len;
			in->tm_time_mesh[ii]=time;

			in->tm_laser[ii]=read_laser;
			in->tm_sun[ii]=read_sun+light_get_sun(&(in->mylight));
			in->tm_voltage[ii]=v;
			in->tm_fs_laser[ii]=0.0;
			time=time+dt;
			v=v+dv;


			if (fired==FALSE)
			{
				if ((time>fs_laser_time)&&(fs_laser_time!= -1.0))
				{
					fired=TRUE;
					in->tm_fs_laser[ii]=laser_pulse_width/dt;
				}
			}

			ii++;
			dt=dt*mul;
		}
	}
}

in->tm_mesh_len=ii;


in->time=in->tm_time_mesh[0];
in->dt=in->tm_time_mesh[1]-in->tm_time_mesh[0];

inp_free(sim,&inp);

in->tm_use_mesh=TRUE;

}


void time_init(struct simulation *sim,struct device *in)
{

time_store(sim,in);

in->time=0.0;
}

void time_store(struct simulation *sim,struct device *in)
{
int x;
int y;
int z;
int band;

for (z=0;z<in->zmeshpoints;z++)
{
	for (x=0;x<in->xmeshpoints;x++)
	{
		for (y=0;y<in->ymeshpoints;y++)
		{
			in->nlast[z][x][y]=in->n[z][x][y];
			in->plast[z][x][y]=in->p[z][x][y];

			for (band=0;band<in->srh_bands;band++)
			{
				in->ntlast[z][x][y][band]=in->nt[z][x][y][band];
				in->ptlast[z][x][y][band]=in->pt[z][x][y][band];
			}
		}
	}
}

contacts_time_step(sim,in);
in->VCext_last=in->VCext;
in->Ilast=get_I(in);
}

void device_timestep(struct simulation *sim,struct device *in)
{
time_store(sim,in);
if (in->go_time==TRUE)
{
	if (in->tm_use_mesh==TRUE)
	{
		if (in->tm_mesh_pos<(in->tm_mesh_len-1))
		{
			in->tm_mesh_pos++;
			in->time=in->tm_time_mesh[in->tm_mesh_pos];
			if (in->tm_mesh_pos==(in->tm_mesh_len-1))
			{
				in->dt=(in->tm_time_mesh[in->tm_mesh_pos]-in->tm_time_mesh[in->tm_mesh_pos-1]);
			}else
			{
				in->dt=(in->tm_time_mesh[in->tm_mesh_pos+1]-in->tm_time_mesh[in->tm_mesh_pos]);
			}

		}
	}else
	{
		in->time+=in->dt;
	}
}
}

int time_test_last_point(struct device *in)
{
	if (in->tm_mesh_pos<(in->tm_mesh_len-1))
	{
		return FALSE;
	}else
	{
		return TRUE;
	}
}

gdouble time_get_voltage(struct device *in)
{
	return in->tm_voltage[in->tm_mesh_pos];
}

gdouble time_get_fs_laser(struct device *in)
{

	return in->tm_fs_laser[in->tm_mesh_pos];
}

gdouble time_get_sun(struct device *in)
{
	return in->tm_sun[in->tm_mesh_pos];
}

gdouble time_get_laser(struct device *in)
{
	return in->tm_laser[in->tm_mesh_pos];
}

void time_memory_free(struct device *in)
{

	free(in->tm_time_mesh);
	free(in->tm_sun);
	free(in->tm_voltage);
	free(in->tm_laser);
	free(in->tm_fs_laser);
	in->tm_time_mesh=NULL;
	in->tm_sun=NULL;
	in->tm_voltage=NULL;
	in->tm_laser=NULL;
	in->tm_fs_laser=NULL;
	in->tm_mesh_len=-1;
	in->tm_use_mesh=-1;
	in->tm_mesh_pos=-1;
}

