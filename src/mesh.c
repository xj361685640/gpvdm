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
#include <lang.h>

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
		printf_log(sim,"calling remesh\n");
		//getchar();
		mesh_remesh_y(sim,in);
		printf_log(sim,"Warning: Length of epitaxy and computational mesh did not match, so I remesshed the device.\n");
	}
}

void mesh_remesh_y(struct simulation *sim,struct device *in)
{
	char device_file_path[1000];
	if (in->remesh_y==TRUE)
	{
		in->ymeshlayers=1;
		in->meshdata_y[0].len=in->ylen;
		if (in->my_epitaxy.electrical_layers==1)
		{
			in->meshdata_y[0].n_points=10;
		}else
		{
			in->meshdata_y[0].n_points=40;
		}
		in->meshdata_y[0].mul=1.0;
		in->meshdata_y[0].left_right=FALSE;
		mesh_save_y(sim,in);
		device_free(sim,in);
		mesh_free(sim,in);

		join_path(2,device_file_path,get_input_path(sim),"epitaxy.inp");
		epitaxy_load(sim,&(in->my_epitaxy),device_file_path);

		mesh_load(sim,in);
		device_get_memory(sim,in);
		mesh_build(sim,in);
	}else
	{
		ewe(sim,"%s\n",_("The mesh does not match the device length and I am not alowed to remesh it"));
	}


}

void mesh_save_y(struct simulation *sim,struct device *in)
{
	int i=0;
	char buffer[2000];
	char temp[2000];
	char full_file_name[200];

	strcpy(buffer,"");
	strcat(buffer,"#remesh_enable\n");
	strcat(buffer,"True\n");
	strcat(buffer,"#mesh_layers\n");

	sprintf(temp,"%d\n",in->ymeshlayers);
	strcat(buffer,temp);

	for (i=0;i<in->ymeshlayers;i++)
	{
		strcat(buffer,"#mesh_layer_length0\n");

		sprintf(temp,"%Le\n",in->meshdata_y[i].len);
		strcat(buffer,temp);

		strcat(buffer,"#mesh_layer_points0\n");

		sprintf(temp,"%d\n",(int)(in->meshdata_y[i].n_points));
		strcat(buffer,temp);
		
		strcat(buffer,"#mesh_layer_mul0\n");

		sprintf(temp,"%d\n",(int)(in->meshdata_y[i].mul));
		strcat(buffer,temp);
		
		strcat(buffer,"#mesh_layer_left_right0\n");

		sprintf(temp,"%d\n",(int)(in->meshdata_y[i].left_right));
		strcat(buffer,temp);
	}

	strcat(buffer,"#ver\n");
	strcat(buffer,"1.0\n");
	strcat(buffer,"#end\n");

	join_path(2,full_file_name,get_input_path(sim),"mesh_y.inp");
	printf_log(sim,"Write new mesh to: %s\n",full_file_name);
	zip_write_buffer(sim,full_file_name,buffer, strlen(buffer));

}

void mesh_free(struct simulation *sim,struct device *in)
{
	int i=0;
	for (i=0;i<in->zmeshlayers;i++)
	{
		free(in->meshdata_z[i].dmesh);
	}
	in->zmeshpoints=0;
	free(in->meshdata_z);

	for (i=0;i<in->xmeshlayers;i++)
	{
		free(in->meshdata_x[i].dmesh);
	}
	in->xmeshpoints=0;
	free(in->meshdata_x);

	for (i=0;i<in->ymeshlayers;i++)
	{
		free(in->meshdata_y[i].dmesh);
	}
	in->ymeshpoints=0;
	free(in->meshdata_y);
}

