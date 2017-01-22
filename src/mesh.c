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


#include "device.h"
#include "mesh.h"
#include "inp.h"
#include "util.h"
#include "const.h"
#include "hard_limit.h"
#include <log.h>
#include <cal_path.h>


void mesh_check_y(struct simulation *sim,struct device *in)
{
int y=0;
gdouble mesh_len=0.0;

	for (y=0;y<in->ymeshlayers;y++)
	{
		mesh_len+=in->meshdata_y[y].len;
	}

	if (fabs(in->ylen-mesh_len)>1e-14)
	{
		printf("calling remesh\n");
		//getchar();
		mesh_remesh_y(sim,in);
		printf_log(sim,"Warning: Length of epitaxy and computational mesh did not match, so I remesshed the device.\n");
	}
}

void mesh_remesh_y(struct simulation *sim,struct device *in)
{
	char device_file_path[1000];
	in->ymeshlayers=1;
	in->meshdata_y[0].len=in->ylen;
	if (in->my_epitaxy.electrical_layers==1)
	{
		in->meshdata_y[0].number=10;
	}else
	{
		in->meshdata_y[0].number=40;
	}		
	mesh_save_y(sim,in);
	device_free(sim,in);
	mesh_free(sim,in);

	join_path(2,device_file_path,get_input_path(sim),"epitaxy.inp");
	epitaxy_load(sim,&(in->my_epitaxy),device_file_path);

	mesh_load(sim,in);
	device_get_memory(sim,in);
	mesh_build(sim,in);


}

void mesh_save_y(struct simulation *sim,struct device *in)
{
	int i=0;
	char buffer[2000];
	char temp[2000];
	char full_file_name[200];

	strcpy(buffer,"");
	strcat(buffer,"#mesh_layers\n");

	sprintf(temp,"%d\n",in->ymeshlayers);
	strcat(buffer,temp);

	for (i=0;i<in->ymeshlayers;i++)
	{
		strcat(buffer,"#mesh_layer_length0\n");

		sprintf(temp,"%Le\n",in->meshdata_y[i].len);
		strcat(buffer,temp);

		strcat(buffer,"#mesh_layer_points0\n");

		sprintf(temp,"%d\n",(int)(in->meshdata_y[i].number));
		strcat(buffer,temp);
	}

	strcat(buffer,"#ver\n");
	strcat(buffer,"1.0\n");
	strcat(buffer,"#end\n");

	join_path(2,full_file_name,get_input_path(sim),"mesh_y.inp");
	printf("Write new mesh to: %s\n",full_file_name);
	zip_write_buffer(sim,full_file_name,buffer, strlen(buffer));

}

void mesh_free(struct simulation *sim,struct device *in)
{
	in->zmeshpoints=0;
	free(in->meshdata_z);

	in->xmeshpoints=0;
	free(in->meshdata_x);

	in->ymeshpoints=0;
	free(in->meshdata_y);
}

void mesh_load(struct simulation *sim,struct device *in)
{
	int i;
	struct inp_file inp;
	char token0[200];
	char token1[200];


	//z
	inp_init(sim,&inp);
	inp_load_from_path(sim,&inp,get_input_path(sim),"mesh_z.inp");
	inp_check(sim,&inp,1.0);
	inp_reset_read(sim,&inp);
	inp_get_string(sim,&inp);//"#mesh_layers"
	sscanf(inp_get_string(sim,&inp),"%d",&(in->zmeshlayers));

	in->meshdata_z = malloc (in->zmeshlayers * sizeof(struct mesh));
	in->zmeshpoints=0;


	for (i=0;i<in->zmeshlayers;i++)
	{
		sscanf(inp_get_string(sim,&inp),"%s",token0);
		sscanf(inp_get_string(sim,&inp),"%Lf",&(in->meshdata_z[i].len));

		sscanf(inp_get_string(sim,&inp),"%s",token1);
		sscanf(inp_get_string(sim,&inp),"%Lf",&(in->meshdata_z[i].number));

		in->meshdata_z[i].len=fabs(in->meshdata_z[i].len);
		hard_limit(sim,token0,&(in->meshdata_z[i].len));
		in->meshdata_z[i].den=in->meshdata_z[i].len/in->meshdata_z[i].number;
		in->zmeshpoints+=in->meshdata_z[i].number;
	}

	inp_free(sim,&inp);

	//x
	inp_init(sim,&inp);
	inp_load_from_path(sim,&inp,get_input_path(sim),"mesh_x.inp");
	inp_check(sim,&inp,1.0);
	inp_reset_read(sim,&inp);
	inp_get_string(sim,&inp);//"#mesh_layers"
	sscanf(inp_get_string(sim,&inp),"%d",&(in->xmeshlayers));

	in->meshdata_x = malloc (in->xmeshlayers * sizeof(struct mesh));
	in->xmeshpoints=0;


	for (i=0;i<in->xmeshlayers;i++)
	{
		sscanf(inp_get_string(sim,&inp),"%s",token0);
		sscanf(inp_get_string(sim,&inp),"%Lf",&(in->meshdata_x[i].len));

		sscanf(inp_get_string(sim,&inp),"%s",token1);
		sscanf(inp_get_string(sim,&inp),"%Lf",&(in->meshdata_x[i].number));

		in->meshdata_x[i].len=fabs(in->meshdata_x[i].len);
		hard_limit(sim,token0,&(in->meshdata_x[i].len));
		in->meshdata_x[i].den=in->meshdata_x[i].len/in->meshdata_x[i].number;
		in->xmeshpoints+=in->meshdata_x[i].number;
	}

	inp_free(sim,&inp);

	//y
	inp_init(sim,&inp);
	inp_load_from_path(sim,&inp,get_input_path(sim),"mesh_y.inp");
	inp_check(sim,&inp,1.0);
	inp_reset_read(sim,&inp);
	inp_get_string(sim,&inp);//"#mesh_layers"
	sscanf(inp_get_string(sim,&inp),"%d",&(in->ymeshlayers));

	in->meshdata_y = malloc (in->ymeshlayers * sizeof(struct mesh));
	in->ymeshpoints=0;


	for (i=0;i<in->ymeshlayers;i++)
	{
		sscanf(inp_get_string(sim,&inp),"%s",token0);
		sscanf(inp_get_string(sim,&inp),"%Lf",&(in->meshdata_y[i].len));

		sscanf(inp_get_string(sim,&inp),"%s",token1);
		sscanf(inp_get_string(sim,&inp),"%Lf",&(in->meshdata_y[i].number));

		in->meshdata_y[i].len=fabs(in->meshdata_y[i].len);
		hard_limit(sim,token0,&(in->meshdata_y[i].len));
		in->meshdata_y[i].den=in->meshdata_y[i].len/in->meshdata_y[i].number;
		//printf("realloc den\n");
		in->ymeshpoints+=in->meshdata_y[i].number;
	}

	inp_free(sim,&inp);


}

