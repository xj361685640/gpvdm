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



#include <stddef.h>
#include <lang.h>
#include <exp.h>
#include "sim.h"
#include "inp.h"
#include <cal_path.h>

static int unused __attribute__((unused));

static gdouble *sun;
static gdouble *voltage;
static gdouble *laser;
static gdouble *time_mesh;
static gdouble *fs_laser;
static int mesh_len=0;
static int use_mesh=FALSE;
static int mesh_pos=0;


static int enable_everything=FALSE;


void time_enable_everything(int in)
{

enable_everything=in;

}

void time_mesh_save(struct simulation *sim)
{

int i;
FILE *out;
out=fopen("mesh_save.dat","w");
if (out==NULL)
{
	ewe(sim,"can not save time mesh file\n");
}

fprintf(out, "%d\n",mesh_len);

for (i=0;i<mesh_len;i++)
{
	fprintf(out, "%Le %Le %Le %Le %Le\n",time_mesh[i],laser[i],sun[i],voltage[i],fs_laser[i]);
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

time_mesh=(gdouble *)malloc(buffer_len*sizeof(gdouble));
sun=(gdouble *)malloc(buffer_len*sizeof(gdouble));
voltage=(gdouble *)malloc(buffer_len*sizeof(gdouble));
laser=(gdouble *)malloc(buffer_len*sizeof(gdouble));
fs_laser=(gdouble *)malloc(buffer_len*sizeof(gdouble));

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
			time_mesh[ii]=time;
			//printf("%Le %d\n",time,ii);
			laser[ii]=read_laser;
			sun[ii]=read_sun+light_get_sun(&(in->mylight));
			voltage[ii]=v;
			fs_laser[ii]=0.0;
			time=time+dt;
			v=v+dv;


			if (fired==FALSE)
			{
				if ((time>fs_laser_time)&&(fs_laser_time!= -1.0))
				{
					fired=TRUE;
					fs_laser[ii]=laser_pulse_width/dt;
				}
			}

			ii++;
			dt=dt*mul;
		}
	}
}

mesh_len=ii;


in->time=time_mesh[0];
in->dt=time_mesh[1]-time_mesh[0];

inp_free(sim,&inp);

use_mesh=TRUE;

}


void time_init(struct device *in)
{

int i;
int band;

for (i=0;i<in->ymeshpoints;i++)
{

	for (band=0;band<in->srh_bands;band++)
	{
		in->ntlast[i][band]=in->nt[i][band];
		in->ptlast[i][band]=in->pt[i][band];
	}

in->nlast[i]=in->n[i];
in->plast[i]=in->p[i];

}

in->time=0.0;
}

void device_timestep(struct simulation *sim,struct device *in)
{

int i;
int band;

for (i=0;i<in->ymeshpoints;i++)
{
	in->nlast[i]=in->n[i];
	in->plast[i]=in->p[i];

	for (band=0;band<in->srh_bands;band++)
	{
		in->ntlast[i][band]=in->nt[i][band];
		in->ptlast[i][band]=in->pt[i][band];
	}
}
in->Vapplied_last=in->Vapplied;
in->VCext_last=in->VCext;
in->Ilast=get_I(in);

if (use_mesh==TRUE)
{
	if (mesh_pos<(mesh_len-1))
	{
		mesh_pos++;
		in->time=time_mesh[mesh_pos];
		if (mesh_pos==(mesh_len-1))
		{
			in->dt=(time_mesh[mesh_pos]-time_mesh[mesh_pos-1]);
		}else
		{
			in->dt=(time_mesh[mesh_pos+1]-time_mesh[mesh_pos]);
		}

	}
}else
{
	in->time+=in->dt;
}

}

int time_test_last_point()
{
	if (mesh_pos<(mesh_len-1))
	{
		return FALSE;
	}else
	{
		return TRUE;
	}
}

gdouble time_get_voltage()
{
	return voltage[mesh_pos];
}

gdouble time_get_fs_laser()
{

	return fs_laser[mesh_pos];
}

gdouble time_get_sun()
{
	return sun[mesh_pos];
}

gdouble time_get_laser()
{
	return laser[mesh_pos];
}

void time_memory_free()
{

	free(time_mesh);
	free(sun);
	free(voltage);
	free(laser);
	free(fs_laser);
}