void mesh_load_file(struct simulation * sim,int *meshpoints,int *remesh, int *meshlayers, struct mesh **meshdata,char *file)
{
	int i;
	struct inp_file inp;
	char token0[200];
	char token1[200];
	char val[200];
	long double dx=0.0;
	int points=0;
	long double pos=0.0;

	inp_init(sim,&inp);
	inp_load_from_path(sim,&inp,get_input_path(sim),file);
	inp_check(sim,&inp,1.0);
	inp_reset_read(sim,&inp);

	inp_get_string(sim,&inp);			//remesh
	strcpy(val,inp_get_string(sim,&inp));
	(*remesh)=english_to_bin(sim,val);

	inp_get_string(sim,&inp);			//layers
	sscanf(inp_get_string(sim,&inp),"%d",meshlayers);

	(*meshdata) = malloc (*meshlayers * sizeof(struct mesh));

	for (i=0;i<*meshlayers;i++)
	{
		sscanf(inp_get_string(sim,&inp),"%s",token0);
		sscanf(inp_get_string(sim,&inp),"%Lf",&((*meshdata)[i].len));

		sscanf(inp_get_string(sim,&inp),"%s",token1);
		sscanf(inp_get_string(sim,&inp),"%ld",&((*meshdata)[i].n_points));
		(*meshdata)[i].dx=(*meshdata)[i].len/((long double)(*meshdata)[i].n_points);

		sscanf(inp_get_string(sim,&inp),"%s",token1);
		sscanf(inp_get_string(sim,&inp),"%Lf",&((*meshdata)[i].mul));

		sscanf(inp_get_string(sim,&inp),"%s",token1);
		sscanf(inp_get_string(sim,&inp),"%s",val);
		(*meshdata)[i].left_right=english_to_bin(sim,val);

		(*meshdata)[i].len=fabs((*meshdata)[i].len);
		hard_limit(sim,token0,&((*meshdata)[i].len));

	}

	inp_free(sim,&inp);

	for (i=0;i<*meshlayers;i++)
	{
		dx=(*meshdata)[i].len/((long double)(*meshdata)[i].n_points);
		pos=0.0;
		(*meshdata)[i].n_points=0;
		while(pos<(*meshdata)[i].len)
		{
			pos+=dx/2.0;
			//printf("%Le %Le\n",pos,(*meshdata)[i].len);
			//getchar();
			if (pos>(*meshdata)[i].len)
			{
				break;
			}

			(*meshdata)[i].n_points++;
			points++;
			pos+=dx/2.0;
			dx=dx*(*meshdata)[i].mul;
		}

		(*meshdata)[i].dmesh=malloc ( (*meshdata)[i].n_points * sizeof(long double));

	}

	(*meshpoints)=points;
}