void mesh_build(struct simulation *sim,struct device *in)
{

	int pos=0;
	int i=0;
	int ii=0;
	int z=0;
	int x=0;
	gdouble dpos=0.0;

	//z
	pos=0;
	dpos=0.0;
	for (i=0;i<in->zmeshlayers;i++)
	{

		for (ii=0;ii<in->meshdata_z[i].number;ii++)
		{
			dpos+=in->meshdata_z[i].den/2.0;
			in->ymesh[pos]=dpos;
			dpos+=in->meshdata_z[i].den/2.0;
			pos++;
		}
	}

	in->zlen=dpos;

	//x
	pos=0;
	dpos=0.0;
	for (i=0;i<in->xmeshlayers;i++)
	{

		for (ii=0;ii<in->meshdata_x[i].number;ii++)
		{
			dpos+=in->meshdata_x[i].den/2.0;
			in->xmesh[pos]=dpos;
			dpos+=in->meshdata_x[i].den/2.0;
			pos++;
		}
	}

	in->xlen=dpos;


	//y
	pos=0;
	dpos=0.0;
	for (i=0;i<in->ymeshlayers;i++)
	{

		for (ii=0;ii<in->meshdata_y[i].number;ii++)
		{
			dpos+=in->meshdata_y[i].den/2.0;
			in->ymesh[pos]=dpos;
			in->imat[0][0][pos]=epitaxy_get_electrical_material_layer(&(in->my_epitaxy),dpos);
			in->imat_epitaxy[0][0][pos]=epitaxy_get_epitaxy_layer_using_electrical_pos(&(in->my_epitaxy),dpos);

			//printf("%s\n",sim->output_path);
			//printf("here %d %d %Le %Le %Le %Le\n",in->imat[0][0][pos],pos,dpos,in->meshdata_y[i].den,in->meshdata_y[i].len,in->meshdata_y[i].number);
			//getchar();
			for (z=0;z<in->zmeshlayers;z++)
			{
				for (x=0;x<in->xmeshlayers;x++)
				{
					//printf("%ld %ld %d\n",z,x,pos);
					in->imat[z][x][pos]=in->imat[0][0][pos];
				}
			}
			dpos+=in->meshdata_y[i].den/2.0;
			pos++;
		}
	}



}
void mesh_cal_layer_widths(struct device *in)
{
int i;
int cur_i=in->imat[0][0][0];

in->layer_start[cur_i]=0.0;

for (i=0;i<in->ymeshpoints;i++)
{
	if ((in->imat[0][0][i]!=cur_i)||(i==(in->ymeshpoints-1)))
	{
		in->layer_stop[cur_i]=in->ymesh[i-1];//+(in->ymesh[i]-in->ymesh[i-1])/2;
		if (i==(in->ymeshpoints-1))
		{
			break;
		}
		cur_i=in->imat[0][0][i];
		in->layer_start[cur_i]=in->ymesh[i];//-(in->ymesh[i]-in->ymesh[i-1])/2;
	}
//printf("%d\n",in->imat[i]);
}
}
