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

/** @file fdtd_solve.c
	@brief Solve the FDTD field on the CPU or get the GPU to do it.
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
#include <inp.h>
#include <sim.h>
#include <log.h>
#include <fdtd.h>

#include "vec.h"


void solve_E(struct simulation *sim,struct fdtd_data *data)
{
	int z=0;
	int y=0;
	//float Cx=(data->dt2/(epsilon0f*data->dx));
	float Cy=(data->dt2/(epsilon0f*data->dy));
	float Cz=(data->dt2/(epsilon0f*data->dz));


	for (y=1;y<data->ylen-1;y++)
	{
		for (z=1;z<(data->zlen-1);z++)
		{
			data->Ex[z][y]=data->Ex_last[z][y]+(data->Hz[z][y]-data->Hz[z][y-1])*data->epsilon_r[z][y]*Cy-(data->Hy[z][y]-data->Hy[z-1][y])*data->epsilon_r[z][y]*Cz;
			data->Ey[z][y]=data->Ey_last[z][y]+(data->Hx[z][y]-data->Hx[z-1][y])*data->epsilon_r[z][y]*Cz;
			data->Ez[z][y]=data->Ez_last[z][y]-(data->Hx[z][y]-data->Hx[z][y-1])*data->epsilon_r[z][y]*Cy;

		}
	}

	for (y=1;y<data->ylen-1;y++)
	{
		for (z=1;z<(data->zlen-1);z++)
		{
			if (z==1)
			{
				float Ex_last_l=data->Ex_last[z-1][y];
				float Ey_last_l=data->Ey_last[z-1][y];
				float Ez_last_l=data->Ez_last[z-1][y];
				data->Ex[z-1][y]=data->Ex_last[z][y]+((clf*data->dt2-data->dz)/(clf*data->dt2+data->dz))*(data->Ex[z][y]-Ex_last_l);
				data->Ey[z-1][y]=data->Ey_last[z][y]+((clf*data->dt2-data->dz)/(clf*data->dt2+data->dz))*(data->Ey[z][y]-Ey_last_l);
				data->Ez[z-1][y]=data->Ez_last[z][y]+((clf*data->dt2-data->dz)/(clf*data->dt2+data->dz))*(data->Ez[z][y]-Ez_last_l);
			}

			if (z==data->zlen-2)
			{
				float Ex_last_r=data->Ex_last[z+1][y];
				float Ey_last_r=data->Ey_last[z+1][y];
				float Ez_last_r=data->Ez_last[z+1][y];
				data->Ex[z+1][y]=data->Ex_last[z][y]+((clf*data->dt2-data->dz)/(clf*data->dt2+data->dz))*(data->Ex[z][y]-Ex_last_r);
				data->Ey[z+1][y]=data->Ey_last[z][y]+((clf*data->dt2-data->dz)/(clf*data->dt2+data->dz))*(data->Ey[z][y]-Ey_last_r);
				data->Ez[z+1][y]=data->Ez_last[z][y]+((clf*data->dt2-data->dz)/(clf*data->dt2+data->dz))*(data->Ez[z][y]-Ez_last_r);
			}

			if (y==1)
			{
				float Ex_last_d=data->Ex_last[z][y-1];
				float Ey_last_d=data->Ey_last[z][y-1];
				float Ez_last_d=data->Ez_last[z][y-1];
				data->Ex[z][y-1]=data->Ex_last[z][y]+((clf*data->dt2-data->dy)/(clf*data->dt2+data->dy))*(data->Ex[z][y]-Ex_last_d);
				data->Ey[z][y-1]=data->Ey_last[z][y]+((clf*data->dt2-data->dy)/(clf*data->dt2+data->dy))*(data->Ey[z][y]-Ey_last_d);
				data->Ez[z][y-1]=data->Ez_last[z][y]+((clf*data->dt2-data->dy)/(clf*data->dt2+data->dy))*(data->Ez[z][y]-Ez_last_d);
			}

			if (y==(data->ylen-2))
			{
				float Ex_last_u=data->Ex_last[z][y+1];
				float Ey_last_u=data->Ey_last[z][y+1];
				float Ez_last_u=data->Ez_last[z][y+1];
				//printf("%ld %le %le\n",dt2,dx,clight);
		
				data->Ex[z][y+1]=data->Ex_last[z][y]+((clf*data->dt2-data->dy)/(clf*data->dt2+data->dy))*(data->Ex[z][y]-Ex_last_u);
				data->Ey[z][y+1]=data->Ey_last[z][y]+((clf*data->dt2-data->dy)/(clf*data->dt2+data->dy))*(data->Ey[z][y]-Ey_last_u);
				data->Ez[z][y+1]=data->Ez_last[z][y]+((clf*data->dt2-data->dy)/(clf*data->dt2+data->dy))*(data->Ez[z][y]-Ez_last_u);

			}

		}
	}

}



void solve_H(struct simulation *sim,struct fdtd_data *data)
{

//int thread;
int z;
int y;
float Cy=(data->dt2/(mu0f*data->dy));
//float Cx=(data->dt2/(mu0f*data->dx));
float Cz=(data->dt2/(mu0f*data->dy));

for (y=0;y<(data->ylen-1);y++)
{
	for (z=0;z<(data->zlen-1);z++)
	{
		data->Hx[z][y]=data->Hx_last[z][y]-(data->Ez[z][y+1]-data->Ez[z][y])*Cy+(data->Ey[z+1][y]-data->Ey[z][y])*Cz;
		data->Hy[z][y]=data->Hy_last[z][y]-(data->Ex[z+1][y]-data->Ex[z][y])*Cz;
		data->Hz[z][y]=data->Hz_last[z][y]+(data->Ex[z][y+1]-data->Ex[z][y])*Cy;
	}
}

}

void fdtd_excitation(struct simulation *sim,struct fdtd_data *data)
{
	int j=data->excitation_mesh_point;
	int i=data->excitation_mesh_point;
	//for (j=0;j<data->ylen;j++)
	{
		//for (i=0;i<data->zlen;i++)
		{
			//if (j==data->excitation_mesh_point)
			{
				//if ((y[j]>src_start)&&(y[j]<src_stop))
				{
					float start=data->y_mesh[1];
					float a=0.0;
					float b=0.0;
					float c=0.0;
					float phi=0.0;
					float theta=0.0;


					float dot=0.0;
					phi=0.0;
					theta=90.0;

					float shift=-2.0;
					a=sin(theta*(2.0*M_PI/360.0))*cos(phi*(2.0*M_PI/360.0));
					b=sin(theta*(2.0*M_PI/360.0))*sin(phi*(2.0*M_PI/360.0));
					c=cos(theta*(2.0*M_PI/360.0));

					dot=tan(2.0*M_PI*(shift)/360.0)*(data->y_mesh[j]-start)*2.0*M_PI/data->lambda;
					float mod=1.0;
					//dot=0.0;
					data->Ex[i][j]=mod*a*sin(dot-data->time*data->omega);
					data->Ey[i][j]=mod*b*sin(dot-data->time*data->omega);
					data->Ez[i][j]=mod*c*sin(dot-data->time*data->omega);
				}
			}
		}
	}


///////////////////////////

}

void fdtd_time_step(struct simulation *sim,struct fdtd_data *data)
{
int i;

	for (i=0;i<data->zlen;i++)
	{

		memcpy ( data->Ex_last[i], data->Ex[i], sizeof(float)*data->ylen );
		memcpy ( data->Ey_last[i], data->Ey[i], sizeof(float)*data->ylen );
		memcpy ( data->Ez_last[i], data->Ez[i], sizeof(float)*data->ylen );
	}

	for (i=0;i<data->zlen;i++)
	{

		memcpy ( data->Hx_last[i], data->Hx[i], sizeof(float)*data->ylen );
		memcpy ( data->Hy_last[i], data->Hy[i], sizeof(float)*data->ylen );
		memcpy ( data->Hz_last[i], data->Hz[i], sizeof(float)*data->ylen );
	}

	data->time+=data->dt;
}
void fdtd_solve_step(struct simulation *sim,struct fdtd_data *data)
{
	int ret=-1;

	if (data->use_gpu==TRUE)
	{
		ret=fdtd_opencl_solve_step(sim,data);
	}

	if (ret==-1)
	{
		solve_E(sim,data);
		fdtd_excitation(sim,data);
		solve_H(sim,data);
	}

	//getchar();
	fdtd_time_step(sim,data);
	//usleep(100);
}


