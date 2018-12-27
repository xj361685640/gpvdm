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
	int x=0;
	int y=0;
	//float Cx=(data->dt2/(epsilon0f*data->dx));
	float Cy=(data->dt2/(epsilon0f*data->dy));
	float Cx=(data->dt2/(epsilon0f*data->dx));


	for (z=0;z<data->zlen;z++)
	{
		for (x=1;x<data->xlen-1;x++)
		{
			for (y=1;y<(data->ylen-1);y++)
			{
				data->Ex[z][x][y]=data->Ex_last[z][x][y]+(data->Hz[z][x][y]-data->Hz[z][x][y-1])*data->epsilon_r[z][x][y]*Cy-(data->Hy[z][x][y]-data->Hy[z][x-1][y])*data->epsilon_r[z][x][y]*Cx;
				data->Ey[z][x][y]=data->Ey_last[z][x][y]+(data->Hx[z][x][y]-data->Hx[z][x-1][y])*data->epsilon_r[z][x][y]*Cx;
				data->Ez[z][x][y]=data->Ez_last[z][x][y]-(data->Hx[z][x][y]-data->Hx[z][x][y-1])*data->epsilon_r[z][x][y]*Cx;

			}
		}
	}


	for (z=0;z<data->zlen;z++)
	{

		for (x=1;x<data->xlen-1;x++)
		{
			for (y=1;y<(data->ylen-1);y++)
			{
				if (x==1)
				{
					float Ex_last_l=data->Ex_last[z][x-1][y];
					float Ey_last_l=data->Ey_last[z][x-1][y];
					float Ez_last_l=data->Ez_last[z][x-1][y];
					data->Ex[z][x-1][y]=data->Ex_last[z][x][y]+((clf*data->dt2-data->dx)/(clf*data->dt2+data->dx))*(data->Ex[z][x][y]-Ex_last_l);
					data->Ey[z][x-1][y]=data->Ey_last[z][x][y]+((clf*data->dt2-data->dx)/(clf*data->dt2+data->dx))*(data->Ey[z][x][y]-Ey_last_l);
					data->Ez[z][x-1][y]=data->Ez_last[z][x][y]+((clf*data->dt2-data->dz)/(clf*data->dt2+data->dx))*(data->Ez[z][x][y]-Ez_last_l);
				}

				if (x==data->xlen-2)
				{
					float Ex_last_r=data->Ex_last[z][x+1][y];
					float Ey_last_r=data->Ey_last[z][x+1][y];
					float Ez_last_r=data->Ez_last[z][x+1][y];
					data->Ex[z][x+1][y]=data->Ex_last[z][x][y]+((clf*data->dt2-data->dx)/(clf*data->dt2+data->dx))*(data->Ex[z][x][y]-Ex_last_r);
					data->Ey[z][x+1][y]=data->Ey_last[z][x][y]+((clf*data->dt2-data->dx)/(clf*data->dt2+data->dx))*(data->Ey[z][x][y]-Ey_last_r);
					data->Ez[z][x+1][y]=data->Ez_last[z][x][y]+((clf*data->dt2-data->dx)/(clf*data->dt2+data->dx))*(data->Ez[z][x][y]-Ez_last_r);
				}

				if (y==1)
				{
					float Ex_last_d=data->Ex_last[z][x][y-1];
					float Ey_last_d=data->Ey_last[z][x][y-1];
					float Ez_last_d=data->Ez_last[z][x][y-1];
					data->Ex[z][x][y-1]=data->Ex_last[z][x][y]+((clf*data->dt2-data->dy)/(clf*data->dt2+data->dy))*(data->Ex[z][x][y]-Ex_last_d);
					data->Ey[z][x][y-1]=data->Ey_last[z][x][y]+((clf*data->dt2-data->dy)/(clf*data->dt2+data->dy))*(data->Ey[z][x][y]-Ey_last_d);
					data->Ez[z][x][y-1]=data->Ez_last[z][x][y]+((clf*data->dt2-data->dy)/(clf*data->dt2+data->dy))*(data->Ez[z][x][y]-Ez_last_d);
				}

				if (y==(data->ylen-2))
				{
					float Ex_last_u=data->Ex_last[z][x][y+1];
					float Ey_last_u=data->Ey_last[z][x][y+1];
					float Ez_last_u=data->Ez_last[z][x][y+1];
					//printf("%ld %le %le\n",dt2,dx,clight);
			
					data->Ex[z][x][y+1]=data->Ex_last[z][x][y]+((clf*data->dt2-data->dy)/(clf*data->dt2+data->dy))*(data->Ex[z][x][y]-Ex_last_u);
					data->Ey[z][x][y+1]=data->Ey_last[z][x][y]+((clf*data->dt2-data->dy)/(clf*data->dt2+data->dy))*(data->Ey[z][x][y]-Ey_last_u);
					data->Ez[z][x][y+1]=data->Ez_last[z][x][y]+((clf*data->dt2-data->dy)/(clf*data->dt2+data->dy))*(data->Ez[z][x][y]-Ez_last_u);

				}

			}
		}

	}
}



void solve_H(struct simulation *sim,struct fdtd_data *data)
{

//int thread;
int z;
int x;
int y;
float Cy=(data->dt2/(mu0f*data->dy));
//float Cx=(data->dt2/(mu0f*data->dx));
float Cx=(data->dt2/(mu0f*data->dy));

for (z=0;z<data->zlen;z++)
{
	for (x=0;x<(data->xlen-1);x++)
	{
		for (y=0;y<(data->ylen-1);y++)
		{
			data->Hx[z][x][y]=data->Hx_last[z][x][y]-(data->Ez[z][x][y+1]-data->Ez[z][x][y])*Cy+(data->Ey[z][x+1][y]-data->Ey[z][x][y])*Cx;
			data->Hy[z][x][y]=data->Hy_last[z][x][y]-(data->Ex[z][x+1][y]-data->Ex[z][x][y])*Cx;
			data->Hz[z][x][y]=data->Hz_last[z][x][y]+(data->Ex[z][x][y+1]-data->Ex[z][x][y])*Cy;
		}
	}

}

}

void fdtd_excitation(struct simulation *sim,struct fdtd_data *data)
{
	int z=0;
	int x=data->excitation_mesh_point;
	int y=data->excitation_mesh_point;
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

					dot=tan(2.0*M_PI*(shift)/360.0)*(data->y_mesh[y]-start)*2.0*M_PI/data->lambda;
					float mod=1.0;
					//dot=0.0;
					data->Ex[z][x][y]=mod*a*sin(dot-data->time*data->omega);
					data->Ey[z][x][y]=mod*b*sin(dot-data->time*data->omega);
					data->Ez[z][x][y]=mod*c*sin(dot-data->time*data->omega);
				}
			}
		}
	}


///////////////////////////

}

void fdtd_time_step(struct simulation *sim,struct fdtd_data *data)
{
	fdtd_memcpy(data, data->Ez_last, data->Ez);
	fdtd_memcpy(data, data->Ex_last, data->Ex);
	fdtd_memcpy(data, data->Ey_last, data->Ey);

	fdtd_memcpy(data, data->Hz_last, data->Hz);
	fdtd_memcpy(data, data->Hx_last, data->Hx);
	fdtd_memcpy(data, data->Hy_last, data->Hy);

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