void mesh_build_from_submesh(struct simulation *sim,struct device *in,char direction)
{
	int pos=0;
	int i=0;
	int ii=0;
	int z=0;
	int x=0;
	gdouble dpos=0.0;
	long double dx=0.0;
	long double len=0.0;

	int meshlayers=0;
	struct mesh *meshdata=NULL;
	long double *ret_len=NULL;
	long double *mesh=NULL;
	long double *dmesh=NULL;
	int meshpoints=0;

	if (direction=='x')
	{
		meshlayers=in->xmeshlayers;
		meshdata=in->meshdata_x;
		ret_len=&(in->xlen);
		mesh=in->xmesh;
		dmesh=in->dxmesh;
		meshpoints=in->xmeshpoints;
	}else
	if (direction=='y')
	{
		meshlayers=in->ymeshlayers;
		meshdata=in->meshdata_y;
		ret_len=&(in->ylen);
		mesh=in->ymesh;
		dmesh=in->dymesh;
		meshpoints=in->ymeshpoints;
	}else
	if (direction=='z')
	{
		meshlayers=in->zmeshlayers;
		meshdata=in->meshdata_z;
		ret_len=&(in->zlen);
		mesh=in->zmesh;
		dmesh=in->dzmesh;
		meshpoints=in->zmeshpoints;
	}else
	{
		ewe(sim,"%s",_("Direction wrong"));
	}

	len=0.0;
	for (i=0;i<meshlayers;i++)
	{
		pos=0;
		dpos=0.0;
		dx=meshdata[i].dx;
		//printf("going to build %ld\n",meshdata[i].n_points);
		//getchar();
		for (ii=0;ii<meshdata[i].n_points;ii++)
		{
			dpos+=dx/2.0;
			meshdata[i].dmesh[ii]=dpos;
			dpos+=dx/2.0;
			dx*=meshdata[i].mul;
			pos++;
		}
		len+=meshdata[i].len;
	}

	(*ret_len)=len;

	len=0.0;
	pos=0;
	for (i=0;i<meshlayers;i++)
	{
		for (ii=0;ii<meshdata[i].n_points;ii++)
		{
			if (meshdata[i].left_right==FALSE)
			{
				mesh[pos]=len+meshdata[i].dmesh[ii];
			}else
			{
				mesh[pos]=len+meshdata[i].len-meshdata[i].dmesh[meshdata[i].n_points-1-ii];
			}
			//printf("%c %ld %Le %d %d %ld\n",direction,pos,mesh[pos],i,ii,meshdata[i].n_points);
			pos++;
		}
		len+=meshdata[i].len;
	}

	long double last=0.0;
	long double next=0.0;
	long double sum=0.0;
	for (i=0;i<meshpoints;i++)
	{
		if ((meshpoints-1)==i)
		{
			next=(*ret_len);
		}else
		{
			next=(mesh[i]+mesh[i+1])/2.0;
		}
		
		dx=next-last;
		dmesh[i]=dx;
		sum=sum+dx;
		last=next;
	}

}

void mesh_dump_x(struct simulation *sim,struct device *in)
{
	int x=0;

	for (x=0;x<in->xmeshpoints;x++)
	{
		printf("%Le\n",in->xmesh[x]);
	}
}

void mesh_dump_y(struct simulation *sim,struct device *in)
{
	int y=0;

	for (y=0;y<in->ymeshpoints;y++)
	{
		printf("%Le\n",in->ymesh[y]);
	}
}

void mesh_load(struct simulation *sim,struct device *in)
{
	mesh_load_file(sim,&(in->zmeshpoints),&(in->remesh_z),&(in->zmeshlayers), &(in->meshdata_z),"mesh_z.inp");
	mesh_load_file(sim,&(in->xmeshpoints),&(in->remesh_x),&(in->xmeshlayers), &(in->meshdata_x),"mesh_x.inp");
	mesh_load_file(sim,&(in->ymeshpoints),&(in->remesh_y),&(in->ymeshlayers), &(in->meshdata_y),"mesh_y.inp");
}


void mesh_build(struct simulation *sim,struct device *in)
{

	int pos=0;
	int z=0;
	int x=0;
	int y=0;

	gdouble dpos=0.0;
	long double dx=0.0;
	long double len=0.0;

	//printf("%d\n",in->meshdata_y[0].n_points);
	//getchar();
	mesh_build_from_submesh(sim,in,'z');
	mesh_build_from_submesh(sim,in,'x');
	mesh_build_from_submesh(sim,in,'y');

	//mesh_dump_y(sim,in);
	//getchar();
	//in->zmesh[pos]=dpos;
	//in->xmesh[pos]=dpos;
	//in->ymesh[pos]=dpos;

	len=0.0;
	for (y=0;y<in->ymeshpoints;y++)
	{
		dpos=in->ymesh[y];
		in->imat[0][0][y]=epitaxy_get_electrical_material_layer(&(in->my_epitaxy),dpos);
		in->imat_epitaxy[0][0][y]=epitaxy_get_epitaxy_layer_using_electrical_pos(&(in->my_epitaxy),dpos);

		for (z=0;z<in->zmeshpoints;z++)
		{
			for (x=0;x<in->xmeshpoints;x++)
			{
				in->imat[z][x][y]=in->imat[0][0][y];
			}
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
//printf_log("%d\n",in->imat[i]);
}
}
