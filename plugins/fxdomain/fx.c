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


#include "sim.h"
#include "inp.h"
#include "fx.h"
#include <cal_path.h>

static int unused __attribute__((unused));

static long double *fx_mesh;
static int mesh_len=0;
static int mesh_pos=0;

void fx_mesh_save(struct simulation *sim)
{
	int i;
	FILE *out;
	out=fopen("fxmesh_save.dat","w");

	if (out==NULL)
	{
		ewe(sim,"can not save fx mesh file\n");
	}

	fprintf(out, "%d\n",mesh_len);

	for (i=0;i<mesh_len;i++)
	{
		fprintf(out, "%Le\n",fx_mesh[i]);
	}

	fprintf(out, "#ver\n");
	fprintf(out, "1.0\n");
	fprintf(out, "#end\n");

	fclose(out);
}

void fx_load_mesh(struct simulation *sim,struct device *in,int number)
{
	struct inp_file inp;
	char mesh_file[200];

	long double start=0.0;
	long double stop=0.0;
	long double points=0.0;
	long double mul=0.0;

	long double dfx=0.0;
	
	int buffer_len=2000;
	long double fx=0.0;
	int ii=0;
	char temp[200];
	fx_mesh=(long double *)malloc(buffer_len*sizeof(long double));

	sprintf(mesh_file,"fxmesh%d.inp",number);

	inp_init(sim,&inp);
	inp_load_from_path(sim,&inp,get_input_path(sim),mesh_file);
	inp_check(sim,&inp,1.1);

	inp_reset_read(sim,&inp);

	while(1)
	{
		strcpy(temp,inp_get_string(sim,&inp));		//token
		if (strcmp(temp,"#ver")==0)
		{
			break;
		}
		sscanf(inp_get_string(sim,&inp),"%Le",&start);

		inp_get_string(sim,&inp);
		sscanf(inp_get_string(sim,&inp),"%Le",&stop);

		inp_get_string(sim,&inp);
		sscanf(inp_get_string(sim,&inp),"%Le",&points);

		inp_get_string(sim,&inp);
		sscanf(inp_get_string(sim,&inp),"%Le",&mul);
		
		dfx=(stop-start)/points;

		if ((dfx!=0.0)&&(mul!=0.0))
		{

			fx=start;

			while(fx<stop)
			{
				fx_mesh[ii]=fx;
				fx=fx+dfx;
				dfx=dfx*mul;
				ii++;
			}
		}
	}

	mesh_len=ii;
	mesh_pos=0;
	inp_free(sim,&inp);

}

void fx_step()
{
mesh_pos++;
}

int fx_points()
{
	return mesh_len;
}

int fx_run()
{
	if (mesh_pos<mesh_len)
	{
		return TRUE;
	}else
	{
		return FALSE;
	}
}

long double fx_get_fx()
{
	return fx_mesh[mesh_pos];
}

void fx_memory_free()
{
	free(fx_mesh);
}

